#include <algorithm>
#include <optional>
#include <chrono>
#include <limits>
#include <format>
#include "schedule_io.h"

namespace fs = std::filesystem;

ScheduleIO::ScheduleIO(Schedule& schedule, Interface& programInterface, std::filesystem::path saveDir)
    : m_schedule(schedule), m_saveDir(saveDir) {
    m_converter = ScheduleDataConverter();
    m_converter.setupObjectTable();

    m_startPageGui = programInterface.getGuiByID<StartPageGui>("StartPageGui");
    m_startPageGui->getSubGui<StartPageNewNameModalSubGui>("StartPageNewNameModalSubGui")
        ->createNewScheduleEvent.addListener(createNewListener);
    m_startPageGui->openScheduleFileEvent.addListener(openListener);

    m_scheduleGui = programInterface.getGuiByID<ScheduleGui>("ScheduleGui");

    m_mainMenuBarGui = programInterface.getGuiByID<MainMenuBarGui>("MainMenuBarGui");
    m_mainMenuBarGui->getSubGui<RenameModalSubGui>("RenameModalSubGui")->renameScheduleEvent.addListener(renameListener);
    m_mainMenuBarGui->getSubGui<NewNameModalSubGui>("NewNameModalSubGui")
        ->createNewScheduleEvent.addListener(createNewListener);
    m_mainMenuBarGui->getSubGui<DeleteModalSubGui>("DeleteModalSubGui")->deleteScheduleEvent.addListener(deleteListener);

    m_mainMenuBarGui->openScheduleFileEvent.addListener(openListener);
    m_mainMenuBarGui->saveEvent.addListener(saveListener);

    m_autosavePopupGui = programInterface.getGuiByID<AutosavePopupGui>("AutosavePopupGui");
    m_autosavePopupGui->applyAutosaveEvent.addListener(applyAutosaveListener);
    m_autosavePopupGui->deleteAutosaveEvent.addListener(deleteAutosaveListener);

    passFileNamesToGui();
}

bool ScheduleIO::isScheduleFilePath(const fs::path& path) const {
    return strcmp(path.extension().string().c_str(), m_converter.getExtension().c_str()) == 0;
}

bool ScheduleIO::isValidScheduleFile(const std::filesystem::path& path) const {
    return m_converter.isValidScheduleFile(path.string().c_str());
}

fs::path ScheduleIO::makeSchedulePathFromName(const char* name) const {
    return fs::path(m_saveDir) / std::format("{}{}", name, m_converter.getExtension()).c_str();
}

fs::path ScheduleIO::makeIniPathFromScheduleName(const char* name) const {
    return fs::path(m_saveDir) / std::format("{}{}", name, INI_FILE_EXTENSION).c_str();
}

void ScheduleIO::passFileNamesToGui() {
    m_startPageGui->passFileNames(getScheduleStemNamesSortedByEditTime(true));
    m_mainMenuBarGui->passFileNames(getScheduleStemNamesSortedByEditTime(false));
}

void ScheduleIO::sendFileInfoUpdates() {
    openFileInfoChangeEvent.invoke(m_currentFileInfo);
}

void ScheduleIO::unloadCurrentFile() {
    m_schedule.clearSchedule();
    m_schedule.getEditHistoryMutable().clearEditHistory();
    m_currentFileInfo.clear();
    sendFileInfoUpdates();
    fileUnloadEvent.invoke();
}

void ScheduleIO::closeCurrentFile() {
    printf("ScheduleIO::closeCurrentFile(): Closing current file...\n");
    if (m_currentFileInfo.empty()) {
        return;
    }

    createAutosave();
    if (isAutosave(m_currentFileInfo.getName()) == false) {
        applyAutosaveToFile(m_currentFileInfo.getName().c_str());
    }
    createIniForFile(m_currentFileInfo.getName().c_str());

    unloadCurrentFile();
}

