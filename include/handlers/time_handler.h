#pragma once

#include "io_handler.h"
#include "schedule.h"
#include "time_wrapper.h"
#include "notifications/notification_handler.h"
#include <functional>
#include <utility>

class TimeHandler {
    private:
        TimeWrapper m_lastTickTime = TimeWrapper();
        Schedule* m_schedule;
        NotificationHandler* m_notificationHandler;

        std::function<void(FileInfo)> fileOpenListener = [&](FileInfo fileInfo) {
            applyResetsSinceEditTime(fileInfo.getScheduleEditTime().value());
        };
        std::function<void()> fileUnloadListener = [&]() { handleFileUnloaded(); };

        std::function<void(NotificationActivation, NotificationInfo)> notificationActivatedListener =
            [&](NotificationActivation activationType, NotificationInfo notificationInfo) {
                switch (activationType) {
                    case NotificationActivation::MarkedDone:
                        completeItem(notificationInfo);
                        break;
                    case NotificationActivation::PreviousMarkedDone:
                        completePreviousItem(notificationInfo);
                        break;
                    default:
                        break;
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
        // Completes ALL items with the same start time AND end time AND name as in the provided NotificationInfo.
        // If there are no such items or the NotificationInfo is missing some values, does nothing.
        // Meant for use when a Notification's "Mark done" button is pressed.
        void completeItem(const NotificationInfo& notificationInfo);
        // Basically, completes the item whose start time is before "startTime", but closest to it.
        // If there are no such items, does nothing.
        // Meant for use when a Notification's "Mark previous done" button is pressed.
        void completePreviousItem(const NotificationInfo& notificationInfo);
        void handleFileUnloaded();

    public:
        void init(IO_Handler& ioHandler, Schedule& schedule, NotificationHandler& notificationHandler);
        // Applies any resets that should have taken place since the read file's last schedule edit time.
        // Called by TimeHandler::fileReadListener.
        void applyResetsSinceEditTime(TimeWrapper lastEditTime);
        void timeTick();
};