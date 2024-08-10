#include <algorithm>
#include <optional>
#include <chrono>
#include <limits>
#include <io_handler.h>

namespace fs = std::filesystem;

void IO_Handler::init(Schedule* schedule, Window* window, Input& input, Interface& interface)
{
    m_schedule = schedule;
    m_windowManager = window;
    m_converter = DataConverter();
    m_converter.setupObjectTable();
    
    window->windowCloseEvent.addListener(windowCloseListener);
    input.addEventListener(INPUT_EVENT_SC_SAVE, saveListener);

    m_startPageGui = interface.getGuiByID<StartPageGui>("StartPageGui");
    m_startPageGui->getSubGui<StartPageNewNameModalSubGui>("StartPageNewNameModalSubGui")->createNewScheduleEvent.addListener(createNewListener);
    m_startPageGui->openScheduleFileEvent.addListener(openListener);

    m_scheduleGui = interface.getGuiByID<ScheduleGui>("ScheduleGui");

    m_mainMenuBarGui = interface.getGuiByID<MainMenuBarGui>("MainMenuBarGui");
    m_mainMenuBarGui->getSubGui<RenameModalSubGui>("RenameModalSubGui")->renameScheduleEvent.addListener(renameListener);
    m_mainMenuBarGui->getSubGui<NewNameModalSubGui>("NewNameModalSubGui")->createNewScheduleEvent.addListener(createNewListener);
    m_mainMenuBarGui->getSubGui<DeleteModalSubGui>("DeleteModalSubGui")->deleteScheduleEvent.addListener(deleteListener);

    m_mainMenuBarGui->openScheduleFileEvent.addListener(openListener);
    m_mainMenuBarGui->saveEvent.addListener(saveListener);

    m_autosavePopupGui = interface.getGuiByID<AutosavePopupGui>("AutosavePopupGui");
    m_autosavePopupGui->openAutosaveEvent.addListener(openAutosaveListener);
    m_autosavePopupGui->ignoreAutosaveOpenFileEvent.addListener(ignoreAutosaveOpenFileEvent);

    passFileNamesToGui();
}

// Returns true if the path has the schedule file extension and is considered a valid file by the DataConverter
bool IO_Handler::isScheduleFilePath(const fs::path& path) const
{
    return (strcmp(path.extension().string().c_str(), SCHEDULE_FILE_EXTENSION) == 0 && m_converter.isValidScheduleFile(makeRelativePathFromName(path.stem().string().c_str()).c_str()));
}

std::string IO_Handler::makeRelativePathFromName(const char* name) const
{
    return std::string(SCHEDULES_SUBDIR_PATH).append(std::string(name)).append(std::string(SCHEDULE_FILE_EXTENSION));
}

void IO_Handler::passFileNamesToGui()
{
    m_startPageGui->passFileNames(getScheduleStemNamesSortedByEditTime(true));
    m_mainMenuBarGui->passFileNames(getScheduleStemNamesSortedByEditTime(false));
}

void IO_Handler::sendFileInfoUpdates()
{
    m_windowManager->setTitleSuffix(std::string(" - ").append(m_currentFileInfo.getName()).c_str());
    m_schedule->setName(m_currentFileInfo.getName());
    m_mainMenuBarGui->passHaveFileOpen(m_currentFileInfo.empty() == false);
}

void IO_Handler::unloadCurrentFile()
{
    m_schedule->clearSchedule();
    m_schedule->getEditHistoryMutable().clearEditHistory();
    m_currentFileInfo.clear();
    sendFileInfoUpdates();
    fileUnloadEvent.invoke();
}

void IO_Handler::closeCurrentFile()
{
    if (m_currentFileInfo.empty()) { return; }

    createAutosave();
    if (isAutosave(m_currentFileInfo.getName()) == false)
    {
        applyAutosaveToFile(m_currentFileInfo.getName().c_str());
    }

    unloadCurrentFile();
}