bool ScheduleIO::applyAutosaveToFile(const char* fileName) {
    std::cout << std::format("ScheduleIO::applyAutosaveToFile(): Applying autosave to file: '{}'", fileName) << std::endl;
    fs::path pathToFile = fs::path(makeSchedulePathFromName(fileName));
    fs::path pathToAutosaveFile = fs::path(makeSchedulePathFromName(getFileAutosaveName(fileName).c_str()));

    // A Schedule file with this path does not exist. stop.
    if (fs::exists(pathToFile) == false) {
        std::cout << std::format("ScheduleIO::applyAutosaveToFile(): Base file for Schedule '{}' not found at path: '{}'",
                                 fileName,
                                 pathToFile.string())
                  << std::endl;
        return false;
    }
    if (fs::exists(pathToAutosaveFile) == false) {
        std::cout << std::format("ScheduleIO::applyAutosaveToFile(): Autosave file for Schedule '{}' not found at path: '{}'",
                                 fileName,
                                 pathToAutosaveFile.string())
                  << std::endl;
        return false;
    }

    // NOTE: It seems std::filesystem::copy_file cannot actually overwrite existing files (on windows?)
    // But i don't want to create OS-specific things here so i will simply delete the existing file, copy the autosave and then delete the autosave.
    // Probably many more opportunities for errors, but eh.
    // TODO: To minimise the risk of losing data, i could rename the old file instead, only deleting it once everything else is done successfully

    // Delete the base file
    fs::remove(pathToFile);
    try {
        if (fs::copy_file(pathToAutosaveFile, pathToFile, fs::copy_options::update_existing)) {
            // The file was successfully copied, which means the autosave can be removed
            fs::remove(pathToAutosaveFile);
            passFileNamesToGui();
            std::cout << std::format("ScheduleIO::applyAutosaveToFile(): Applied autosave to Schedule '{}'", fileName)
                      << std::endl;
            return true;
        }
    }
    // Failed to copy
    catch (fs::filesystem_error& e)
    {
        std::cout << std::format(
                         "ScheduleIO::applyAutosaveToFile(): Could not copy autosave at '{}' to file at '{}'. Error: {}",
                         pathToAutosaveFile.string(),
                         pathToFile.string(),
                         e.what())
                  << std::endl;
        return false;
    }
    passFileNamesToGui();

    return false;
}

void ScheduleIO::createIniForFile(const char* name) {
    if (ImGui::GetIO().WantSaveIniSettings) {
        if (isAutosave(name)) {
            ImGui::SaveIniSettingsToDisk(makeIniPathFromScheduleName(getFileBaseName(name).c_str()).string().c_str());
        } else {
            ImGui::SaveIniSettingsToDisk(makeIniPathFromScheduleName(name).string().c_str());
        }
        ImGui::GetIO().WantSaveIniSettings = false;
    }
}

bool ScheduleIO::writeSchedule(const char* name) {
    if (fs::exists(m_saveDir) == false) {
        if (fs::create_directories(m_saveDir)) {
            std::cout
                << std::format(
                       "ScheduleIO::writeSchedule(): Schedule save directory did not exist. Created a 'schedules' directory at: '{}'",
                       m_saveDir.string())
                << std::endl;
        } else {
            std::cout
                << std::format(
                       "ScheduleIO::writeSchedule(): Schedule save directory did not exist. FAILED to create a 'schedules' directory at: '{}'",
                       m_saveDir.string())
                << std::endl;
            return false;
        }
    }

    fs::path schedulePath = makeSchedulePathFromName(name);

    if (m_converter.writeSchedule(schedulePath.string().c_str(), m_schedule.getAllColumns()) == 0) {
        createIniForFile(name);
        std::cout << std::format("ScheduleIO::writeSchedule(): Wrote Schedule to file: '{}'", schedulePath.string())
                  << std::endl;
    }
    // TODO: make some event that the Schedule can listen to?
    m_schedule.getEditHistoryMutable().setEditedSinceWrite(false);
    passFileNamesToGui();
    return true;
}

