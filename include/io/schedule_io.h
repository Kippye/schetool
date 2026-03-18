#pragma once

#include <functional>
#include <filesystem>
#include <string>
#include <map>
#include "event.h"
#include "schedule_data_converter.h"
#include "schedule.h"
#include "file_info.h"
#include "start_page_gui.h"
#include "main_menu_bar/main_menu_bar_gui.h"
#include "autosave_popup_gui.h"
#include "interface.h"

enum class ScheduleFileFilter {
    All,
    Base,
    Autosave,
};

enum class ScheduleFileSort {
    None,
    Name_Ascending,
    Name_Descending,
    EditTime_Ascending,
    EditTime_Descending,
};

using directory_entry = std::filesystem::directory_entry;
using filter_func = std::function<bool(const directory_entry&)>;
using sort_func = std::function<bool(const directory_entry&, const directory_entry&)>;

class ScheduleIO {
    private:
        const std::map<ScheduleFileFilter, filter_func> m_scheduleFileFilterFunctions = {
            {ScheduleFileFilter::All, [](const directory_entry&) -> bool { return true; }},
            {ScheduleFileFilter::Base,
             [&](const directory_entry& entry) -> bool { return !isAutosave(FileInfo(entry.path())); }},
            {ScheduleFileFilter::Autosave,
             [&](const directory_entry& entry) -> bool { return isAutosave(FileInfo(entry.path())); }},
        };
        const std::map<ScheduleFileSort, sort_func> m_scheduleFileSortFunctions = {
            // This function shouldn't be used since calling sort can be avoided entirely, but this is here just in case.
            {ScheduleFileSort::None, [](const directory_entry& a, const directory_entry& b) -> bool { return false; }},
            {ScheduleFileSort::Name_Ascending,
             [](const directory_entry& a, const directory_entry& b) -> bool {
                 return a.path().stem().string().compare(b.path().stem().string()) < 0;
             }},
            {ScheduleFileSort::Name_Descending,
             [](const directory_entry& a, const directory_entry& b) -> bool {
                 return a.path().stem().string().compare(b.path().stem().string()) > 0;
             }},
            {ScheduleFileSort::EditTime_Ascending,
             [](const directory_entry& a, const directory_entry& b) -> bool {
                 return a.last_write_time() < b.last_write_time();
             }},
            {ScheduleFileSort::EditTime_Descending,
             [](const directory_entry& a, const directory_entry& b) -> bool {
                 return a.last_write_time() > b.last_write_time();
             }},
        };
        Schedule& m_schedule;
        std::filesystem::path m_saveDir;  // Set in constructor
        ScheduleDataConverter m_converter;
        std::shared_ptr<StartPageGui> m_startPageGui = nullptr;
        std::shared_ptr<MainMenuBarGui> m_mainMenuBarGui = nullptr;
        std::shared_ptr<AutosavePopupGui> m_autosavePopupGui = nullptr;
        std::optional<FileInfo> m_currentFileInfo = std::nullopt;
        const char* m_autosaveSuffix = "_auto";

        std::function<void()> saveListener = [&]() { saveCurrentFile(); };
        // gui listeners
        // ScheduleNameModalSubGui
        std::function<void(std::string)> renameListener = [&](std::string name) {
            if (renameCurrentFile(name)) {
                m_mainMenuBarGui->closeModal();
            }
        };
        std::function<void(std::string)> createNewListener = [&](std::string name) {
            closeCurrentFile();
            if (createNewSchedule(name.c_str())) {
                m_mainMenuBarGui->closeModal();
            }
        };
        // DeleteModalSubGui
        std::function<void(std::string)> deleteListener = [this](std::string name) {
            deleteSchedule(FileInfo(nameToSchedulePath(name.c_str())));
            // Modal hides itself
        };
        // MainMenuBarGui
        std::function<void(FileInfo)> openListener = [&](FileInfo fileInfo) {
            closeCurrentFile();
            readSchedule(fileInfo);
        };
        std::function<void(std::string)> saveAndCloseListener = [&](std::string) {
            closeCurrentFile();
            goToStartPage();
        };
        std::function<void(std::string)> closeWithoutSaveListener = [&](std::string) {
            if (!isThereFileOpen()) {
                return;
            }
            deleteAutosaveFor(m_currentFileInfo.value());
            unloadCurrentFile();
            goToStartPage();
        };
        std::function<void(std::shared_ptr<const ScheduleEdit>)> editListener = [&](std::shared_ptr<const ScheduleEdit>) {
            fileHasEditsStateChanged();
        };
        // TO MainMenuBarGui
        std::function<void()> fileHasEditsStateChanged = [&]() {
            if (m_mainMenuBarGui) {
                m_mainMenuBarGui->passFileHasEdits(m_schedule.getEditHistory().getEditedSinceWrite());
            }
        };
        // AutosavePopupGui
        // NOTE: all of these assume that the most recently edited file is still an autosave
        // TODO: handle the (rare?) case where it isn't
        std::function<void()> applyAutosaveListener = [&]() {
            std::optional<FileInfo> lastEditedFileInfo = getLastEditedFileInfo();
            if (!lastEditedFileInfo.has_value()) {
                return;
            }
            // 99% of the time, the autosave will be the most recent
            if (isAutosave(lastEditedFileInfo.value())) {
                FileInfo baseFileInfo = FileInfo(getFileBasePath(lastEditedFileInfo.value()));
                // Apply autosave to file
                applyAutosaveToFile(baseFileInfo);
                // Read the updated base file
                readSchedule(baseFileInfo);
            }
            // Somehow, the most recently edited file was an autosave but now isn't?
            else
            {
                // Let's give up! :D
                goToStartPage();
            }
        };
        std::function<void()> deleteAutosaveListener = [&]() {
            std::optional<FileInfo> lastEditedFileInfo = getLastEditedFileInfo();
            if (!lastEditedFileInfo.has_value()) {
                return;
            }

            FileInfo baseFileInfo = FileInfo(getFileBasePath(lastEditedFileInfo.value()));
            deleteAutosaveFor(baseFileInfo);
            readSchedule(baseFileInfo);
        };

