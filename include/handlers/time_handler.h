#pragma once

#include "io_handler.h"
#include "schedule.h"
#include "time_wrapper.h"
#include "notification_handler.h"
#include <functional>
#include <utility>

class TimeHandler {
    private:
        TimeWrapper m_lastTickTime = TimeWrapper();
        Schedule* m_schedule;
        NotificationHandler* m_notificationHandler;

        std::function<void(FileInfo)> fileOpenListener = [&](FileInfo fileInfo) {
            applyResetsSinceEditTime(fileInfo.getScheduleEditTime());
        };
        std::function<void()> fileUnloadListener = [&]() { handleFileUnloaded(); };

        std::function<void(NotificationActivation, NotificationInfo)> notificationActivatedListener =
            [&](NotificationActivation activationType, NotificationInfo notificationInfo) {
                if (activationType == NotificationActivation::PreviousMarkedDone) {
                    if (notificationInfo.startTime.has_value()) {
                        completePreviousItem(notificationInfo.startTime.value());
                    }
                }
            };

        // Returns 2 counts:
        // 1. The amount of items that would be visible for today, not taking into account the Finished column
        // 2. The amount of items that would be visible today and whose Finished is checked.
        // This isn't really "standard" enough to put in Schedule/Core so it will be here.
        std::pair<size_t, size_t> countTodayAndCompletedItems() const;

        void applyResetsSince(const TimeWrapper& previousTime);
        // Show notifications for visible items that began this time tick.
        void showItemStartNotifications(const TimeWrapper& currentTime, const TimeWrapper& previousTime);
        // Basically, completes the item whose start time is before "startTime", but closest to it.
        // If there are no such items, does nothing.
        // Mostly (only) meant for use when a Notification's "Mark previous done" button is pressed.
        void completePreviousItem(const ClockTimeWrapper& startTime);
        void handleFileUnloaded();

    public:
        void init(IO_Handler& ioHandler, Schedule& schedule, NotificationHandler& notificationHandler);
        // Applies any resets that should have taken place since the read file's last schedule edit time.
        // Called by TimeHandler::fileReadListener.
        void applyResetsSinceEditTime(TimeWrapper lastEditTime);
        void timeTick();
};