// Reads a schedule from file and applies / opens it
bool ScheduleIO::readSchedule(const char* name) {
    fs::path schedulePath = makeSchedulePathFromName(name);

    if (fs::exists(schedulePath) == false) {
        std::cout << std::format("ScheduleIO::readSchedule(): Tried to read Schedule at path to non-existant file: '{}'",
                                 schedulePath.string())
                  << std::endl;
        return false;
    }

    m_schedule.getEditHistoryMutable().clearEditHistory();
    if (std::optional<FileInfo> readFileInfo =
            m_converter.readSchedule(schedulePath.string().c_str(), m_schedule.getAllColumnsMutable()))
    {
        if (!isAutosave(schedulePath.string())) {
            ImGui::LoadIniSettingsFromDisk(makeIniPathFromScheduleName(name).string().c_str());
        }
        std::cout << std::format("ScheduleIO::readSchedule(): Read Schedule from file: '{}'", schedulePath.string())
                  << std::endl;
        m_schedule.sortColumns();
        m_currentFileInfo.fill(std::string(name), getFileEditTimeWrapped(schedulePath), readFileInfo->getScheduleEditTime());
        sendFileInfoUpdates();
        m_startPageGui->setVisible(false);
        m_scheduleGui->setVisible(true);
        m_schedule.getEditHistoryMutable().setEditedSinceWrite(false);
        fileReadEvent.invoke(m_currentFileInfo);
        return true;
    } else {
        std::cout << std::format("ScheduleIO::readSchedule(): Failed when reading Schedule from file: '{}'",
                                 schedulePath.string())
                  << std::endl;
        return false;
    }
}

bool ScheduleIO::createNewSchedule(const char* name) {
    m_schedule.createDefaultSchedule();

    if (writeSchedule(name))  // passes new list of file names to gui
    {
        fs::path createdFilePath = makeSchedulePathFromName(name);
        m_currentFileInfo.fill(std::string(name), getFileEditTimeWrapped(createdFilePath), TimeWrapper::getCurrentTime());
        sendFileInfoUpdates();
        passFileNamesToGui();
        m_startPageGui->setVisible(false);
        m_scheduleGui->setVisible(true);
        fileCreatedEvent.invoke(m_currentFileInfo);
        return true;
    }
    return false;
}

// Deletes the Schedule with the name and returns true if it exists or returns false.
bool ScheduleIO::deleteSchedule(const char* name) {
    fs::path schedulePath = makeSchedulePathFromName(name);

    if (fs::exists(schedulePath) == false) {
        std::cout << std::format("ScheduleIO::deleteSchedule(): Tried to delete non-existent Schedule at: '{}'",
                                 schedulePath.string())
                  << std::endl;
        return false;
    }

    fs::path autosavePath = makeSchedulePathFromName(getFileAutosaveName(name).c_str());

    // delete the file's autosave if it exists
    if (fs::exists(autosavePath)) {
        fs::remove(autosavePath);
    }

    fs::path iniFilePath = makeIniPathFromScheduleName(name);

    // delete the file's ini file if it exists
    if (fs::exists(iniFilePath)) {
        fs::remove(iniFilePath);
    }

    if (fs::remove(schedulePath)) {
        passFileNamesToGui();
        // deleted the file that was open
        if (m_currentFileInfo.getName() == name) {
            unloadCurrentFile();
            m_scheduleGui->setVisible(false);
            m_startPageGui->setVisible(true);
        }
        return true;
    }

    return false;
}

