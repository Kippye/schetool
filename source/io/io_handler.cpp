#include <algorithm>
#include <chrono>
#include <filesystem>
#include <limits>
#include <io_handler.h>

namespace fs = std::filesystem;

bool IO_Handler::isAutosave(const std::string& name)
{
    return name.rfind(m_autosaveSuffix) != std::string::npos;
}

std::string IO_Handler::getFileAutosaveName(const char* name)
{
    return std::string(name).append(m_autosaveSuffix);
}

std::string IO_Handler::makeRelativePathFromName(const char* name)
{
    return std::string(SCHEDULES_SUBDIR_PATH).append(std::string(name)).append(std::string(SCHEDULE_FILE_EXTENSION));
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
            return true;
        }
    }
    // Failed to copy
    catch (fs::filesystem_error& e)
    {
        printf("Could not copy autosave %s to file %s: %s\n", pathToAutosaveFile.string().c_str(), pathToFile.string().c_str(), e.what());
        return false;
    }

    return false;
}

void IO_Handler::init(Schedule* schedule, Window* window, Input& input, Interface& interface)
{
    m_schedule = schedule;
    m_windowManager = window;
    input.addEventListener(INPUT_EVENT_SC_SAVE, saveListener);

    m_mainMenuBarGui = interface.getGuiByID<MainMenuBarGui>("MainMenuBarGui");
    m_mainMenuBarGui->getSubGui<ScheduleNameModalSubGui>("ScheduleNameModalSubGui")->renameScheduleEvent.addListener(renameListener);
    m_mainMenuBarGui->getSubGui<ScheduleNameModalSubGui>("ScheduleNameModalSubGui")->createNewScheduleEvent.addListener(createNewListener);

    m_mainMenuBarGui->getSubGui<ScheduleDeleteModalSubGui>("ScheduleDeleteModalSubGui")->deleteScheduleEvent.addListener(deleteListener);

    m_mainMenuBarGui->openScheduleFileEvent.addListener(openListener);
    m_mainMenuBarGui->saveEvent.addListener(saveListener);
    m_mainMenuBarGui->passFileNames(getScheduleStemNamesSortedByEditTime());

    m_converter = DataConverter();
    m_converter.setupObjectTable();
}

void IO_Handler::closeCurrentFile()
{
    createAutosave();
    applyAutosaveToFile(m_openScheduleFilename.c_str());
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
    m_mainMenuBarGui->passFileNames(getScheduleStemNamesSortedByEditTime());
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
    if (m_converter.readSchedule(relativePath.c_str(), m_schedule->getAllColumnsMutable()) == 0)
    {
        std::cout << "Successfully read Schedule from file: " << relativePath << std::endl;
    }
    m_schedule->sortColumns();
    setOpenScheduleFilename(std::string(name));
    m_schedule->getEditHistoryMutable().setEditedSinceWrite(false);
    return true;
}

bool IO_Handler::createNewSchedule(const char* name)
{
    m_schedule->createDefaultSchedule();

    if (writeSchedule(name)) // passes new list of file names to gui
    {
        setOpenScheduleFilename(std::string(name));
        return true;
    }
    return false;
}
 
// Deletes the Schedule with the name and returns true if it exists or returns false.
// NOTE: This can delete the currently open file. Is this the correct behaviour?
bool IO_Handler::deleteSchedule(const char* name)
{
    std::string relativePath = makeRelativePathFromName(name);

    if (fs::exists(relativePath) == false)
    {
        std::cout << "Tried to delete non-existant Schedule:" << name << std::endl;
        return false;
    }

    if (fs::remove(relativePath) )
    {
        m_mainMenuBarGui->passFileNames(getScheduleStemNamesSortedByEditTime());
        return true;
    }

    return false;
}

bool IO_Handler::createAutosave()
{
    if (writeSchedule(getFileAutosaveName(m_openScheduleFilename.c_str()).c_str()))
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

std::string IO_Handler::getOpenScheduleFilename()
{
    return m_openScheduleFilename;
}

bool IO_Handler::setOpenScheduleFilename(const std::string& name, bool renameFile)
{
    if (renameFile)
    {
        fs::path schedulesPath = fs::path(SCHEDULES_SUBDIR_PATH);
        fs::path pathToOpenFile = fs::path(makeRelativePathFromName(m_openScheduleFilename.c_str()));
        fs::path pathToRenamedFile = fs::path(makeRelativePathFromName(name.c_str()));
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
            writeSchedule(name.c_str());
            std::cout << "Wrote current file to renamed path:" << pathToRenamedFile.string() << std::endl;
        }
        else // All is fine, rename the file
        {
            fs::rename(pathToOpenFile, pathToRenamedFile);
        }
    }

    m_openScheduleFilename = name;
    m_schedule->setName(name);
    m_windowManager->setTitleSuffix(std::string(" - ").append(m_schedule->getName()).c_str());
    m_mainMenuBarGui->passFileNames(getScheduleStemNamesSortedByEditTime());
    return true;
}

std::vector<std::string> IO_Handler::getScheduleStemNames()
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
        filenames.push_back(entry.path().stem().string());
    }
    return filenames;
}

std::vector<std::string> IO_Handler::getScheduleStemNamesSortedByEditTime()
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
        filenames.push_back(entry.path().stem().string());
        filenamesEditTimes.insert({entry.path().stem().string(), fs::last_write_time(entry).time_since_epoch().count()}); 
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
        fs::file_time_type editTime = fs::last_write_time(entry);

        if (latestEditTime == std::numeric_limits<long long>::min() || editTime.time_since_epoch().count() > latestEditTime)
        {
            latestEditedPath = entry;
            latestEditTime = editTime.time_since_epoch().count();
        }
    }

    return latestEditedPath.stem().string();
}