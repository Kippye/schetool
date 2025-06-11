#include "time_handler.h"

void TimeHandler::init(IO_Handler& ioHandler, Schedule& schedule, NotificationHandler& notificationHandler) {
    auto scheduleIO = ioHandler.getScheduleIO();
    if (scheduleIO) {
        scheduleIO->fileReadEvent.addListener(fileOpenListener);
        scheduleIO->fileCreatedEvent.addListener(fileOpenListener);
        scheduleIO->fileUnloadEvent.addListener(fileUnloadListener);
    }

    notificationHandler.notificationActivatedEvent.addListener(notificationActivatedListener);
    m_schedule = &schedule;
    m_notificationHandler = &notificationHandler;
}

std::pair<size_t, size_t> TimeHandler::countTodayAndCompletedItems() const {
    auto& scheduleColumns = m_schedule->getAllColumns();
    size_t todayItemCount = 0;
    size_t todayCompletedItemCount = 0;

    for (size_t row = 0; row < m_schedule->getRowCount(); row++) {
        bool isVisibleToday = false;
        bool isCompletedToday = false;
        for (const Column& col : scheduleColumns) {
            // NOT the "Finished" column
            if (!(col.flags & ScheduleColumnFlags_Finished)) {
                // Check if the row's Element passes every FilterGroup in this Column
                isVisibleToday = col.checkElementPassesFilters(row);
                if (isVisibleToday == false) {
                    break;
                }
            }
            // "Finished" column
            else
            {
                isCompletedToday =
                    m_schedule->getElementValue<bool>(m_schedule->getFlaggedColumnIndex(ScheduleColumnFlags_Finished), row);
            }
        }
        if (isVisibleToday) {
            todayItemCount++;
            // Only add completed items that are visible based on other columns' filtering
            if (isCompletedToday) {
                todayCompletedItemCount++;
            }
        }
    }

    return {todayItemCount, todayCompletedItemCount};
}

void TimeHandler::applyResetsSince(const TimeWrapper& previousTime) {
    TimeWrapper currentTime = TimeWrapper::getCurrentTime();
    int daysSince = TimeWrapper::getDifference<std::chrono::days>(currentTime, previousTime);

    // Same or previous date, don't reset
    if (daysSince < 1) {
        return;
    }

    // Map column indices to their reset option
    std::map<ColumnResetOption, std::vector<size_t>> columnsByResetOption = {
        {ColumnResetOption::Daily, {}},
        {ColumnResetOption::Weekly, {}},
        {ColumnResetOption::Monthly, {}},
    };

    for (size_t i = 0; i < m_schedule->getAllColumns().size(); i++) {
        const Column& column = m_schedule->getAllColumns().at(i);

        if (column.resetOption == ColumnResetOption::Never) {
            continue;
        }

        columnsByResetOption.at(column.resetOption).push_back(i);
    }

    // Apply daily resets
    for (size_t columnIndex : columnsByResetOption.at(ColumnResetOption::Daily)) {
        m_schedule->applyColumnTimeBasedReset(columnIndex);
    }

    // Enough days since previous time to get to the next week
    if (daysSince > (7 - previousTime.getWeekday())) {
        for (size_t columnIndex : columnsByResetOption.at(ColumnResetOption::Weekly)) {
            m_schedule->applyColumnTimeBasedReset(columnIndex);
        }
    }

    int monthsSince = TimeWrapper::getDifference<std::chrono::months>(currentTime, previousTime);
    // New month
    if (monthsSince > 0) {
        for (size_t columnIndex : columnsByResetOption.at(ColumnResetOption::Monthly)) {
            m_schedule->applyColumnTimeBasedReset(columnIndex);
        }
    }
}

void TimeHandler::applyResetsSinceEditTime(TimeWrapper lastEditTime) {
    applyResetsSince(lastEditTime);
    m_lastTickTime = TimeWrapper::getCurrentTime();
}

