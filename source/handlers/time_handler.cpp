#include "time_handler.h"

void TimeHandler::init(IO_Handler& ioHandler, Schedule& schedule)
{
    ioHandler.fileReadEvent.addListener(fileReadListener);
    ioHandler.fileUnloadEvent.addListener(fileUnloadListener);
    m_schedule = &schedule;
}

void TimeHandler::applyResetsSince(const TimeWrapper& previousTime)
{
    TimeWrapper currentTime = TimeWrapper::getCurrentTime();
    int daysSince = TimeWrapper::getDifference<days>(currentTime, previousTime);

    // Same or previous date, don't reset
    if (daysSince < 1) { return; }

    // Map column indices to their reset option
    std::map<ColumnResetOption, std::vector<size_t>> columnsByResetOption =
    {
        { ColumnResetOption::Daily, {} },
        { ColumnResetOption::Weekly, {} },
        { ColumnResetOption::Monthly, {} },
    };
    
    for (size_t i = 0; i < m_schedule->getAllColumns().size(); i++)
    {
        const Column& column = m_schedule->getAllColumns().at(i);

        if (column.resetOption == ColumnResetOption::Never) { continue; }
        
        columnsByResetOption.at(column.resetOption).push_back(i);
    }

    // Apply daily resets
    for (size_t columnIndex : columnsByResetOption.at(ColumnResetOption::Daily))
    {
        m_schedule->resetColumn(columnIndex, false);
    }

    // Enough days since previous time to get to the next week
    if (daysSince > (7 - previousTime.getWeekday()))
    {
        for (size_t columnIndex : columnsByResetOption.at(ColumnResetOption::Weekly))
        {
            m_schedule->resetColumn(columnIndex, false);
        }
    }

    int monthsSince = TimeWrapper::getDifference<months>(currentTime, previousTime);
    // New month
    if (monthsSince > 0)
    {
        for (size_t columnIndex : columnsByResetOption.at(ColumnResetOption::Monthly))
        {
            m_schedule->resetColumn(columnIndex, false);
        }
    }
}

void TimeHandler::applyResetsSinceEditTime(TimeWrapper lastEditTime)
{
    applyResetsSince(lastEditTime);
    m_lastTickTime = TimeWrapper::getCurrentTime();
}

void TimeHandler::timeTick()
{
    // The last tick time is full when a file is open.
    if (m_lastTickTime.getIsEmpty()) { return; }

    TimeWrapper currentTime = TimeWrapper::getCurrentTime();

    // Apply any resets since the last tick time.
    applyResetsSince(m_lastTickTime);
}

void TimeHandler::handleFileUnloaded()
{
    // Clear last tick time so the handler doesn't try to do resets when no file is even loaded
    m_lastTickTime.clear();
}