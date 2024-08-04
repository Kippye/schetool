#pragma once

#include <functional>
#include "io_handler.h"
#include "schedule.h"

class TimeTracker
{
    private:
        ScheduleCore* m_scheduleCore;
    public:
        void init(IO_Handler& ioHandler, Schedule& schedule);
        void timeTick();
};