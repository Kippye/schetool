#pragma once

#include <functional>
#include <filesystem>
#include <string>
#include "event.h"
#include "schedule_data_converter.h"
#include "schedule.h"
#include "window.h"
#include "input.h"
#include "file_info.h"
#include "start_page_gui.h"
#include "schedule/schedule_gui.h"
#include "main_menu_bar/main_menu_bar_gui.h"
#include "autosave_popup_gui.h"
#include "interface.h"

class ScheduleIO {
    private:
        Schedule& m_schedule;
        ScheduleDataConverter m_converter;
        std::shared_ptr<StartPageGui> m_startPageGui = NULL;
        std::shared_ptr<MainMenuBarGui> m_mainMenuBarGui = NULL;
        std::shared_ptr<AutosavePopupGui> m_autosavePopupGui = NULL;
        std::shared_ptr<ScheduleGui> m_scheduleGui = NULL;
        FileInfo m_currentFileInfo = FileInfo();
        std::filesystem::path m_saveDir = std::filesystem::path();
        const char* m_autosaveSuffix = "_auto";

        std::function<void()> saveListener = std::function<void()>([&]() {
            if (m_currentFileInfo.empty()) {
                return;
            }
            writeSchedule(m_currentFileInfo.getName().c_str());
        });
        // gui listeners
        // ScheduleNameModalSubGui
        std::function<void(std::string)> renameListener = std::function<void(std::string)>([&](std::string name) {
            if (renameCurrentFile(name)) {
                m_mainMenuBarGui->closeModal();
            }
        });
        std::function<void(std::string)> createNewListener = std::function<void(std::string)>([&](std::string name) {
            closeCurrentFile();
            if (createNewSchedule(name.c_str())) {
                m_mainMenuBarGui->closeModal();
            }
        });
        // DeleteModalSubGui
        std::function<void(std::string)> deleteListener = std::function<void(std::string)>([&](std::string name) {
            deleteSchedule(name.c_str());
            // Modal hides itself
        });
        // MainMenuBarGui
        std::function<void(std::string)> openListener = std::function<void(std::string)>([&](std::string name) {
            closeCurrentFile();
            readSchedule(name.c_str());
        });
        // AutosavePopupGui
        // NOTE: all of these assume that the most recently edited file is still an autosave
        // TODO: handle the (rare?) case where it isn't
        std::function<void()> applyAutosaveListener = std::function<void()>([&]() {
            std::string lastEditedFileName = getLastEditedScheduleStemName();
            // 99% of the time, the autosave will be the most recent
            if (isAutosave(lastEditedFileName)) {
                std::string baseFileName = getFileBaseName(lastEditedFileName.c_str());
                // Apply autosave to file
                applyAutosaveToFile(baseFileName.c_str());
                // Read the updated base file
                readSchedule(baseFileName.c_str());
            }
            // Somehow, the most recently edited file was an autosave but now isn't?
            else
            {
            }
        });
        std::function<void()> deleteAutosaveListener = std::function<void()>([&]() {
            std::string lastEditedFileName = getLastEditedScheduleStemName();
            std::string baseFileName = getFileBaseName(lastEditedFileName.c_str());
            // 99% of the time, the autosave will be the most recent
            if (isAutosave(lastEditedFileName)) {
                deleteSchedule(lastEditedFileName.c_str());
            }
            // Somehow, the most recently edited file was an autosave but now isn't?
            // The base file will be opened anyway
            else
            {
                // Get the autosave name from the base file name and try to delete it.
                deleteSchedule(getFileAutosaveName(lastEditedFileName.c_str()).c_str());
            }
            readSchedule(baseFileName.c_str());
        });

        // Returns true if the path has the schedule file extension.
        bool isScheduleFilePath(const std::filesystem::path& path) const;
        // Returns true if the path points to a file that can be loaded by the ScheduleDataConverter.
        bool isValidScheduleFile(const std::filesystem::path& path) const;
        std::filesystem::path makeSchedulePathFromName(const char* name) const;
        std::filesystem::path makeIniPathFromScheduleName(const char* name) const;
        // Apply the autosave of the given file to it.
        // NOTE: The requested name is the name of the *base file*.
        bool applyAutosaveToFile(const char* name);
        // Pass the file name of a schedule file to create an imgui .ini file for it.
        void createIniForFile(const char* name);
        void sendFileInfoUpdates();
        void passFileNamesToGui();
        // Cleans everything about the currently open file (clears the schedule, edit history, etc)
        void unloadCurrentFile();

    public:
        const char* INI_FILE_EXTENSION = ".ini";

        Event<FileInfo> openFileInfoChangeEvent;
        Event<FileInfo> fileReadEvent;
        Event<FileInfo> fileCreatedEvent;
        Event<> fileUnloadEvent;

        ScheduleIO(Schedule& schedule, Interface& interface, std::filesystem::path saveDir);

        bool writeSchedule(const char* name);
        bool readSchedule(const char* name);
        bool createNewSchedule(const char* name);
        bool deleteSchedule(const char* name);
        // Rename the currently open file to the provided name.
        // Cancelled if a file with that name already exists.
        // If the open file doesn't exist, write a file with the new name.
        // If the open file exists, rename it to the new name.
        bool renameCurrentFile(const std::string& newName);
        // Mostly just creates and applies an autosave of the file before it is unloaded by calling unloadCurrentFile().
        void closeCurrentFile();
        FileInfo getCurrentFileInfo() const;
        void openMostRecentFile();
        bool createAutosave();
        bool isAutosave(const std::string& fileName);
        std::string getFileAutosaveName(const char* fileName);
        // Remove the autosave suffix from a file name.
        // If the file name already isn't that of an autosave, the initial name is be returned.
        std::string getFileBaseName(const char* autosaveName);
        // TODO: Use an optional<long long> instead of throwing on error?
        long long getFileEditTime(std::filesystem::path filePath);
        TimeWrapper getFileEditTimeWrapped(std::filesystem::path filePath);
        std::string getFileEditTimeString(std::filesystem::path filePath);
        std::vector<std::string> getScheduleStemNames(bool includeAutosaves = true);
        std::vector<std::string> getScheduleStemNamesSortedByEditTime(bool includeAutosaves = true);
        std::string getLastEditedScheduleStemName();
};