bool IO_Handler::applyAutosaveToFile(const char* fileName)
{
    fs::path pathToFile = fs::path(makeRelativePathFromName(fileName));
    fs::path pathToAutosaveFile = fs::path(makeRelativePathFromName(getFileAutosaveName(fileName).c_str()));

    // A Schedule file with this path does not exist. stop.
    if (fs::exists(pathToFile) == false)
    {
        printf("IO_Handler::applyAutosaveToFile(%s): Base file not found at path %s\n", fileName, pathToFile.string().c_str());
        return false;
    }
    if (fs::exists(pathToAutosaveFile) == false)
    {
        printf("IO_Handler::applyAutosaveToFile(%s): Autosave file not found at path %s\n", fileName, pathToAutosaveFile.string().c_str());
        return false;
    }

    // NOTE: It seems std::filesystem::copy_file cannot actually overwrite existing files (on windows?)
    // But i don't want to create OS-specific things here so i will simply delete the existing file, copy the autosave and then delete the autosave.
    // Probably many more opportunities for errors, but eh.
    // TODO: To minimise the risk of losing data, i could rename the old file instead, only deleting it once everything else is done successfully

    // Delete the base file
    fs::remove(pathToFile);
    try
    {
        if (fs::copy_file(pathToAutosaveFile, pathToFile, fs::copy_options::update_existing))
        {
            // The file was successfully copied, which means the autosave can be removed
            fs::remove(pathToAutosaveFile);
            passFileNamesToGui();
            return true;
        }
    }
    // Failed to copy
    catch (fs::filesystem_error& e)
    {
        printf("Could not copy autosave %s to file %s: %s\n", pathToAutosaveFile.string().c_str(), pathToFile.string().c_str(), e.what());
        return false;
    }
    passFileNamesToGui();

    return false;
}

bool IO_Handler::writeSchedule(const char* name)
{
    fs::path schedulesPath = fs::path(SCHEDULES_SUBDIR_PATH);
    if (fs::exists(schedulesPath) == false)
    {
        std::cout << "Tried to write Schedule but the schedules directory did not exist. Created a schedules directory." << std::endl;
        fs::create_directory(schedulesPath);
    }

    std::string relativePath = makeRelativePathFromName(name);

    if (m_converter.writeSchedule(relativePath.c_str(), m_schedule->getAllColumns()) == 0)
    {
        std::cout << "Successfully wrote Schedule to file: " << relativePath << std::endl;
    }
    // TODO: make some event that the Schedule can listen to?
    m_schedule->getEditHistoryMutable().setEditedSinceWrite(false);
    passFileNamesToGui();
    return true;
}

// Reads a schedule from file and applies / opens it
bool IO_Handler::readSchedule(const char* name)
{    
    std::string relativePath = makeRelativePathFromName(name);
    
    if (fs::exists(fs::path(relativePath.c_str())) == false)
    {
        std::cout << "Tried to read Schedule at path to non-existant file:" << relativePath << std::endl;
        return false;
    }

    m_schedule->getEditHistoryMutable().clearEditHistory();
    if (std::optional<FileInfo> readFileInfo = m_converter.readSchedule(relativePath.c_str(), m_schedule->getAllColumnsMutable()))
    {
        std::cout << "Successfully read Schedule from file: " << relativePath << std::endl;
        m_schedule->sortColumns();
        m_currentFileInfo.fill(std::string(name), getFileEditTimeWrapped(fs::path(relativePath)), readFileInfo->getScheduleEditTime());
        fileReadEvent.invoke(m_currentFileInfo);
        sendFileInfoUpdates();
        m_startPageGui->setVisible(false);
        m_scheduleGui->setVisible(true);
        m_schedule->getEditHistoryMutable().setEditedSinceWrite(false);
        return true;
    }
    else
    {
        std::cout << "Failed when reading Schedule from file: " << relativePath << std::endl;
        return false;
    }
}

bool IO_Handler::createNewSchedule(const char* name)
{
    m_schedule->createDefaultSchedule();

    if (writeSchedule(name)) // passes new list of file names to gui
    {
        fs::path createdFilePath = fs::path(makeRelativePathFromName(name));
        m_currentFileInfo.fill(std::string(name), getFileEditTimeWrapped(createdFilePath), DateContainer::getCurrentSystemDate().getTime());
        sendFileInfoUpdates();
        passFileNamesToGui();
        m_startPageGui->setVisible(false);
        m_scheduleGui->setVisible(true);
        return true;
    }
    return false;
}
 
