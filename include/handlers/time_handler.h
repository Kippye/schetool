#pragma once

#include <functional>
#include "io_handler.h"
#include "schedule.h"
#include "time_wrapper.h"

class TimeHandler
{
    private:
        TimeWrapper m_lastTickTime = TimeWrapper();
        Schedule* m_schedule;

        std::function<void(FileInfo)> fileReadListener = [&](FileInfo fileInfo)
        {
            applyResetsSinceEditTime(fileInfo.getScheduleEditTime());
        };
        std::function<void()> fileUnloadListener = [&]()
        {
            handleFileUnloaded();
        };

        void applyResetsSince(const TimeWrapper& previousTime);
        // Called by TimeHandler::fileReadListener.
        void applyResetsSinceEditTime(TimeWrapper lastEditTime);
        void handleFileUnloaded();
    public:
        void init(IO_Handler& ioHandler, Schedule& schedule);
        // Applies any resets that should have taken place since the read file's last schedule edit time.
        void timeTick();
};