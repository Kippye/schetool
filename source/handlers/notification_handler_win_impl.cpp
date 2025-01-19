#if defined(_WIN32) && !defined(__MINGW32__)

#include "notification_handler_win_impl.h"

#include <NotificationActivationCallback.h>

#include <format>
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>

using namespace ABI::Windows::Data::Xml::Dom;
using namespace ABI::Windows::UI::Notifications;
using namespace Microsoft::WRL;

NotificationInfo::NotificationInfo(size_t ID, std::optional<std::string> itemName, std::optional<ClockTimeWrapper> startTime, std::optional<ClockTimeWrapper> endTime)
{
    this->ID = ID;
    this->itemName = itemName;
    this->startTime = startTime;
    this->endTime = endTime;
}

bool NotificationHandlerWinImpl::init()
{
    // Register AUMID and COM server (for a packaged app, this is a no-operation)
    HRESULT hr = DesktopNotificationManagerCompat::RegisterAumidAndComServer(L"Kip.schetool", __uuidof(NotificationActivator));
    if (SUCCEEDED(hr))
    {
        // Register activator type
        hr = DesktopNotificationManagerCompat::RegisterActivator();
        if (SUCCEEDED(hr))
        {
            printf("Initialised Windows notification handler.\n");
            m_initialised = true;
        }
    }
    if (SUCCEEDED(hr) == false)
    {
        printf("NotificationHandlerWinImpl::init(): Failed to initialise notification handler. Notifications will not be shown.\n");
        m_initialised = false;
    }
    if (m_initialised)
    {
        srand(time(0));
    }
    return m_initialised;
}

bool NotificationHandlerWinImpl::showNotificationWithXmlString(const std::wstring& xmlString)
{
    if (getIsInitialised() == false) { return false; }

    // Construct XML
    ComPtr<IXmlDocument> doc;
    HRESULT hr = DesktopNotificationManagerCompat::CreateXmlDocumentFromString(
        xmlString.c_str(),
        &doc
    );
    if (SUCCEEDED(hr))
    {
        // Create the notifier
        // Desktop apps must use the compat method to create the notifier.
        ComPtr<IToastNotifier> notifier;
        hr = DesktopNotificationManagerCompat::CreateToastNotifier(&notifier);
        if (SUCCEEDED(hr))
        {
            // Create the notification itself (using helper method from compat library)
            ComPtr<IToastNotification> toast;
            hr = DesktopNotificationManagerCompat::CreateToastNotification(doc.Get(), &toast);
            if (SUCCEEDED(hr))
            {
                // And show it!
                hr = notifier->Show(toast.Get());
                if (SUCCEEDED(hr))
                {
                    return true;
                }
            }
        }
    }

    return false;
}

bool NotificationHandlerWinImpl::showNotification(const std::string& title, const std::string& content, unsigned int timeout_sec)
{
    if (getIsInitialised() == false) { return false; }
    std::string xmlText = std::format(m_notificationFormat, title, content);
    std::wstring wxmlText(xmlText.begin(), xmlText.end());
    
    if (showNotificationWithXmlString(wxmlText))
    {
        m_notificationHistory.emplace_back(m_notificationID);
        m_notificationID++;
        return true;
    }
    return false;
}

bool NotificationHandlerWinImpl::showItemNotification(const std::string& name, const ClockTimeWrapper& beginning, const ClockTimeWrapper& end)
{
    // Choose a random message
    size_t messageIndex = rand() % m_scheduleProgressMessages.size();
    std::string supportiveMessage = messageIndex >= m_scheduleProgressMessages.size() ? "" : m_scheduleProgressMessages[messageIndex];

    if (getIsInitialised() == false) { return false; }
    std::string xmlText = std::format(
        m_elementNotificationFormat,        // Notification format XML
        name,                               // Name of the element for launch (not supported)
        ITEM_NOTIFICATION_TIMEOUT_SEC,      // How long the notification is shown for
        name,                               // Item name in notification
        TimeWrapper(beginning).getStringUTC(TIME_FORMAT_TIME).c_str(), TimeWrapper(end).getStringUTC(TIME_FORMAT_TIME),
        supportiveMessage,                  // Supportive message for schedule progress
        m_notificationID
    );
    std::wstring wxmlText(xmlText.begin(), xmlText.end());
    if (showNotificationWithXmlString(wxmlText))
    {
        m_notificationHistory.emplace_back(m_notificationID, name, beginning, end);
        m_notificationID++;
        return true;
    }
    return false;
}

// NotificationHandlerWinImpl::m_notificationHistory
#endif