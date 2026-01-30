#include <algorithm>
#include <optional>
#include <chrono>
#include <format>
#include <stdexcept>
#include "schedule_io.h"
#include "messages/status_message_queue.h"

namespace fs = std::filesystem;

ScheduleIO::ScheduleIO(Schedule& schedule, Interface& programInterface, std::filesystem::path saveDir)
    : m_schedule(schedule), m_saveDir(saveDir), m_converter() {
    m_startPageGui = programInterface.getGuiByID<StartPageGui>("StartPageGui");
    m_startPageGui->createNewScheduleEventPipe.addListener(createNewListener);
    m_startPageGui->openScheduleFileEvent.addListener(openListener);

    m_mainMenuBarGui = programInterface.getGuiByID<MainMenuBarGui>("MainMenuBarGui");
    m_mainMenuBarGui->renameScheduleEventPipe.addListener(renameListener);
    m_mainMenuBarGui->createNewScheduleEventPipe.addListener(createNewListener);
    m_mainMenuBarGui->deleteScheduleEventPipe.addListener(deleteListener);

    m_mainMenuBarGui->openScheduleFileEvent.addListener(openListener);
    m_mainMenuBarGui->saveEvent.addListener(saveListener);
    m_mainMenuBarGui->saveAndCloseEventPipe.addListener(saveAndCloseListener);
    m_mainMenuBarGui->closeWithoutSaveEventPipe.addListener(closeWithoutSaveListener);

    m_autosavePopupGui = programInterface.getGuiByID<AutosavePopupGui>("AutosavePopupGui");
    m_autosavePopupGui->applyAutosaveEvent.addListener(applyAutosaveListener);
    m_autosavePopupGui->deleteAutosaveEvent.addListener(deleteAutosaveListener);

    m_schedule.getScheduleEvents().editAdded.addListener(editListener);
    m_schedule.getScheduleEvents().editRedone.addListener(editListener);
    m_schedule.getScheduleEvents().editUndone.addListener(editListener);

    onBaseFileListChange();
}

bool ScheduleIO::isValidScheduleFile(const std::filesystem::path& path) const {
    return m_converter.isValidScheduleFile(path.string().c_str());
}

bool ScheduleIO::isAutosave(const FileInfo& fileInfo) {
    return fileInfo.getStem().rfind(m_autosaveSuffix) != std::string::npos;
}

fs::path ScheduleIO::nameToSchedulePath(const char* name) const {
    return fs::path(m_saveDir) / std::format("{}{}", name, m_converter.getExtension()).c_str();
}

fs::path ScheduleIO::nameToIniPath(const char* name) const {
    return fs::path(m_saveDir) / std::format("{}{}", name, INI_FILE_EXTENSION).c_str();
}

std::filesystem::path ScheduleIO::getFileAutosavePath(const FileInfo& fileInfo) {
    std::string autosaveName = fileInfo.getStem().append(m_autosaveSuffix);
    auto autosaveInfo = FileInfo(fileInfo);
    autosaveInfo.rename(autosaveName);
    return autosaveInfo.getPath();
}

std::string ScheduleIO::getFileBaseName(const FileInfo& autosaveInfo) {
    if (isAutosave(autosaveInfo) == false) {
        std::cout << std::format("ScheduleIO::getFileBaseName(): File at '{}' is not an autosave",
                                 autosaveInfo.getPath().string())
                  << std::endl;
        return autosaveInfo.getStem();
    }

    return autosaveInfo.getStem().substr(0, autosaveInfo.getStem().rfind(m_autosaveSuffix));
}

std::filesystem::path ScheduleIO::getFileBasePath(const FileInfo& autosaveInfo) {
    std::string fileBaseName = getFileBaseName(autosaveInfo);
    auto fileBaseInfo = FileInfo(autosaveInfo);
    fileBaseInfo.rename(fileBaseName);
    return fileBaseInfo.getPath();
}

void ScheduleIO::onCurrentFileSaveSuccess() {
    StatusMessageQueue::push(StatusMessage::info("Saved!"));
}

void ScheduleIO::onCurrentFileInfoChange() {
    openFileInfoChangeEvent.invoke(m_currentFileInfo);
}

void ScheduleIO::onBaseFileListChange() {
    auto baseFileInfoList = getScheduleFileInfoList(ScheduleFileFilter::Base, ScheduleFileSort::EditTime_Descending);
    m_startPageGui->passFileInfoList(baseFileInfoList);
    m_mainMenuBarGui->passFileInfoList(baseFileInfoList);
}

