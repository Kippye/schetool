#include "time_handler.h"

void TimeHandler::init(IO_Handler& ioHandler, Schedule& schedule, NotificationHandler& notificationHandler)
{
    ioHandler.fileReadEvent.addListener(fileOpenListener);
    ioHandler.fileCreatedEvent.addListener(fileOpenListener);
    ioHandler.fileUnloadEvent.addListener(fileUnloadListener);
    m_schedule = &schedule;
    m_notificationHandler = &notificationHandler;
}

void TimeHandler::applyResetsSince(const TimeWrapper& previousTime)
{
    TimeWrapper currentTime = TimeWrapper::getCurrentTime();
    int daysSince = TimeWrapper::getDifference<std::chrono::days>(currentTime, previousTime);

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
        m_schedule->applyColumnTimeBasedReset(columnIndex);
    }

    // Enough days since previous time to get to the next week
    if (daysSince > (7 - previousTime.getWeekday()))
    {
        for (size_t columnIndex : columnsByResetOption.at(ColumnResetOption::Weekly))
        {
            m_schedule->applyColumnTimeBasedReset(columnIndex);
        }
    }

    int monthsSince = TimeWrapper::getDifference<std::chrono::months>(currentTime, previousTime);
    // New month
    if (monthsSince > 0)
    {
        for (size_t columnIndex : columnsByResetOption.at(ColumnResetOption::Monthly))
        {
            m_schedule->applyColumnTimeBasedReset(columnIndex);
        }
    }
}

void TimeHandler::applyResetsSinceEditTime(TimeWrapper lastEditTime)
{
    applyResetsSince(lastEditTime);
    m_lastTickTime = TimeWrapper::getCurrentTime();
}

void TimeHandler::showItemStartNotifications(const TimeWrapper& currentTime, const TimeWrapper& previousTime)
{
    // ELEMENT START NOTIFICATIONS
    auto scheduleColumns = m_schedule->getAllColumns();
    size_t startColumnIndex = m_schedule->getFlaggedColumnIndex(ScheduleColumnFlags_Start);
    const Column* startColumn = m_schedule->getColumn(startColumnIndex);

    for (size_t row = 0; row < m_schedule->getRowCount(); row++)
    {
        // FIRST check if the item is even visible
        bool isItemVisible = false;
        for (const Column& col: scheduleColumns)
        {
            // Check if the row's Element passes every FilterGroup in this Column
            isItemVisible = col.checkElementPassesFilters(row
                // NOTE: Do i use override time here?
                // Usually the override time applies when viewing a different date
                // But you still only want to get notifications for the actual current date
                // So for now, no. We will let it use TimeWrapper::getCurrentTime() by default.
            );
            if (isItemVisible == false)
            {
                break;
            }
        }
        // Only send notifications for the beginning of visible schedule items
        if (isItemVisible == false)
        {
            continue;
        }

        auto elementTimeContainer = m_schedule->getElementAsSpecial<TimeContainer>(startColumnIndex, row);
        TimeWrapper elementTime = TimeWrapper::getTimeWithOffsetSubtracted(TimeWrapper(currentTime.getDateUTC(), ClockTimeWrapper(elementTimeContainer->getConstValueReference().getHours(), elementTimeContainer->getConstValueReference().getMinutes())));
        // The element's start time was reached just this frame
        if (currentTime >= elementTime && previousTime < elementTime)
        {
            size_t nameColumnIndex = m_schedule->getFlaggedColumnIndex(ScheduleColumnFlags_Name);
            size_t endColumnIndex = m_schedule->getFlaggedColumnIndex(ScheduleColumnFlags_End);
            auto endElementTimeContainer = m_schedule->getElementAsSpecial<TimeContainer>(endColumnIndex, row);
            ClockTimeWrapper endTime = ClockTimeWrapper(endElementTimeContainer->getConstValueReference().getHours(), endElementTimeContainer->getConstValueReference().getMinutes());
            // Send a notification about the element starting
            m_notificationHandler->showElementNotification
            (
                m_schedule->getElementAsSpecial<std::string>(nameColumnIndex, row)->getValue(),
                elementTime.getLocalClockTime(),
                endTime
            );
        }
    }
}

void TimeHandler::handleFileUnloaded()
{
    // Clear last tick time so the handler doesn't try to do resets when no file is even loaded
    m_lastTickTime.clear();
}


void TimeHandler::timeTick()
{
    // The last tick time is full when a file is open.
    if (m_lastTickTime.getIsEmpty()) { return; }

    TimeWrapper currentTime = TimeWrapper::getCurrentTime();
    showItemStartNotifications(currentTime, m_lastTickTime);
    // Apply any resets since the last tick time.
    applyResetsSince(m_lastTickTime);
    m_lastTickTime = currentTime;
}