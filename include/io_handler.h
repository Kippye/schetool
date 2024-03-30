#pragma once

#include <data_converter.h>
#include <schedule.h>

class IO_Handler
{
    private:
        Schedule* m_schedule;
        DataConverter m_converter;
        std::string m_openScheduleFilename;
        std::string makeRelativePathFromName(const char* name);
    public:
        const char* SCHEDULES_SUBDIR_PATH = ".\\schedules\\";
        const char* SCHEDULE_FILE_EXTENSION = ".blf";
        void init(Schedule* schedule);
        bool writeSchedule(const char* name);
        bool readSchedule(const char* name);
        bool createNewSchedule(const char* name);
        std::string getOpenScheduleFilename();
        void setOpenScheduleFilename(const std::string& name, bool renameFile = false);
        std::vector<std::string> getScheduleStemNames();
        std::vector<std::string> getScheduleStemNamesSortedByEditTime();
        std::string getLastEditedScheduleStemName();
};