// Deletes the Schedule with the name and returns true if it exists or returns false.
// NOTE: This can delete the currently open file. Is this the correct behaviour?
bool IO_Handler::deleteSchedule(const char* name)
{
    if (m_currentFileInfo.empty()) { return false; }

    std::string relativePath = makeRelativePathFromName(name);

    if (fs::exists(relativePath) == false)
    {
        std::cout << "Tried to delete non-existant Schedule:" << name << std::endl;
        return false;
    }

    std::string autosavePath = makeRelativePathFromName(getFileAutosaveName(name).c_str());

    // delete the file's autosave if it exists
    if (fs::exists(autosavePath))
    {
        fs::remove(autosavePath);
    }

    if (fs::remove(relativePath) )
    {
        passFileNamesToGui();
        // deleted the file that was open
        if (m_currentFileInfo.getName() == name)
        {
            unloadCurrentFile();
            m_scheduleGui->setVisible(false);
            m_startPageGui->setVisible(true);
        }
        return true;
    }

    return false;
}

bool IO_Handler::renameCurrentFile(const std::string& newName)
{
    if (m_currentFileInfo.empty()) { return false; }

    fs::path schedulesPath = fs::path(SCHEDULES_SUBDIR_PATH);
    fs::path pathToOpenFile = fs::path(makeRelativePathFromName(m_currentFileInfo.getName().c_str()));
    fs::path pathToRenamedFile = fs::path(makeRelativePathFromName(newName.c_str()));
    bool schedulesDirWasCreated = false;

    // Write-operation: Create schedules directory if it doesn't exist.
    if (fs::exists(schedulesPath) == false)
    {
        std::cout << "Tried to rename a Schedule but the schedules directory did not exist. Created a schedules directory." << std::endl;
        fs::create_directory(schedulesPath);
        schedulesDirWasCreated = true;
    }
    //  A Schedule file with this name already exists, don't overwrite it. Just stop.
    if (fs::exists(pathToRenamedFile))
    {
        return false;
    }
    // If the file to rename doesn't exist, just write the Schedule to the file with the provided new name
    if (schedulesDirWasCreated || fs::exists(pathToOpenFile) == false)
    {
        std::cout << "Tried to change the name of the Schedule file, but the file was not found at its previous path:" << pathToOpenFile.string() << std::endl;
        writeSchedule(newName.c_str());
        std::cout << "Wrote current file to renamed path:" << pathToRenamedFile.string() << std::endl;
    }
    else // All is fine, rename the file
    {
        fs::rename(pathToOpenFile, pathToRenamedFile);
    }
    // Rename the autosave as well, if it exists
    fs::path pathToAutosave = fs::path(makeRelativePathFromName(getFileAutosaveName(pathToOpenFile.stem().string().c_str()).c_str()));
    fs::path pathToRenamedAutosave = fs::path(makeRelativePathFromName(getFileAutosaveName(newName.c_str()).c_str()));
    if (fs::exists(pathToAutosave))
    {
        fs::rename(pathToAutosave, pathToRenamedAutosave);
    }

    passFileNamesToGui();

    m_currentFileInfo.rename(newName);
    sendFileInfoUpdates();

    return true;
}

FileInfo IO_Handler::getCurrentFileInfo() const
{
    return m_currentFileInfo;
}

