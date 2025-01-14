#pragma once

#include "notification_handler_base_impl.h"

class NotificationHandler
{
    private:
        std::shared_ptr<NotificationHandlerImpl> m_implementation = nullptr;
    public:
        // Initialise the notification handler and its implementation.
        void init();
        
        // Show a notification about an element (usually about it starting)
        bool showElementNotification(const std::string& name, const ClockTimeWrapper& beginning, const ClockTimeWrapper& end);
};