bool ScheduleIO::renameCurrentFile(const std::string& newName) {
    if (m_currentFileInfo.empty()) {
        return false;
    }

    fs::path pathToOpenFile = fs::path(makeSchedulePathFromName(m_currentFileInfo.getName().c_str()));
    fs::path pathToRenamedFile = fs::path(makeSchedulePathFromName(newName.c_str()));
    bool schedulesDirWasCreated = false;

    // Write-operation: Create schedules directory if it doesn't exist.
    if (fs::exists(m_saveDir) == false) {
        if (fs::create_directories(m_saveDir)) {
            std::cout
                << std::format(
                       "ScheduleIO::renameCurrentFile(): Tried to rename a Schedule but the schedules directory did not exist. Created a schedules directory at: '{}'",
                       m_saveDir.string())
                << std::endl;
            schedulesDirWasCreated = true;
        } else {
            std::cout
                << std::format(
                       "ScheduleIO::renameCurrentFile(): Tried to rename a Schedule but the schedules directory did not exist. FAILED to create a schedules directory at: '{}'",
                       m_saveDir.string())
                << std::endl;
        }
    }
    //  A Schedule file with this name already exists, don't overwrite it. Just stop.
    if (fs::exists(pathToRenamedFile)) {
        return false;
    }
    // If the file to rename doesn't exist, just write the Schedule to the file with the provided new name
    if (schedulesDirWasCreated || fs::exists(pathToOpenFile) == false) {
        std::cout
            << std::format(
                   "ScheduleIO::renameCurrentFile(): Tried to change the name of the Schedule file, but the file was not found at its previous path: '{}'",
                   pathToOpenFile.string())
            << std::endl;
        writeSchedule(newName.c_str());
        std::cout << std::format("ScheduleIO::renameCurrentFile(): Wrote current file to renamed path: '{}'",
                                 pathToRenamedFile.string())
                  << std::endl;
    } else  // All is fine, rename the file
    {
        fs::rename(pathToOpenFile, pathToRenamedFile);
    }
    // Rename the autosave as well, if it exists
    fs::path pathToAutosave =
        fs::path(makeSchedulePathFromName(getFileAutosaveName(pathToOpenFile.stem().string().c_str()).c_str()));
    fs::path pathToRenamedAutosave = fs::path(makeSchedulePathFromName(getFileAutosaveName(newName.c_str()).c_str()));
    if (fs::exists(pathToAutosave)) {
        fs::rename(pathToAutosave, pathToRenamedAutosave);
    }
    // Rename the ini file as well, if it exists
    fs::path pathToIniFile = fs::path(makeIniPathFromScheduleName(pathToOpenFile.stem().string().c_str()));
    fs::path pathToRenamedIniFile = fs::path(makeIniPathFromScheduleName(newName.c_str()));
    if (fs::exists(pathToIniFile)) {
        fs::rename(pathToIniFile, pathToRenamedIniFile);
    }

    passFileNamesToGui();

    m_currentFileInfo.rename(newName);
    sendFileInfoUpdates();

    return true;
}

FileInfo ScheduleIO::getCurrentFileInfo() const {
    return m_currentFileInfo;
}