void ScheduleIO::goToStartPage() {
    m_schedule.hideAllViews();
    m_startPageGui->setVisible(true);
}

std::optional<FileInfo> ScheduleIO::writeSchedule(const fs::path& path, Schedule& schedule) {
    if (path.parent_path() != m_saveDir) {
        std::cout << std::format("ScheduleIO::writeSchedule(): Target path '{}' is not in save dir: '{}'",
                                 path.string(),
                                 m_saveDir.string())
                  << std::endl;
        return std::nullopt;
    }
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
            return std::nullopt;
        }
    }

    if (auto writtenFileInfo = m_converter.writeSchedule(path, schedule.getAllColumns(), schedule.getPreferences())) {
        createIniForFile(writtenFileInfo.value());
        std::cout << std::format("ScheduleIO::writeSchedule(): Wrote Schedule to file: '{}'",
                                 writtenFileInfo->getPath().string())
                  << std::endl;
        schedule.getEditHistoryMutable().setEditedSinceWrite(false);
        onBaseFileListChange();
        return writtenFileInfo;
    }

    return std::nullopt;
}

bool ScheduleIO::applyAutosaveToFile(const FileInfo& fileInfo) {
    std::cout << std::format("ScheduleIO::applyAutosaveToFile(): Applying autosave to file: '{}'", fileInfo.getPath().string())
              << std::endl;
    fs::path pathToAutosaveFile = getFileAutosavePath(fileInfo);

    // A Schedule file with this path does not exist. stop.
    if (fs::exists(fileInfo.getPath()) == false) {
        std::cout << std::format("ScheduleIO::applyAutosaveToFile(): Base file for Schedule '{}' not found at path: '{}'",
                                 fileInfo.getStem(),
                                 fileInfo.getPath().string())
                  << std::endl;
        return false;
    }
    if (fs::exists(pathToAutosaveFile) == false) {
        std::cout << std::format("ScheduleIO::applyAutosaveToFile(): Autosave file for Schedule '{}' not found at path: '{}'",
                                 fileInfo.getStem(),
                                 pathToAutosaveFile.string())
                  << std::endl;
        return false;
    }

    // NOTE: It seems std::filesystem::copy_file cannot actually overwrite existing files (on windows?)
    // But i don't want to create OS-specific things here so i will simply delete the existing file, copy the autosave and then delete the autosave.
    // Probably many more opportunities for errors, but eh.
    // TODO: To minimise the risk of losing data, i could rename the old file instead, only deleting it once everything else is done successfully

    // Delete the base file
    fs::remove(fileInfo.getPath());
    try {
        if (fs::copy_file(pathToAutosaveFile, fileInfo.getPath(), fs::copy_options::update_existing)) {
            // The file was successfully copied, which means the autosave can be removed
            fs::remove(pathToAutosaveFile);
            // Applied autosave to current file
            if (isThereFileOpen() && m_currentFileInfo.value() == fileInfo) {
                onCurrentFileSaveSuccess();
            }
            onBaseFileListChange();
            std::cout << std::format("ScheduleIO::applyAutosaveToFile(): Applied autosave to Schedule '{}'", fileInfo.getStem())
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
                         fileInfo.getPath().string(),
                         e.what())
                  << std::endl;
    }
    onBaseFileListChange();

    return false;
}

bool ScheduleIO::deleteAutosaveFor(const FileInfo& fileInfo) {
    fs::path pathToAutosaveFile = getFileAutosavePath(fileInfo);

    // A Schedule file with this path does not exist. stop.
    if (fs::exists(pathToAutosaveFile) == false) {
        return false;
    }

    return fs::remove(pathToAutosaveFile);
}

void ScheduleIO::createIniForFile(const FileInfo& fileInfo) {
    // TODO: This function is ugly with the .c_str() spam
    // I think it would really be better to just have separate dirs with the same stem but different extension
    if (ImGui::GetIO().WantSaveIniSettings) {
        ImGui::SaveIniSettingsToDisk(nameToIniPath(getFileBaseName(fileInfo).c_str()).string().c_str());
        ImGui::GetIO().WantSaveIniSettings = false;
    }
}

void ScheduleIO::unloadCurrentFile() {
    m_schedule.clearSchedule();
    m_schedule.clearDateOverride();
    m_schedule.getEditHistoryMutable().clearEditHistory();
    m_currentFileInfo.reset();
    onCurrentFileInfoChange();
    fileUnloadEvent.invoke();
}

