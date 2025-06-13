#pragma once

#include "time_wrapper.h"
#include "notifications/notification_handler_base_impl.h"
#include <functional>
#include <libnotify/notify.h>

// Linux notification handler implementation.
// Uses libnotify to create notifications.
class NotificationHandlerLinuxImpl : public NotificationHandlerImpl {
    private:
        size_t m_notificationID = 0;

        std::function<void(NotifyNotification*, char*, gpointer)> notificationActionCallback =
            [&](NotifyNotification* notification, char* action, gpointer userData) {
                if (action == std::string("default")) {
                    notificationActivatedEvent.invoke(NotificationActivation::Clicked, *(NotificationInfo*)userData);
                } else if (action == std::string("markPreviousDone")) {
                    notificationActivatedEvent.invoke(NotificationActivation::PreviousMarkedDone, *(NotificationInfo*)userData);
                } else if (action == std::string("dismiss")) {
                    notificationActivatedEvent.invoke(NotificationActivation::Dismissed, *(NotificationInfo*)userData);
                    // Close the notification.
                    // We don't so much care if it ACTUALLY closed so we don't handle errors or check the return value.
                    notify_notification_close(notification, NULL);
                }
            };

        void addNotificationAction(NotifyNotification* notification,
                                   const char* action,
                                   const char* label,
                                   NotificationInfo* notificationInfo);

    public:
        bool init() override;
        bool showNotification(const std::string& title, const std::string& content, unsigned int timeout_sec) override;
        bool showItemNotification(const std::string& name,
                                  const ClockTimeWrapper& beginning,
                                  const ClockTimeWrapper& end,
                                  const ItemNotificationData& notificationData) override;
};