void IO_Handler::openMostRecentFile()
{
    // LAMBDA
    auto goToStartPage = [&] ()
    {
        m_scheduleGui->setVisible(false);
		m_startPageGui->setVisible(true);
    };
    // There are pre-existing Schedules. Open the most recently edited one.
	if (getScheduleStemNames().size() > 0)
	{
        std::string lastEditedScheduleName = getLastEditedScheduleStemName();

        // the most recently edited file is an autosave, the program might not have been closed correctly.
        // show autosave prompt popup
        if (isAutosave(lastEditedScheduleName))
        {
            std::string fileBaseName = getFileBaseName(lastEditedScheduleName.c_str());
            fs::path fileBasePath = fs::path(makeRelativePathFromName(fileBaseName.c_str()));

            // The autosave has a base file, ask which to open
            if (std::filesystem::exists(fs::path(makeRelativePathFromName(fileBaseName.c_str()))))
            {
                fs::path autosavePath = fs::path(makeRelativePathFromName(lastEditedScheduleName.c_str()));

                // TODO: Make this nicer. Note that we can't pass the scheduleEditTime since the files aren't actually being loaded until the user chooses to.
                m_autosavePopupGui->open(
                    FileInfo(fileBaseName, getFileEditTimeWrapped(fileBasePath), TimeWrapper()),
                    FileInfo(lastEditedScheduleName, getFileEditTimeWrapped(autosavePath), TimeWrapper()),
                    getFileEditTimeString(fileBasePath),
                    getFileEditTimeString(fs::path(autosavePath))
                );
            }
            // Somehow there is only an autosave and no base file.
            // Don't know what to do, open the start page and let the user decide?
            else
            {
                goToStartPage();
            }
        }
        // the most recent file is a normal file, read it
        else
        {
		    readSchedule(getLastEditedScheduleStemName().c_str());
        }
	}
	// There are no Schedule files. Open the Start Page so the user can create one from there or File->New.
	else
	{
        goToStartPage();
	}
}

bool IO_Handler::createAutosave()
{
    if (m_currentFileInfo.empty()) { return false; }

    // save to open file name if the open file is itself an autosave, otherwise get the autosave name from the base file name
    std::string autosaveName = isAutosave(m_currentFileInfo.getName()) ? m_currentFileInfo.getName() : getFileAutosaveName(m_currentFileInfo.getName().c_str());

    if (writeSchedule(autosaveName.c_str()))
    {
        return true;
    }

    return false;
}

void IO_Handler::addToAutosaveTimer(double delta)
{
    m_timeSinceAutosave += delta;

    if (m_timeSinceAutosave > (double)AUTOSAVE_DELAY_SECONDS)
    {
        if (m_schedule->getEditHistory().getEditedSinceWrite() == true)
        {
            createAutosave();
        }
        m_timeSinceAutosave = 0;
    }
}

bool IO_Handler::isAutosave(const std::string& name)
{
    return name.rfind(m_autosaveSuffix) != std::string::npos;
}

std::string IO_Handler::getFileAutosaveName(const char* name)
{
    return std::string(name).append(m_autosaveSuffix);
}

std::string IO_Handler::getFileBaseName(const char* autosaveName)
{
    std::string autosaveString = std::string(autosaveName);
    if (isAutosave(autosaveString) == false)
    {
        printf("IO_Handler::getFileBaseName(%s): File name is not an autosave\n", autosaveName);
    }

    return autosaveString.substr(0, autosaveString.rfind(m_autosaveSuffix));
}

long long IO_Handler::getFileEditTime(fs::path path)
{
    if (fs::exists(path) == false) { printf("IO_Handler::getFileEditTime(%s): No file exists at the path\n", path.string().c_str()); return 0; }

    return fs::last_write_time(path).time_since_epoch().count();
}

TimeWrapper IO_Handler::getFileEditTimeWrapped(fs::path path)
{
    if (fs::exists(path) == false) { printf("IO_Handler::getFileEditTimeWrapped(%s): No file exists at the path\n", path.string().c_str()); return TimeWrapper(); }

    const auto fileEditTime = fs::last_write_time(path);
    time_t time;

    #if defined(WIN32) && !defined(__clang__) // windows implementation using clock_cast (my version of clang / libc++ didn't have it either)
    const auto systemTime = std::chrono::clock_cast<std::chrono::system_clock>(fileEditTime);
    time = std::chrono::system_clock::to_time_t(systemTime);
    #else // workaround without clock_cast
    auto systemNow = std::chrono::system_clock::now();
    auto fileNow = std::chrono::file_clock::now();
    auto systemTime = std::chrono::time_point_cast<std::chrono::system_clock::duration>(fileEditTime - fileNow + systemNow);
    time = std::chrono::system_clock::to_time_t(systemTime);
    #endif
    // TODO: converting this chrono stuff or a time_t into a TimeWrapper
    // return TimeWrapper(*localtime(&time));
    return TimeWrapper::getCurrentTime();
}

std::string IO_Handler::getFileEditTimeString(fs::path path)
{
    if (fs::exists(path) == false) { printf("IO_Handler::getFileEditTimeString(%s): No file exists at the path\n", path.string().c_str()); return std::string(""); }

    return getFileEditTimeWrapped(path).getString(TIME_FORMAT_FULL);
}

