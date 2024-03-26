#pragma once

#include <data_converter.h>
#include <schedule.h>

class IO_Handler
{
    private:
        Schedule* m_schedule;
        DataConverter m_converter;
    public:
        void init(Schedule* schedule);
        void writeSchedule(const char* path);
        void readSchedule(const char* path);
};