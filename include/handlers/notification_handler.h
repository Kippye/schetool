#pragma once

#include "notification_handler_base_impl.h"
#include "preferences_io.h"
#include "preferences.h"
#include <memory>

class NotificationHandler {
    private:
        bool m_notificationsEnabled = Preferences::getDefault().getNotificationsEnabled();
        std::shared_ptr<NotificationHandlerImpl> m_implementation = nullptr;

        std::function<void(Preferences)> preferencesChangedListener = [&](Preferences preferences) {
            m_notificationsEnabled = preferences.getNotificationsEnabled();
        };

        std::function<void(NotificationActivation, NotificationInfo)> notificationActivatedListener =
            [&](NotificationActivation activationType, NotificationInfo notificationInfo) {
                notificationActivatedEvent.invoke(activationType, notificationInfo);
            };

    public:
        Event<NotificationActivation, NotificationInfo> notificationActivatedEvent;

        // Initialise the notification handler and its implementation.
        void init();
        void initEventListeners(std::shared_ptr<PreferencesIO> preferencesIO);

        // Show a notification about anything for the given length of time.
        // If no timeout length is given, the default timeout is used.
        // NOTE: The Windows implementation doesn't support a custom timeout length. Instead, the time that notifications are displayed for can be changed in Windows accessbility settings.
        bool showNotification(const std::string& title,
                              const std::string& content,
                              unsigned int timeout_sec = DEFAULT_NOTIFICATION_TIMEOUT_SEC);
        // Show a notification about an element (usually about it starting)
        bool showItemNotification(const std::string& name,
                                  const ClockTimeWrapper& beginning,
                                  const ClockTimeWrapper& end,
                                  const ItemNotificationData& itemNotificationData);
};