std::optional<FileInfo> ScheduleIO::getCurrentFileInfo() const {
    return m_currentFileInfo;
}

bool ScheduleIO::isThereFileOpen() const {
    return m_currentFileInfo.has_value();
}

// Reads a schedule from file and applies / opens it
bool ScheduleIO::readSchedule(const FileInfo& fileInfo) {
    if (fs::exists(fileInfo.getPath()) == false) {
        std::cout << std::format("ScheduleIO::readSchedule(): Tried to read Schedule at path to non-existant file: '{}'",
                                 fileInfo.getPath().string())
                  << std::endl;
        return false;
    }

    // Empty schedule & file preferences to fill with the read data
    std::vector<Column> readSchedule = {};
    SchedulePreferences readSchedulePreferences;
    if (std::optional<FileInfo> readFileInfo = m_converter.readSchedule(fileInfo, readSchedule, readSchedulePreferences)) {
        m_schedule.replaceSchedule(readSchedule);
        m_schedule.updatePreferences(readSchedulePreferences);
        if (!isAutosave(fileInfo)) {
            ImGui::LoadIniSettingsFromDisk(nameToIniPath(getFileBaseName(fileInfo).c_str()).string().c_str());
        }
        std::cout << std::format("ScheduleIO::readSchedule(): Read Schedule from file: '{}'", fileInfo.getPath().string())
                  << std::endl;
        m_schedule.sortColumns();
        m_currentFileInfo =
            FileInfo(fileInfo.getPath(), getFileEditTime(fileInfo.getPath()), readFileInfo->getScheduleEditTime());
        onCurrentFileInfoChange();
        m_startPageGui->setVisible(false);
        m_schedule.getEditHistoryMutable().setEditedSinceWrite(false);
        fileHasEditsStateChanged();
        fileReadEvent.invoke(m_currentFileInfo.value());
        return true;
    } else {
        std::cout << std::format("ScheduleIO::readSchedule(): Failed when reading Schedule from file: '{}'",
                                 fileInfo.getPath().string())
                  << std::endl;
        return false;
    }
}

bool ScheduleIO::createNewSchedule(const char* name) {
    m_schedule.createDefaultSchedule();

    if (auto createdFileInfo = writeSchedule(nameToSchedulePath(name), m_schedule))  // passes new list of file names to gui
    {
        m_currentFileInfo = createdFileInfo;
        onCurrentFileInfoChange();
        onBaseFileListChange();
        onCurrentFileSaveSuccess();
        m_startPageGui->setVisible(false);
        // Update with default preferences for a new file
        m_schedule.updatePreferences(SchedulePreferences());
        fileCreatedEvent.invoke(m_currentFileInfo.value());
        return true;
    }
    return false;
}

// Deletes the Schedule with the name and returns true if it exists or returns false.
bool ScheduleIO::deleteSchedule(const FileInfo& fileInfo) {
    if (fs::exists(fileInfo.getPath()) == false) {
        std::cout << std::format("ScheduleIO::deleteSchedule(): Tried to delete non-existent Schedule at: '{}'",
                                 fileInfo.getPath().string())
                  << std::endl;
        return false;
    }

    fs::path autosavePath = getFileAutosavePath(fileInfo);

    // Delete the file's autosave if it exists
    if (fs::exists(autosavePath)) {
        fs::remove(autosavePath);
    }

    fs::path iniFilePath = nameToIniPath(getFileBaseName(fileInfo).c_str());

    // Delete the file's ini file if it exists
    if (fs::exists(iniFilePath)) {
        fs::remove(iniFilePath);
    }

    if (fs::remove(fileInfo.getPath())) {
        onBaseFileListChange();
        // Deleted the file that was open
        if (m_currentFileInfo.has_value() && m_currentFileInfo->getPath() == fileInfo.getPath()) {
            unloadCurrentFile();
            goToStartPage();
        }
        return true;
    }

    return false;
}

bool ScheduleIO::saveCurrentFile() {
    if (!isThereFileOpen()) {
        return false;
    }

    if (auto updatedFileInfo = writeSchedule(m_currentFileInfo->getPath(), m_schedule)) {
        m_currentFileInfo = updatedFileInfo;
        onBaseFileListChange();
        fileHasEditsStateChanged();
        onCurrentFileSaveSuccess();
        return true;
    }
    StatusMessageQueue::push(StatusMessage::error(std::format("Failed to save file!")));
    return false;
}

