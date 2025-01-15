#ifndef _WIN32
#include "notification_handler_linux_impl.h"
#include <libnotify/notify.h>

#include <format>
#include <iostream>

constexpr int ELEMENT_NOTIFICATION_TIMEOUT_MS = 10 * 1000;

bool NotificationHandlerLinuxImpl::init()
{
    if (notify_init("schetool") == TRUE)
    {
        printf("Initialised Linux notification handler.\n");
        m_initialised = true;
    }
    else
    {
        printf("NotificationHandlerLinuxImpl::init(): Failed to initialise notification handler. Notifications will not be shown.\n");
        m_initialised = false;
    }
    return m_initialised;
}

bool NotificationHandlerLinuxImpl::showElementNotification(const std::string& name, const ClockTimeWrapper& beginning, const ClockTimeWrapper& end)
{
    if (getIsInitialised() == false) { return false; }

    NotifyNotification* notif = notify_notification_new
    (
        std::format("'{}' is beginning.", name).c_str(),
        std::format("{} - {}", TimeWrapper(beginning).getStringUTC(TIME_FORMAT_TIME).c_str(), TimeWrapper(end).getStringUTC(TIME_FORMAT_TIME)).c_str(),
        "appointment-new"
    );
    notify_notification_set_timeout(notif, ELEMENT_NOTIFICATION_TIMEOUT_MS);
    if (notify_notification_show(notif, NULL))
    {
        return true;
    }
    return false;
}
#endif