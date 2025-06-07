#ifdef SCHETOOL_LINUX
#include "notifications/notification_handler_linux_impl.h"

#include <format>

std::function<void(NotifyNotification*, char*, gpointer)> notifyActionCallback = nullptr;

// Free user data after it isn't needed by a notification anymore.
// NOTE: The data gets cast to a NotificationInfo pointer so userdata always has to be of that type!
void notifyFreeUserDataCallback(gpointer data) {
    delete (NotificationInfo*)data;
}

void notifyActionCallbackWrapper(NotifyNotification* notification, char* action, gpointer userData) {
    if (notifyActionCallback) {
        notifyActionCallback(notification, action, userData);
    }
}

bool NotificationHandlerLinuxImpl::init() {
    if (notify_init("schetool") == TRUE) {
        notifyActionCallback = notificationActionCallback;
        m_initialised = true;
    } else {
        m_initialised = false;
    }
    return m_initialised;
}

bool NotificationHandlerLinuxImpl::showNotification(const std::string& title,
                                                    const std::string& content,
                                                    unsigned int timeout_sec) {
    if (getIsInitialised() == false) {
        return false;
    }

    NotifyNotification* notif = notify_notification_new(title.c_str(),
                                                        content.c_str(),
                                                        NULL  // TODO: Support providing icon?
    );
    notify_notification_set_timeout(notif, timeout_sec * 1000);
    if (notify_notification_show(notif, NULL)) {
        m_notificationID++;
        return true;
    }
    return false;
}

bool NotificationHandlerLinuxImpl::showItemNotification(const std::string& name,
                                                        const ClockTimeWrapper& beginning,
                                                        const ClockTimeWrapper& end,
                                                        const ItemNotificationData& notificationData) {
    if (getIsInitialised() == false) {
        return false;
    }

    NotifyNotification* notif =
        notify_notification_new(std::format("'{}' is beginning.", name).c_str(),
                                std::format("{} - {}\n{}/{} items",
                                            TimeWrapper(beginning).getStringUTC(TIME_FORMAT_TIME).c_str(),
                                            TimeWrapper(end).getStringUTC(TIME_FORMAT_TIME).c_str(),
                                            notificationData.completedItemCount,
                                            notificationData.totalItemCount)
                                    .c_str(),
                                "appointment-new");
    notify_notification_set_timeout(notif, ITEM_NOTIFICATION_TIMEOUT_SEC * 1000);
    // Callback for clicking on the notification itself
    notify_notification_add_action(notif,
                                   "default",
                                   "Clicked on",
                                   notifyActionCallbackWrapper,
                                   new NotificationInfo(m_notificationID, name, beginning, end),
                                   notifyFreeUserDataCallback);
    // Mark previous done button
    notify_notification_add_action(notif,
                                   "markPreviousDone",
                                   "Mark previous done",
                                   notifyActionCallbackWrapper,
                                   new NotificationInfo(m_notificationID, name, beginning, end),
                                   notifyFreeUserDataCallback);
    // Dismiss button
    notify_notification_add_action(notif,
                                   "dismiss",
                                   "Dismiss",
                                   notifyActionCallbackWrapper,
                                   new NotificationInfo(m_notificationID, name, beginning, end),
                                   notifyFreeUserDataCallback);
    if (notify_notification_show(notif, NULL)) {
        m_notificationID++;
        return true;
    }
    return false;
}
#endif