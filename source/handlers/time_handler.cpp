#include "time_handler.h"

void TimeHandler::init(IO_Handler& ioHandler, Schedule& schedule)
{
    ioHandler.fileReadEvent.addListener(fileReadListener);
    m_schedule = &schedule;
}

void TimeHandler::applyResetsSince(const TimeWrapper& previousTime)
{
    TimeWrapper currentTime = TimeWrapper::getCurrentSystemTime();
    DateContainer currentDate = DateContainer(currentTime);
    int daysSince = currentDate.getDayDifference(DateContainer(previousTime));

    // Same or previous date, don't reset
    if (daysSince < 1) { return; }

    // Apply daily resets

    // Enough days since previous time to get to the next week
    if (daysSince > (7 - previousTime.getWeekDay()))
    {
        // Apply weekly resets
    }

    // New month
    if (currentTime.getYear() > previousTime.getYear() || currentTime.getMonth() > previousTime.getMonth())
    {
        // Apply monthly resets
    }
}

void TimeHandler::applyResetsSinceEditTime(TimeWrapper lastEditTime)
{
    applyResetsSince(lastEditTime);
}

void TimeHandler::timeTick()
{
    
}