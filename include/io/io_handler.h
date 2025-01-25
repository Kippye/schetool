#pragma once

#include <functional>
#include <filesystem>
#include <string>
#include "event.h"
#include "data_converter.h"
#include "schedule.h"
#include "window.h"
#include "input.h"
#include "file_info.h"
#include "start_page_gui.h"
#include "schedule_gui.h"
#include "main_menu_bar_gui.h"
#include "autosave_popup_gui.h"
#include "interface.h"

const unsigned int AUTOSAVE_DELAY_SECONDS = 1 * 60;

class IO_Handler
{
    private:
        Schedule* m_schedule = NULL;
        Window* m_windowManager = NULL;
        DataConverter m_converter;
        std::shared_ptr<StartPageGui> m_startPageGui = NULL;
        std::shared_ptr<MainMenuBarGui> m_mainMenuBarGui = NULL;
        std::shared_ptr<AutosavePopupGui> m_autosavePopupGui = NULL;
        std::shared_ptr<ScheduleGui> m_scheduleGui = NULL;
        FileInfo m_currentFileInfo = FileInfo();
        double m_timeSinceAutosave = 0.0;
        std::filesystem::path m_saveDir = std::filesystem::path();
        const char* m_autosaveSuffix = "_auto";

        // input listeners
        std::function<void()> saveListener = std::function<void()>([&]()
        {
            if (m_currentFileInfo.empty()) { return; }
            writeSchedule(m_currentFileInfo.getName().c_str());
        });
        // window event listeners
        std::function<void()> windowCloseListener = std::function<void()>([&]()
        {
            closeCurrentFile();
        });
        // gui listeners
        // ScheduleNameModalSubGui
        std::function<void(std::string)> renameListener = std::function<void(std::string)>([&](std::string name)
        {
            if (renameCurrentFile(name))
            {
                m_mainMenuBarGui->closeModal();
            }
        });
        std::function<void(std::string)> createNewListener = std::function<void(std::string)>([&](std::string name)
        {
            closeCurrentFile();
            if (createNewSchedule(name.c_str()))
            {
                m_mainMenuBarGui->closeModal();
            }
        });
        // DeleteModalSubGui
        std::function<void(std::string)> deleteListener = std::function<void(std::string)>([&](std::string name)
        {
            deleteSchedule(name.c_str());
            // Modal hides itself
        });
        // MainMenuBarGui
        std::function<void(std::string)> openListener = std::function<void(std::string)>([&](std::string name)
        {
            closeCurrentFile();
            readSchedule(name.c_str());
        });
        // AutosavePopupGui
        // NOTE: all of these assume that the most recently edited file is still an autosave
        // TODO: handle the (rare?) case where it isn't
        std::function<void()> openAutosaveListener = std::function<void()>([&]()
        {
            readSchedule(getLastEditedScheduleStemName().c_str());
        });
        std::function<void()> ignoreAutosaveOpenFileEvent = std::function<void()>([&]()
        {
            readSchedule(getFileBaseName(getLastEditedScheduleStemName().c_str()).c_str());
        });

        // Chooses the most suitable save directory based on the current platform and existence of certain directories
        std::filesystem::path getBestSaveDirPath() const;
        // Returns true if the path has the schedule file extension.
        bool isScheduleFilePath(const std::filesystem::path& path) const;
        // Returns true if the path points to a file that can be loaded by the DataConverter.
        bool isValidScheduleFile(const std::filesystem::path& path) const;
        std::filesystem::path makeSchedulePathFromName(const char* name) const;
        std::filesystem::path makeIniPathFromScheduleName(const char* name) const;
        bool applyAutosaveToFile(const char* name);
        void sendFileInfoUpdates();
        void passFileNamesToGui();
        // Cleans everything about the currently open file (clears the schedule, edit history, etc)
        void unloadCurrentFile();
        // Mostly just creates and applies an autosave of the file before it is unloaded by calling unloadCurrentFile().
        void closeCurrentFile();
    public:
        const char* SCHEDULE_FILE_EXTENSION = ".blf";
        const char* INI_FILE_EXTENSION = ".ini";

        Event<FileInfo> fileReadEvent;
        Event<FileInfo> fileCreatedEvent;
        Event<> fileUnloadEvent;

        void init(Schedule* schedule, Window* window, Input& input, Interface& interface);

        bool writeSchedule(const char* name);
        bool readSchedule(const char* name);
        bool createNewSchedule(const char* name);
        bool deleteSchedule(const char* name);
        // Rename the currently open file to the provided name.
        // Cancelled if a file with that name already exists.
        // If the open file doesn't exist, write a file with the new name.
        // If the open file exists, rename it to the new name.
        bool renameCurrentFile(const std::string& newName);
        FileInfo getCurrentFileInfo() const;
        void openMostRecentFile();
        bool createAutosave();
        void addToAutosaveTimer(double delta);
        bool isAutosave(const std::string& fileName);
        std::string getFileAutosaveName(const char* fileName);
        std::string getFileBaseName(const char* autosaveName);
        long long getFileEditTime(std::filesystem::path filePath);
        TimeWrapper getFileEditTimeWrapped(std::filesystem::path filePath);
        std::string getFileEditTimeString(std::filesystem::path filePath);
        std::vector<std::string> getScheduleStemNames(bool includeAutosaves = true);
        std::vector<std::string> getScheduleStemNamesSortedByEditTime(bool includeAutosaves = true);
        std::string getLastEditedScheduleStemName();
};
