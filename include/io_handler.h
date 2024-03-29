#pragma once

#include <data_converter.h>
#include <schedule.h>

class IO_Handler
{
    private:
        Schedule* m_schedule;
        DataConverter m_converter;
        const char* m_openScheduleFilename;
        std::string makeRelativePathFromName(const char* name);
    public:
        const char* SCHEDULES_SUBDIR_PATH = ".\\schedules\\";
        const char* SCHEDULE_FILE_EXTENSION = ".blf";
        void init(Schedule* schedule);
        bool writeSchedule(const char* name);
        bool readSchedule(const char* name);
        bool createNewSchedule(const char* name);
        const char* getOpenScheduleFilename();
        std::vector<std::string> getScheduleStemNames();
        std::string getLastEditedScheduleStemName();
};