#pragma once

#include "time_wrapper.h"
#include <string>
#include "event.h"

constexpr int ITEM_NOTIFICATION_TIMEOUT_SEC = 10;
constexpr int DEFAULT_NOTIFICATION_TIMEOUT_SEC = 10;

enum class NotificationActivation
{
    Clicked,
    PreviousMarkedDone,
    Dismissed
};

struct NotificationInfo
{
    size_t ID;
    std::optional<std::string> itemName = std::nullopt;
    std::optional<ClockTimeWrapper> startTime = std::nullopt;
    std::optional<ClockTimeWrapper> endTime = std::nullopt;

    NotificationInfo(size_t ID, std::optional<std::string> itemName = std::nullopt, std::optional<ClockTimeWrapper> startTime = std::nullopt, std::optional<ClockTimeWrapper> endTime = std::nullopt)
    {
        this->ID = ID;
        this->itemName = itemName;
        this->startTime = startTime;
        this->endTime = endTime;
    }
};

// Struct passed as input to showItemNotification.
// Contains extra data to show useful information.
struct ItemNotificationData
{
    size_t completedItemCount = 0;
    size_t totalItemCount = 0;
};

// Base for notification handler implementations. You shouldn't actually make instances of this.
class NotificationHandlerImpl
{
    protected:
        bool m_initialised = false;
    public:
        Event<NotificationActivation, NotificationInfo> notificationActivatedEvent;

        virtual bool init() { return false; };
        virtual bool showNotification(const std::string& title, const std::string& content, unsigned int timeout_sec) { return false; }
        virtual bool showItemNotification(const std::string& name, const ClockTimeWrapper& beginning, const ClockTimeWrapper& end, const ItemNotificationData& itemNotificationData) { return false; };
        bool getIsInitialised() const
        {
            return m_initialised;
        }
};