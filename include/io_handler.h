#pragma once

#include <functional>
#include "data_converter.h"
#include "schedule.h"
#include "window.h"
#include "input.h"
#include "main_menu_bar_gui.h"
#include "interface.h"

const unsigned int AUTOSAVE_DELAY_SECONDS = 2 * 60;

class IO_Handler
{
    private:
        Schedule* m_schedule = NULL;
        Window* m_windowManager = NULL;
        DataConverter m_converter;
        std::shared_ptr<MainMenuBarGui> m_mainMenuBarGui = NULL;
        std::string m_openScheduleFilename;
        std::string makeRelativePathFromName(const char* name);
        double m_timeSinceAutosave = 0.0;

        // input listeners
        std::function<void()> saveListener = std::function<void()>([&]()
        {
            writeSchedule(m_openScheduleFilename.c_str());
        });
        // gui listeners
        // ScheduleNameModalSubGui
        std::function<void(const std::string&, const bool&)> renameListener = std::function<void(const std::string&, const bool&)>([&](const std::string& name, const bool& renameFile)
        {
            if (setOpenScheduleFilename(name, renameFile))
            {
                m_mainMenuBarGui->closeScheduleNameModal();
            }
        });
        std::function<void(const std::string&)> createNewListener = std::function<void(const std::string&)>([&](const std::string& name)
        {
            if (createNewSchedule(name.c_str()))
            {
                m_mainMenuBarGui->closeScheduleNameModal();
            }
        });
        // ScheduleDeleteModalSubGui
        std::function<void(const std::string&)> deleteListener = std::function<void(const std::string&)>([&](const std::string& name)
        {
            deleteSchedule(name.c_str());
            // Modal hides itself
        });
        // MainMenuBarGui
        std::function<void(const std::string&)> openListener = std::function<void(const std::string&)>([&](const std::string& name)
        {
            readSchedule(name.c_str());
        });

    public:
        const char* SCHEDULES_SUBDIR_PATH = "./schedules/";
        const char* SCHEDULE_FILE_EXTENSION = ".blf";
        void init(Schedule* schedule, Window* window, Input& input, Interface& interface);
        bool writeSchedule(const char* name);
        bool readSchedule(const char* name);
        bool createNewSchedule(const char* name);
        bool deleteSchedule(const char* name);
        void addToAutosaveTimer(double delta);
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