void TimeHandler::showItemStartNotifications(const TimeWrapper& currentTime, const TimeWrapper& previousTime) {
    auto& scheduleColumns = m_schedule->getAllColumns();
    size_t startColumnIndex = m_schedule->getFlaggedColumnIndex(ScheduleColumnFlags_Start);
    const Column* startColumn = m_schedule->getColumn(startColumnIndex);

    auto [todayItemCount, todayCompletedItemCount] = countTodayAndCompletedItems();

    for (size_t row = 0; row < m_schedule->getRowCount(); row++) {
        // FIRST check if the item is even visible
        bool isItemVisible = false;
        for (const Column& col : scheduleColumns) {
            // Check if the row's Element passes every FilterGroup in this Column
            isItemVisible =
                col.checkElementPassesFilters(row
                                              // NOTE: Do i use override time here?
                                              // Usually the override time applies when viewing a different date
                                              // But you still only want to get notifications for the actual current date
                                              // So for now, no. We will let it use TimeWrapper::getCurrentTime() by default.
                );
            if (isItemVisible == false) {
                break;
            }
        }
        // Only send notifications for the beginning of visible schedule items
        if (isItemVisible == false) {
            continue;
        }

        auto startElementTimeContainer = m_schedule->getElementAsSpecial<TimeContainer>(startColumnIndex, row);
        // Create a TimeWrapper from the current local date and the item start time (interpreted as UTC date-time but they are actually local)
        // Then subtract the time zone offset to get a correct UTC date-time TimeWrapper
        TimeWrapper itemStartTime = TimeWrapper::getTimeWithOffsetSubtracted(
            TimeWrapper(currentTime.getLocalDate(),
                        ClockTimeWrapper(startElementTimeContainer->getConstValueReference().getHours(),
                                         startElementTimeContainer->getConstValueReference().getMinutes())));
        // The element's start time was reached just this frame
        if (currentTime >= itemStartTime && previousTime < itemStartTime) {
            size_t nameColumnIndex = m_schedule->getFlaggedColumnIndex(ScheduleColumnFlags_Name);
            size_t endColumnIndex = m_schedule->getFlaggedColumnIndex(ScheduleColumnFlags_End);
            auto endElementTimeContainer = m_schedule->getElementAsSpecial<TimeContainer>(endColumnIndex, row);
            ClockTimeWrapper itemEndTime = ClockTimeWrapper(endElementTimeContainer->getConstValueReference().getHours(),
                                                            endElementTimeContainer->getConstValueReference().getMinutes());
            // Send a notification about the element starting
            m_notificationHandler->showItemNotification(
                m_schedule->getElementAsSpecial<std::string>(nameColumnIndex, row)->getValue(),
                itemStartTime.getLocalClockTime(),
                itemEndTime,
                {todayCompletedItemCount, todayItemCount});
        }
    }
}

void TimeHandler::completePreviousItem(const ClockTimeWrapper& startTime) {
    auto scheduleColumns = m_schedule->getAllColumns();
    size_t startColumnIndex = m_schedule->getFlaggedColumnIndex(ScheduleColumnFlags_Start);
    size_t finishedColumnIndex = m_schedule->getFlaggedColumnIndex(ScheduleColumnFlags_Finished);
    const Column* startColumn = m_schedule->getColumn(startColumnIndex);
    const Column* finishedColumn = m_schedule->getColumn(finishedColumnIndex);

    ClockTimeWrapper closestPreviousItemTime = startTime;
    size_t previousItemRow = 0;

    for (size_t row = 0; row < m_schedule->getRowCount(); row++) {
        // FIRST check if the item is even visible (exclude Finished, we want to know if the closest previous item is already finished so we can decide to do nothing)
        bool isItemCurrentlyVisible = false;
        for (size_t i = 0; i < m_schedule->getColumnCount(); i++) {
            if (i != finishedColumnIndex) {
                // Check if the row's Element passes every FilterGroup in this Column
                isItemCurrentlyVisible = m_schedule->getColumn(i)->checkElementPassesFilters(
                    row
                    // NOTE: Do i use override time here?
                    // Usually the override time applies when viewing a different date
                    // But you still only want to get notifications for the actual current date
                    // So for now, no. We will let it use TimeWrapper::getCurrentTime() by default.
                );
            }
            if (isItemCurrentlyVisible == false) {
                break;
            }
        }
        // Skip items that are not visible right now
        if (isItemCurrentlyVisible == false) {
            continue;
        }
        // Compare the item's start time
        TimeContainer startValue = m_schedule->getElementValue<TimeContainer>(startColumnIndex, row);
        ClockTimeWrapper itemStartTime = ClockTimeWrapper(startValue.getHours(), startValue.getMinutes());
        // First previous time or a closer time than the current closest
        if (itemStartTime < startTime && (closestPreviousItemTime == startTime || itemStartTime > closestPreviousItemTime)) {
            closestPreviousItemTime = itemStartTime;
            previousItemRow = row;
        }
    }

    if (closestPreviousItemTime < startTime) {
        m_schedule->setElementValue(finishedColumnIndex, previousItemRow, true);
    }
}

void TimeHandler::handleFileUnloaded() {
    // Clear last tick time so the handler doesn't try to do resets when no file is even loaded
    m_lastTickTime.clear();
}

void TimeHandler::timeTick() {
    // The last tick time is full when a file is open.
    if (m_lastTickTime.getIsEmpty()) {
        return;
    }

    TimeWrapper currentTime = TimeWrapper::getCurrentTime();
    showItemStartNotifications(currentTime, m_lastTickTime);
    // Apply any resets since the last tick time.
    applyResetsSince(m_lastTickTime);
    m_lastTickTime = currentTime;
}