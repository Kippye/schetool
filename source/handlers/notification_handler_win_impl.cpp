#if defined(_WIN32) && !defined(__MINGW32__)

#include "notification_handler_win_impl.h"

#include <NotificationActivationCallback.h>
#include <windows.ui.notifications.h>

#include <format>
#include <iostream>
#include <string>

using namespace ABI::Windows::Data::Xml::Dom;
using namespace ABI::Windows::UI::Notifications;
using namespace Microsoft::WRL;

constexpr int ELEMENT_NOTIFICATION_TIMEOUT_MS = 10 * 1000;

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
    return m_initialised;
}

bool NotificationHandlerWinImpl::showElementNotification(const std::string& name, const ClockTimeWrapper& beginning, const ClockTimeWrapper& end)
{
    if (getIsInitialised() == false) { return false; }
    std::string xmlText = std::format(m_elementNotificationFormat, name, TimeWrapper(beginning).getStringUTC(TIME_FORMAT_TIME).c_str(), TimeWrapper(end).getStringUTC(TIME_FORMAT_TIME));
    std::wstring wxmlText(xmlText.begin(), xmlText.end());
    // Construct XML
    ComPtr<IXmlDocument> doc;
    HRESULT hr = DesktopNotificationManagerCompat::CreateXmlDocumentFromString(
        wxmlText.c_str(),
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
#endif