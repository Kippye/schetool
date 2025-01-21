#ifdef SCHETOOL_LINUX
#include "notification_handler_linux_impl.h"
#include <libnotify/notify.h>

#include <format>
#include <iostream>

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

bool NotificationHandlerLinuxImpl::showNotification(const std::string& title, const std::string& content, unsigned int timeout_sec)
{
    if (getIsInitialised() == false) { return false; }

    NotifyNotification* notif = notify_notification_new
    (
        title.c_str(),
        content.c_str(),
        NULL // TODO: Support providing icon?
    );
    notify_notification_set_timeout(notif, timeout_sec * 1000);
    if (notify_notification_show(notif, NULL))
    {
        return true;
    }
    return false;
}

bool NotificationHandlerLinuxImpl::showItemNotification(const std::string& name, const ClockTimeWrapper& beginning, const ClockTimeWrapper& end, const ItemNotificationData& notificationData)
{
    if (getIsInitialised() == false) { return false; }

    NotifyNotification* notif = notify_notification_new
    (
        std::format("'{}' is beginning.", name).c_str(),
        std::format("{} - {}", TimeWrapper(beginning).getStringUTC(TIME_FORMAT_TIME).c_str(), TimeWrapper(end).getStringUTC(TIME_FORMAT_TIME)).c_str(),
        "appointment-new"
    );
    notify_notification_set_timeout(notif, ITEM_NOTIFICATION_TIMEOUT_SEC * 1000);
    if (notify_notification_show(notif, NULL))
    {
        return true;
    }
    return false;
}
#endif