        // Returns true if the path points to a file that can be loaded by the ScheduleDataConverter.
        bool isValidScheduleFile(const std::filesystem::path& path) const;
        bool isAutosave(const FileInfo& fileInfo);

        std::filesystem::path nameToSchedulePath(const char* name) const;
        std::filesystem::path nameToIniPath(const char* name) const;
        std::filesystem::path getFileAutosavePath(const FileInfo& fileInfo);
        // Get the name (stem) of the file or its base file (if it's an autosave)
        std::string getFileBaseName(const FileInfo& autosaveInfo);
        std::filesystem::path getFileBasePath(const FileInfo& autosaveInfo);

        void onCurrentFileSaveSuccess();
        void onCurrentFileInfoChange();
        void onBaseFileListChange();
        void goToStartPage();

        std::optional<FileInfo> writeSchedule(const std::filesystem::path& path, Schedule& schedule);

        bool checkFileHasAutosave(const FileInfo& baseFile);
        // Apply the autosave of the given file to it.
        bool applyAutosaveToFile(const FileInfo& baseFile);
        // Delete the autosave for the provided file.
        // Return true if the autosave was deleted; false if no file was found or it wasn't deleted.
        bool deleteAutosaveFor(const FileInfo& baseFile);
        // Pass a schedule file to create an imgui .ini file for it.
        void createIniForFile(const FileInfo& file);

        // Cleans everything about the currently open file (clears the schedule, edit history, etc)
        void unloadCurrentFile();
        const filter_func& getScheduleFileFilter(ScheduleFileFilter filter) const;
        const sort_func& getScheduleFileSort(ScheduleFileSort sort) const;

    public:
        const char* INI_FILE_EXTENSION = ".ini";

        Event<std::optional<FileInfo>> openFileInfoChangeEvent;
        Event<FileInfo> fileReadEvent;
        Event<FileInfo> fileCreatedEvent;
        Event<> fileUnloadEvent;

        ScheduleIO() = delete;
        ScheduleIO(Schedule& schedule, Interface& interface, std::filesystem::path saveDir);

        std::optional<FileInfo> getCurrentFileInfo() const;
        // Returns true if a schedule file is currently loaded
        bool isThereFileOpen() const;

        bool readSchedule(const FileInfo& fileInfo);
        bool createNewSchedule(const char* name);
        bool deleteSchedule(const FileInfo& fileInfo);

        // Save the currently loaded file or do nothing if no file is loaded.
        bool saveCurrentFile();
        bool createCurrentFileAutosave();
        // Rename the currently open file to the provided name.
        // Cancelled if a file with that name already exists.
        // If the open file doesn't exist, write a file with the new name.
        // If the open file exists, rename it to the new name.
        bool renameCurrentFile(const std::string& newName);
        // Mostly just creates and applies an autosave of the file before it is unloaded by calling unloadCurrentFile().
        void closeCurrentFile();
        void openMostRecentFile();

        // Get the edit time of the file at filePath, wrapped in a TimeWrapper.
        TimeWrapper getFileEditTime(std::filesystem::path filePath) const;
        // Get the edit time of the directory entry, wrapped in a TimeWrapper.
        TimeWrapper getFileEditTime(const std::filesystem::directory_entry& file) const;

        // Get a list of FileInfo for all schedule files.
        // Use the filter and sort enums to filter files and sort the list, respectively.
        // If no arguments are provided, returns a list of all schedule file infos, unsorted.
        std::vector<FileInfo> getScheduleFileInfoList(ScheduleFileFilter filter = ScheduleFileFilter::All,
                                                      ScheduleFileSort sort = ScheduleFileSort::None) const;
        // Shortcut for getting the first element of getScheduleFileInfoList sorted by edit time.
        // Returns nullopt if there are no schedule files.
        std::optional<FileInfo> getLastEditedFileInfo() const;
};
