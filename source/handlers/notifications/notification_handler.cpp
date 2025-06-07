#include "notifications/notification_handler.h"
#ifdef _WIN32
#ifndef __MINGW32__
#include "notifications/notification_handler_win_impl.h"
#endif
#elif defined(SCHETOOL_LINUX)
#include "notifications/notification_handler_linux_impl.h"
#endif
#include <iostream>

void NotificationHandler::init() {
#ifdef _WIN32  // Windows-specific implementation
#ifndef __MINGW32__  // does not work for MinGW yet
    m_implementation = std::make_shared<NotificationHandlerWinImpl>();
    if (m_implementation->init()) {
        std::cout << "Initialised Windows notification handler." << std::endl;
    } else {
        std::cout << "Failed to initialise Windows notification handler." << std::endl;
    }
#endif
#elif defined(SCHETOOL_LINUX)  // Linux implementation
    m_implementation = std::make_shared<NotificationHandlerLinuxImpl>();
    if (m_implementation->init()) {
        std::cout << "Initialised Linux notification handler." << std::endl;
    } else {
        std::cout << "Failed to initialise Linux notification handler." << std::endl;
    }
#endif
    if (m_implementation) {
        m_implementation->notificationActivatedEvent.addListener(notificationActivatedListener);
    } else {
        std::cout << "NotificationHandler::init(): No suitable implementations. Notifications will not be shown." << std::endl;
    }
}

void NotificationHandler::initEventListeners(std::shared_ptr<PreferencesIO> preferencesIO) {
    if (preferencesIO) {
        preferencesIO->preferencesChangedEvent.addListener(preferencesChangedListener);
    }
}

bool NotificationHandler::showNotification(const std::string& title, const std::string& content, unsigned int timeout_sec) {
    if (!m_implementation) {
        return false;
    }
    if (m_implementation->getIsInitialised() == false) {
        return false;
    }
    if (!m_notificationsEnabled) {
        return false;
    }

    return m_implementation->showNotification(title, content, timeout_sec);
}

bool NotificationHandler::showItemNotification(const std::string& name,
                                               const ClockTimeWrapper& beginning,
                                               const ClockTimeWrapper& end,
                                               const ItemNotificationData& itemNotificationData) {
    if (!m_implementation) {
        return false;
    }
    if (m_implementation->getIsInitialised() == false) {
        return false;
    }
    if (!m_notificationsEnabled) {
        return false;
    }

    return m_implementation->showItemNotification(name, beginning, end, itemNotificationData);
}