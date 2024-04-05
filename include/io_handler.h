#pragma once

#include <data_converter.h>
#include <schedule.h>
#include <input.h>

const unsigned int AUTOSAVE_DELAY_SECONDS = 2 * 60;

class IO_Handler
{
    private:
        Schedule* m_schedule;
        DataConverter m_converter;
        std::string m_openScheduleFilename;
        std::string makeRelativePathFromName(const char* name);
        double m_timeSinceAutosave = 0.0;

        // input listeners
        std::function<void()> saveCallback = std::function<void()>([&]()
        {
            writeSchedule(m_openScheduleFilename.c_str());
        });
    public:
        const char* SCHEDULES_SUBDIR_PATH = "./schedules/";
        const char* SCHEDULE_FILE_EXTENSION = ".blf";
        void init(Schedule* schedule, Input& input);
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
