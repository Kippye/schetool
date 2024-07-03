#pragma once

#include <functional>
#include <filesystem>
#include <string>
#include "data_converter.h"
#include "schedule.h"
#include "window.h"
#include "input.h"
#include "main_menu_bar_gui.h"
#include "autosave_popup_gui.h"
#include "interface.h"

const unsigned int AUTOSAVE_DELAY_SECONDS = 2 * 60;

class IO_Handler
{
    private:
        Schedule* m_schedule = NULL;
        Window* m_windowManager = NULL;
        DataConverter m_converter;
        std::shared_ptr<MainMenuBarGui> m_mainMenuBarGui = NULL;
        std::shared_ptr<AutosavePopupGui> m_autosavePopupGui = NULL;
        std::string m_openScheduleFilename;
        double m_timeSinceAutosave = 0.0;
        const char* m_autosaveSuffix = "_auto";

        // input listeners
        std::function<void()> saveListener = std::function<void()>([&]()
        {
            writeSchedule(m_openScheduleFilename.c_str());
        });
        // window event listeners
        std::function<void()> windowCloseListener = std::function<void()>([&]()
        {
            closeCurrentFile();
        });
        // gui listeners
        // ScheduleNameModalSubGui
        std::function<void(std::string, bool)> renameListener = std::function<void(std::string, bool)>([&](std::string name, bool renameFile)
        {
            if (setOpenScheduleFilename(name, renameFile))
            {
                m_mainMenuBarGui->closeScheduleNameModal();
            }
        });
        std::function<void(std::string)> createNewListener = std::function<void(std::string)>([&](std::string name)
        {
            closeCurrentFile();
            if (createNewSchedule(name.c_str()))
            {
                m_mainMenuBarGui->closeScheduleNameModal();
            }
        });
        // ScheduleDeleteModalSubGui
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
        std::function<void()> applyAutosaveOpenFileListener = std::function<void()>([&]()
        {
            std::string autosaveName = getLastEditedScheduleStemName();
            applyAutosaveToFile(getFileBaseName(autosaveName.c_str()).c_str());
            readSchedule(getFileBaseName(autosaveName.c_str()).c_str());
        });
        std::function<void()> ignoreAutosaveOpenFileEvent = std::function<void()>([&]()
        {
            readSchedule(getFileBaseName(getLastEditedScheduleStemName().c_str()).c_str());
        });

        std::string makeRelativePathFromName(const char* name);
        bool applyAutosaveToFile(const char* name);
    public:
        const char* SCHEDULES_SUBDIR_PATH = "./schedules/";
        const char* SCHEDULE_FILE_EXTENSION = ".blf";
        void init(Schedule* schedule, Window* window, Input& input, Interface& interface);

        // Does any necessary procedures between when a file is "closed" and the program closed or a new one opened
        void closeCurrentFile();
        bool writeSchedule(const char* name);
        bool readSchedule(const char* name);
        bool createNewSchedule(const char* name);
        bool deleteSchedule(const char* name);
        void openMostRecentFile();
        bool createAutosave();
        void addToAutosaveTimer(double delta);
        bool isAutosave(const std::string& fileName);
        std::string getFileAutosaveName(const char* fileName);
        std::string getFileBaseName(const char* autosaveName);
        long long getFileEditTime(std::filesystem::path filePath);
        std::string getFileEditTimeString(std::filesystem::path filePath);
        std::string getOpenScheduleFilename();
        // Rename the currently open file to the provided name.
        // Cancelled if a file with that name already exists.
        // If the open file doesn't exist, write a file with the new name.
        // If the open file exists, rename it to the new name.
        bool setOpenScheduleFilename(const std::string& name, bool renameFile = false);
        std::vector<std::string> getScheduleStemNames();
        std::vector<std::string> getScheduleStemNamesSortedByEditTime();
        std::string getLastEditedScheduleStemName();
};