void ScheduleIO::openMostRecentFile() {
    // LAMBDA
    auto goToStartPage = [&]() {
        m_scheduleGui->setVisible(false);
        m_startPageGui->setVisible(true);
    };
    // There are pre-existing Schedules. Open the most recently edited one.
    if (getScheduleStemNames().size() > 0) {
        std::string lastEditedScheduleName = getLastEditedScheduleStemName();

        // the most recently edited file is an autosave, the program might not have been closed correctly.
        // show autosave prompt popup
        if (isAutosave(lastEditedScheduleName)) {
            std::string fileBaseName = getFileBaseName(lastEditedScheduleName.c_str());
            fs::path fileBasePath = makeSchedulePathFromName(fileBaseName.c_str());

            // The autosave has a base file, ask which to open
            if (std::filesystem::exists(makeSchedulePathFromName(fileBaseName.c_str()))) {
                fs::path autosavePath = makeSchedulePathFromName(lastEditedScheduleName.c_str());

                // TODO: Make this nicer. Note that we can't pass the scheduleEditTime since the files aren't actually being loaded until the user chooses to.
                m_autosavePopupGui->open(FileInfo(fileBaseName, getFileEditTimeWrapped(fileBasePath), TimeWrapper()),
                                         FileInfo(lastEditedScheduleName, getFileEditTimeWrapped(autosavePath), TimeWrapper()),
                                         getFileEditTimeString(fileBasePath),
                                         getFileEditTimeString(fs::path(autosavePath)));
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
            if (!readSchedule(getLastEditedScheduleStemName().c_str())) {
                // Go to start page if there was a failure to read the file
                goToStartPage();
            }
        }
    }
    // There are no Schedule files. Open the Start Page so the user can create one from there or File->New.
    else
    {
        goToStartPage();
    }
}

bool ScheduleIO::createAutosave() {
    if (m_currentFileInfo.empty()) {
        return false;
    }
    if (m_schedule.getEditHistory().getEditedSinceWrite() == false) {
        return false;
    }
    printf("ScheduleIO::createAutosave(): Creating autosave...\n");

    // save to open file name if the open file is itself an autosave, otherwise get the autosave name from the base file name
    std::string autosaveName = isAutosave(m_currentFileInfo.getName())
        ? m_currentFileInfo.getName()
        : getFileAutosaveName(m_currentFileInfo.getName().c_str());

    if (writeSchedule(autosaveName.c_str())) {
        return true;
        printf("ScheduleIO::createAutosave(): Created autosave!\n");
    }

    return false;
}

bool ScheduleIO::isAutosave(const std::string& name) {
    return name.rfind(m_autosaveSuffix) != std::string::npos;
}

std::string ScheduleIO::getFileAutosaveName(const char* name) {
    return std::string(name).append(m_autosaveSuffix);
}

std::string ScheduleIO::getFileBaseName(const char* autosaveName) {
    std::string autosaveString = std::string(autosaveName);
    if (isAutosave(autosaveString) == false) {
        std::cout << std::format("ScheduleIO::getFileBaseName(): File name '{}' is not an autosave", autosaveName) << std::endl;
        return autosaveString;
    }

    return autosaveString.substr(0, autosaveString.rfind(m_autosaveSuffix));
}

long long ScheduleIO::getFileEditTime(fs::path path) {
    if (fs::exists(path) == false) {
        std::cout << std::format("ScheduleIO::getFileEditTime(): No file exists at path: '{}'", path.string()) << std::endl;
        return 0;
    }

    return fs::last_write_time(path).time_since_epoch().count();
}

TimeWrapper ScheduleIO::getFileEditTimeWrapped(fs::path path) {
    if (fs::exists(path) == false) {
        std::cout << std::format("ScheduleIO::getFileEditTimeWrapped(): No file exists at path: '{}'", path.string())
                  << std::endl;
        return TimeWrapper();
    }

    const auto fileEditTime = fs::last_write_time(path);
    std::chrono::system_clock::time_point systemTime;

#if defined(WIN32) && \
    !defined(__clang__)  // windows implementation using clock_cast (my version of clang / libc++ didn't have it either)
    systemTime = std::chrono::clock_cast<std::chrono::system_clock>(fileEditTime);
#else  // workaround without clock_cast
    auto systemNow = std::chrono::system_clock::now();
    auto fileNow = std::chrono::file_clock::now();
    systemTime = std::chrono::time_point_cast<std::chrono::system_clock::duration>(fileEditTime - fileNow + systemNow);
#endif
    return TimeWrapper(systemTime);
}

std::string ScheduleIO::getFileEditTimeString(fs::path path) {
    if (fs::exists(path) == false) {
        std::cout << std::format("ScheduleIO::getFileEditTimeString(): No file exists at path: '{}'", path.string())
                  << std::endl;
        return std::string("");
    }

    return getFileEditTimeWrapped(path).getString(TIME_FORMAT_FULL);
}

std::vector<std::string> ScheduleIO::getScheduleStemNames(bool includeAutosaves) {
    std::vector<std::string> filenames = {};
    if (fs::exists(m_saveDir) == false) {
        std::cout << std::format("ScheduleIO::getScheduleStemNames(): Schedules directory '{}' does not exist.",
                                 m_saveDir.string())
                  << std::endl;
        return filenames;
    }

    for (const auto& entry : fs::directory_iterator(m_saveDir)) {
        // Skip non-regular files
        if (entry.is_regular_file() == false) {
            std::cout << "ScheduleIO::getScheduleStemNames(): Skipped file " << entry.path().string()
                      << " (entry.is_regular_file() == false)" << std::endl;
            continue;
        }
        // Skip non-schedule files
        if (isScheduleFilePath(entry.path()) == false) {
            continue;
        }
        // Skip invalid (probably outdated) schedule files
        if (isValidScheduleFile(entry.path()) == false) {
            std::cout << "ScheduleIO::getScheduleStemNames(): Skipped file " << entry.path().string()
                      << " (not a valid schedule file)" << std::endl;
            continue;
        }
        // Skip autosave files if includeAutosaves == false
        if (includeAutosaves == false && isAutosave(entry.path().stem().string())) {
            continue;
        }

        filenames.push_back(entry.path().stem().string());
    }
    return filenames;
}

std::vector<std::string> ScheduleIO::getScheduleStemNamesSortedByEditTime(bool includeAutosaves) {
    std::vector<std::string> filenames = {};
    std::map<std::string, long long> filenamesEditTimes = {};

    if (fs::exists(m_saveDir) == false) {
        std::cout << std::format("ScheduleIO::getScheduleStemNamesSortedByEditTime(): Schedules directory '{}' does not exist.",
                                 m_saveDir.string())
                  << std::endl;
        return filenames;
    }

    for (const auto& entry : fs::directory_iterator(m_saveDir)) {
        // Skip non-regular files
        if (entry.is_regular_file() == false) {
            std::cout << "ScheduleIO::getScheduleStemNamesSortedByEditTime(): Skipped file " << entry.path().string()
                      << " (entry.is_regular_file() == false)" << std::endl;
            continue;
        }
        // Skip non-schedule files
        if (isScheduleFilePath(entry.path()) == false) {
            continue;
        }
        // Skip invalid (probably outdated) schedule files
        if (isValidScheduleFile(entry.path()) == false) {
            std::cout << "ScheduleIO::getScheduleStemNamesSortedByEditTime(): Skipped file " << entry.path().string()
                      << " (not a valid schedule file)" << std::endl;
            continue;
        }
        // Skip autosave files if includeAutosaves == false
        if (includeAutosaves == false && isAutosave(entry.path().stem().string())) {
            continue;
        }

        filenames.push_back(entry.path().stem().string());
        filenamesEditTimes.insert({entry.path().stem().string(), getFileEditTime(entry)});
    }

    std::sort(filenames.begin(), filenames.end(), [&](std::string fs1, std::string fs2) {
        return filenamesEditTimes.at(fs1) > filenamesEditTimes.at(fs2);
    });
    return filenames;
}

std::string ScheduleIO::getLastEditedScheduleStemName() {
    if (fs::exists(m_saveDir) == false) {
        std::cout << std::format("ScheduleIO::getLastEditedScheduleStemName(): Schedules directory '{}' does not exist.",
                                 m_saveDir.string())
                  << std::endl;
        return std::string("");
    }

    long long latestEditTime = std::numeric_limits<long long>::min();
    fs::path latestEditedPath;
    fs::directory_iterator dirIterator = fs::directory_iterator(m_saveDir);

    for (const auto& entry : dirIterator) {
        // Skip non-regular files
        if (entry.is_regular_file() == false) {
            std::cout << "ScheduleIO::getLastEditedScheduleStemName(): Skipped file " << entry.path().string()
                      << " (entry.is_regular_file() == false)" << std::endl;
            continue;
        }
        // Skip non-schedule files
        if (isScheduleFilePath(entry.path()) == false) {
            continue;
        }
        // Skip invalid (probably outdated) schedule files
        if (isValidScheduleFile(entry.path()) == false) {
            std::cout << "ScheduleIO::getLastEditedScheduleStemName(): Skipped file " << entry.path().string()
                      << " (not a valid schedule file)" << std::endl;
            continue;
        }

        fs::file_time_type editTime = fs::last_write_time(entry);

        if (latestEditTime == std::numeric_limits<long long>::min() || editTime.time_since_epoch().count() > latestEditTime) {
            latestEditedPath = entry;
            latestEditTime = editTime.time_since_epoch().count();
        }
    }

    return latestEditedPath.stem().string();
}