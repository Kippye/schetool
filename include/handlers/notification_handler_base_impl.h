#pragma once

#include "time_wrapper.h"
#include <string>
#include "event.h"

constexpr int ITEM_NOTIFICATION_TIMEOUT_SEC = 10;
constexpr int DEFAULT_NOTIFICATION_TIMEOUT_SEC = 10;

// Base for notification handler implementations. You shouldn't actually make instances of this.
class NotificationHandlerImpl
{
    protected:
        bool m_initialised = false;
    public:
        virtual bool init() { return false; };
        virtual bool showNotification(const std::string& title, const std::string& content, unsigned int timeout_sec) { return false; }
        virtual bool showItemNotification(const std::string& name, const ClockTimeWrapper& beginning, const ClockTimeWrapper& end) { return false; };
        bool getIsInitialised() const
        {
            return m_initialised;
        }
};