bool ScheduleIO::createCurrentFileAutosave() {
    if (!isThereFileOpen()) {
        return false;
    }
    if (m_schedule.getEditHistory().getEditedSinceWrite() == false) {
        return false;
    }
    printf("ScheduleIO::createAutosave(): Creating autosave...\n");

    // Save to open file name if the open file is itself an autosave
    if (isAutosave(m_currentFileInfo.value())) {
        return saveCurrentFile();
    }
    // Get autosave name / path and save to it
    else
    {
        std::filesystem::path autosavePath = getFileAutosavePath(m_currentFileInfo.value());
        if (writeSchedule(autosavePath, m_schedule)) {
            StatusMessageQueue::push(StatusMessage::info("Autosave created!"));
            return true;
        }

        StatusMessageQueue::push(StatusMessage::warning("Failed to create autosave!"));
        return false;
    }
}

bool ScheduleIO::renameCurrentFile(const std::string& newName) {
    if (!isThereFileOpen()) {
        return false;
    }

    FileInfo renamedFileInfo = m_currentFileInfo->getRenamed(newName);

    // Write-operation: Create schedules directory if it doesn't exist.
    if (fs::exists(m_saveDir) == false) {
        if (fs::create_directories(m_saveDir)) {
            std::cout << std::format("ScheduleIO::renameCurrentFile(): Had to create missing schedules directory at: '{}'",
                                     m_saveDir.string())
                      << std::endl;
        } else {
            std::cout << std::format("ScheduleIO::renameCurrentFile(): Failed to create missing schedules directory at: '{}'",
                                     m_saveDir.string())
                      << std::endl;
            return false;
        }
    }

    //  A Schedule file with the new name already exists, don't overwrite it. Just stop.
    if (fs::exists(renamedFileInfo.getPath())) {
        return false;
    }

    // If the file to rename doesn't exist, just write the Schedule to the file with the provided new name
    if (fs::exists(m_currentFileInfo->getPath()) == false) {
        std::cout
            << std::format(
                   "ScheduleIO::renameCurrentFile(): Tried to change the name of the Schedule file, but the file was not found at its previous path: '{}'",
                   m_currentFileInfo->getPath().string())
            << std::endl;

        if (writeSchedule(renamedFileInfo.getPath(), m_schedule)) {
            std::cout << std::format("ScheduleIO::renameCurrentFile(): Wrote current file to renamed path: '{}'",
                                     renamedFileInfo.getPath().string())
                      << std::endl;
        } else {
            return false;
        }
    } else  // All is fine, rename the file
    {
        fs::rename(m_currentFileInfo->getPath(), renamedFileInfo.getPath());
    }
    // Rename the autosave as well, if it exists
    fs::path pathToAutosave = getFileAutosavePath(m_currentFileInfo.value());
    if (fs::exists(pathToAutosave)) {
        fs::path pathToRenamedAutosave = getFileAutosavePath(renamedFileInfo);
        fs::rename(pathToAutosave, pathToRenamedAutosave);
    }
    // Rename the ini file as well, if it exists
    fs::path pathToIniFile = fs::path(nameToIniPath(m_currentFileInfo->getStem().c_str()));
    if (fs::exists(pathToIniFile)) {
        fs::path pathToRenamedIniFile = fs::path(nameToIniPath(renamedFileInfo.getStem().c_str()));
        fs::rename(pathToIniFile, pathToRenamedIniFile);
    }

    onBaseFileListChange();
    m_currentFileInfo = renamedFileInfo;
    onCurrentFileInfoChange();
    StatusMessageQueue::push(StatusMessage::info(std::format("Renamed file to '{}'", renamedFileInfo.getStem())));
    onCurrentFileSaveSuccess();

    return true;
}

void ScheduleIO::closeCurrentFile() {
    printf("ScheduleIO::closeCurrentFile(): Closing current file...\n");
    if (!isThereFileOpen()) {
        return;
    }

    createCurrentFileAutosave();
    if (!isAutosave(m_currentFileInfo.value())) {
        applyAutosaveToFile(m_currentFileInfo.value());
    }
    createIniForFile(m_currentFileInfo.value());

    unloadCurrentFile();
}

