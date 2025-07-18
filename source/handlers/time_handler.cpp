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
    std::vector<Column> scheduleColumns = m_schedule->getAllColumns();
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

    for (size_t i = 0; i < m_schedule->getColumnCount(); i++) {
        const Column& column = m_schedule->getColumnConst(i);

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
    std::vector<Column> scheduleColumns = m_schedule->getAllColumns();
    size_t startColumnIndex = m_schedule->getFlaggedColumnIndex(ScheduleColumnFlags_Start);

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

        TimeContainer startElementTimeContainer = m_schedule->getElementValue<TimeContainer>(startColumnIndex, row);
        // Create a TimeWrapper from the current local date and the item start time (interpreted as UTC date-time but they are actually local)
        // Then subtract the time zone offset to get a correct UTC date-time TimeWrapper
        TimeWrapper itemStartTime = TimeWrapper::getTimeWithOffsetSubtracted(
            TimeWrapper(currentTime.getLocalDate(),
                        ClockTimeWrapper(startElementTimeContainer.getHours(), startElementTimeContainer.getMinutes())));
        // The element's start time was reached just this frame
        if (currentTime >= itemStartTime && previousTime < itemStartTime) {
            size_t nameColumnIndex = m_schedule->getFlaggedColumnIndex(ScheduleColumnFlags_Name);
            size_t endColumnIndex = m_schedule->getFlaggedColumnIndex(ScheduleColumnFlags_End);
            TimeContainer endElementTimeContainer = m_schedule->getElementValue<TimeContainer>(endColumnIndex, row);
            ClockTimeWrapper itemEndTime =
                ClockTimeWrapper(endElementTimeContainer.getHours(), endElementTimeContainer.getMinutes());
            // Send a notification about the element starting
            m_notificationHandler->showItemNotification(m_schedule->getElementValue<std::string>(nameColumnIndex, row),
                                                        itemStartTime.getLocalClockTime(),
                                                        itemEndTime,
                                                        {todayCompletedItemCount, todayItemCount});
        }
    }
}

void TimeHandler::completeItem(const NotificationInfo& notificationInfo) {
    if (notificationInfo.startTime.has_value() == false || notificationInfo.endTime.has_value() == false ||
        notificationInfo.itemName.has_value() == false)
    {
        return;
    }

    ClockTimeWrapper startTime = notificationInfo.startTime.value();
    ClockTimeWrapper endTime = notificationInfo.endTime.value();
    std::string name = notificationInfo.itemName.value();

    size_t startColumnIndex = m_schedule->getFlaggedColumnIndex(ScheduleColumnFlags_Start);
    size_t endColumnIndex = m_schedule->getFlaggedColumnIndex(ScheduleColumnFlags_End);
    size_t nameColumnIndex = m_schedule->getFlaggedColumnIndex(ScheduleColumnFlags_Name);
    size_t finishedColumnIndex = m_schedule->getFlaggedColumnIndex(ScheduleColumnFlags_Finished);

    for (size_t row = 0; row < m_schedule->getRowCount(); row++) {
        if (!m_schedule->checkPassesAllFilters(row)) {
            continue;
        }

        // Compare the item's start and end time
        TimeContainer itemStartValue = m_schedule->getElementValue<TimeContainer>(startColumnIndex, row);
        ClockTimeWrapper itemStartTime = ClockTimeWrapper(itemStartValue.getHours(), itemStartValue.getMinutes());
        TimeContainer itemEndValue = m_schedule->getElementValue<TimeContainer>(endColumnIndex, row);
        ClockTimeWrapper itemEndTime = ClockTimeWrapper(itemEndValue.getHours(), itemEndValue.getMinutes());
        std::string itemName = m_schedule->getElementValue<std::string>(nameColumnIndex, row);
        // Start and end times as well as the name are the same, mark the item as finished
        if (itemStartTime == startTime && itemEndTime == endTime && itemName == name) {
            m_schedule->setElementValue(finishedColumnIndex, row, true);
        }
    }
}

void TimeHandler::completePreviousItem(const NotificationInfo& notificationInfo) {
    if (notificationInfo.startTime.has_value() == false) {
        return;
    }

    ClockTimeWrapper startTime = notificationInfo.startTime.value();

    size_t startColumnIndex = m_schedule->getFlaggedColumnIndex(ScheduleColumnFlags_Start);
    size_t finishedColumnIndex = m_schedule->getFlaggedColumnIndex(ScheduleColumnFlags_Finished);

    ClockTimeWrapper closestPreviousItemTime = startTime;
    std::optional<size_t> previousItemRow = std::nullopt;

    for (size_t row = 0; row < m_schedule->getRowCount(); row++) {
        // FIRST check if the item is even visible (exclude Finished, we want to know if the closest previous item is already finished so we can decide to do nothing)
        // Skip items that are not visible right now
        if (!m_schedule->checkPassesAllFilters(row, std::nullopt, {finishedColumnIndex})) {
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

    if (previousItemRow.has_value() && closestPreviousItemTime < startTime &&
        m_schedule->getElementValue<bool>(finishedColumnIndex, previousItemRow.value()) == false)
    {
        m_schedule->setElementValue(finishedColumnIndex, previousItemRow.value(), true);
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