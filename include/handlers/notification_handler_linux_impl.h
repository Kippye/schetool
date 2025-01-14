#pragma once

#include "time_wrapper.h"
#include "notification_handler_base_impl.h"

// Linux notification handler implementation.
// Uses libnotify to create notifications.
class NotificationHandlerLinuxImpl : public NotificationHandlerImpl
{        
    public:
        bool init() override;
        bool showElementNotification(const std::string& name, const ClockTimeWrapper& beginning, const ClockTimeWrapper& end) override;
};