void ScheduleIO::openMostRecentFile() {
    std::optional<FileInfo> lastEditedFileInfo = getLastEditedFileInfo();

    // There are no Schedule files. Open the Start Page so the user can create one from there or File->New.
    if (!lastEditedFileInfo.has_value()) {
        goToStartPage();
        return;
    }

    // The most recently edited file is an autosave, the program might not have been closed correctly.
    // Show autosave prompt popup
    if (isAutosave(lastEditedFileInfo.value())) {
        fs::path fileBasePath = getFileBasePath(lastEditedFileInfo.value());

        // The autosave has a base file, ask which to open
        if (std::filesystem::exists(fileBasePath)) {
            fs::path autosavePath = lastEditedFileInfo->getPath();

            // Note that we can't pass a scheduleEditTime to the FileInfo-s since the files aren't actually being loaded until the user chooses to.
            m_autosavePopupGui->open(FileInfo(fileBasePath, getFileEditTime(fileBasePath)),
                                     FileInfo(autosavePath, getFileEditTime(autosavePath)));
        }
        // Somehow there is only an autosave and no base file.
        // Don't know what to do, open the start page and let the user decide?
        else
        {
            goToStartPage();
        }
    }
    // The most recent file is a normal file, read it
    else
    {
        if (!readSchedule(lastEditedFileInfo.value())) {
            // Go to start page if there was a failure to read the file
            goToStartPage();
        }
    }
}

TimeWrapper ScheduleIO::getFileEditTime(fs::path path) const {
    if (fs::exists(path) == false) {
        throw std::runtime_error(
            std::format("ScheduleIO::getFileEditTimeWrapped(): No file exists at path: '{}'", path.string()));
    }

    const auto fileEditTime = fs::last_write_time(path);
    std::chrono::system_clock::time_point systemTime;

    systemTime = std::chrono::clock_cast<std::chrono::system_clock>(fileEditTime);

    return TimeWrapper(systemTime);
}

TimeWrapper ScheduleIO::getFileEditTime(const fs::directory_entry& file) const {
    const auto fileEditTime = file.last_write_time();
    std::chrono::system_clock::time_point systemTime;

    systemTime = std::chrono::clock_cast<std::chrono::system_clock>(fileEditTime);

    return TimeWrapper(systemTime);
}

std::vector<FileInfo> ScheduleIO::getScheduleFileInfoList(ScheduleFileFilter filter, ScheduleFileSort sort) const {
    std::vector<fs::directory_entry> files = {};

    if (fs::exists(m_saveDir) == false) {
        std::cout << std::format("ScheduleIO::getScheduleStemNames(): Schedules directory '{}' does not exist.",
                                 m_saveDir.string())
                  << std::endl;
        return {};
    }

    for (const auto& entry : fs::directory_iterator(m_saveDir)) {
        // Skip non-regular files
        if (entry.is_regular_file() == false) {
            std::cout << "ScheduleIO::getScheduleStemNames(): Skipped file " << entry.path().string()
                      << " (entry.is_regular_file() == false)" << std::endl;
            continue;
        }
        // Skip invalid (probably outdated) schedule files
        if (isValidScheduleFile(entry.path()) == false) {
            std::cout << "ScheduleIO::getScheduleStemNames(): Skipped file " << entry.path().string()
                      << " (not a valid schedule file)" << std::endl;
            continue;
        }
        // Add file info to list if it passes the filter
        if (getScheduleFileFilter(filter)(entry)) {
            files.push_back(entry);
        }
    }
    // Sort if needed
    if (sort != ScheduleFileSort::None) {
        std::sort(files.begin(), files.end(), getScheduleFileSort(sort));
    }

    std::vector<FileInfo> fileInfoList = {};
    for (const fs::directory_entry& entry : files) {
        TimeWrapper fileEditTime = getFileEditTime(entry);
        fileInfoList.push_back(FileInfo(entry.path(), fileEditTime, std::nullopt));
    }

    return fileInfoList;
}

std::optional<FileInfo> ScheduleIO::getLastEditedFileInfo() const {
    auto fileInfoList = getScheduleFileInfoList(ScheduleFileFilter::All, ScheduleFileSort::EditTime_Descending);
    return fileInfoList.empty() ? std::nullopt : std::optional<FileInfo>(fileInfoList.front());
}

const filter_func& ScheduleIO::getScheduleFileFilter(ScheduleFileFilter filter) const {
    return m_scheduleFileFilterFunctions.at(filter);
}

const sort_func& ScheduleIO::getScheduleFileSort(ScheduleFileSort sort) const {
    return m_scheduleFileSortFunctions.at(sort);
}