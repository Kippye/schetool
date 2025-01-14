#pragma once

#include "time_wrapper.h"
#include <string>

// Base for notification handler implementations. You shouldn't actually make instances of this.
class NotificationHandlerImpl
{
    protected:
        bool m_initialised = false;
    public:
        virtual bool init() { return false; };
        virtual bool showElementNotification(const std::string& name, const ClockTimeWrapper& beginning, const ClockTimeWrapper& end) { return false; };
        bool getIsInitialised() const
        {
            return m_initialised;
        }
};