std::vector<std::string> IO_Handler::getScheduleStemNames(bool includeAutosaves)
{
    std::vector<std::string> filenames = {};
    fs::path schedulesPath = fs::path(SCHEDULES_SUBDIR_PATH);
    if (fs::exists(schedulesPath) == false)
    {
        std::cout << "Tried to get Schedule stem names but the schedules directory did not exist." << std::endl;
        return filenames;
    }

    for (const auto& entry: fs::directory_iterator(schedulesPath))
    {
        // Skip non-regular files
        if (entry.is_regular_file() == false)
        {
            std::cout << "Skipped file " << entry.path().string() << " (entry.is_regular_file() == false)" << std::endl;
            continue;
        }
        // Skip non-schedule files
        if (isScheduleFilePath(entry.path()) == false)
        {
            std::cout << "Skipped file " << entry.path().string() << " (not a valid schedule file)" << std::endl;
            continue;
        }
        // Skip autosave files if includeAutosaves == false
        if (includeAutosaves == false && isAutosave(entry.path().stem().string()))
        {
            continue;
        }

        filenames.push_back(entry.path().stem().string());
    }
    return filenames;
}

std::vector<std::string> IO_Handler::getScheduleStemNamesSortedByEditTime(bool includeAutosaves)
{
    std::vector<std::string> filenames = {};
    std::map<std::string, long long> filenamesEditTimes = {};

    fs::path schedulesPath = fs::path(SCHEDULES_SUBDIR_PATH);
    if (fs::exists(schedulesPath) == false)
    {
        std::cout << "Tried to get sorted Schedule stem names but the schedules directory did not exist." << std::endl;
        return filenames;
    }

    for (const auto& entry: fs::directory_iterator(schedulesPath))
    {
        // Skip non-regular files
        if (entry.is_regular_file() == false)
        {
            std::cout << "Skipped file " << entry.path().string() << " (entry.is_regular_file() == false)" << std::endl;
            continue;
        }
        // Skip non-schedule files
        if (isScheduleFilePath(entry.path()) == false)
        {
            std::cout << "Skipped file " << entry.path().string() << " (not a valid schedule file)" << std::endl;
            continue;
        }
        // Skip autosave files if includeAutosaves == false
        if (includeAutosaves == false && isAutosave(entry.path().stem().string()))
        {
            continue;
        }

        filenames.push_back(entry.path().stem().string());
        filenamesEditTimes.insert({entry.path().stem().string(), getFileEditTime(entry)}); 
    }

    std::sort(filenames.begin(), filenames.end(), [&](std::string fs1, std::string fs2){ return filenamesEditTimes.at(fs1) > filenamesEditTimes.at(fs2); });
    return filenames;
}

std::string IO_Handler::getLastEditedScheduleStemName()
{
    fs::path schedulesPath = fs::path(SCHEDULES_SUBDIR_PATH);
    if (fs::exists(schedulesPath) == false)
    {
        std::cout << "Tried to get the latest edited Schedule, but the schedules directory did not exist." << std::endl;
        return std::string("");
    }

    long long latestEditTime = std::numeric_limits<long long>::min();
    fs::path latestEditedPath;
    fs::directory_iterator dirIterator = fs::directory_iterator(schedulesPath);

    for (const auto& entry: dirIterator)
    {
        // Skip non-regular files
        if (entry.is_regular_file() == false)
        {
            std::cout << "Skipped file " << entry.path().string() << " (entry.is_regular_file() == false)" << std::endl;
            continue;
        }
        // Skip non-schedule files
        if (isScheduleFilePath(entry.path()) == false)
        {
            std::cout << "Skipped file " << entry.path().string() << " (not a valid schedule file)" << std::endl;
            continue;
        }

        fs::file_time_type editTime = fs::last_write_time(entry);

        if (latestEditTime == std::numeric_limits<long long>::min() || editTime.time_since_epoch().count() > latestEditTime)
        {
            latestEditedPath = entry;
            latestEditTime = editTime.time_since_epoch().count();
        }
    }

    return latestEditedPath.stem().string();
}