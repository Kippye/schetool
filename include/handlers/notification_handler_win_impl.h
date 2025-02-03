#pragma once
#include "DesktopNotificationManagerCompat.h"
#include <NotificationActivationCallback.h>

#include "time_wrapper.h"
#include "notification_handler_base_impl.h"
#include "event.h"
#include <string>
#include <iostream>
#include <optional>
#include <functional>

class DECLSPEC_UUID("79A832A4-47BC-46CD-998A-73DCD7CAF255") NotificationActivator WrlSealed // WrlFinal
    : public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>, INotificationActivationCallback>
{
public:
    // Invoked when a notification is activated.
    // Passes the NotificationActivation type and the notification's ID.
    inline static Event<NotificationActivation, size_t> notificationActivatedEvent = Event<NotificationActivation, size_t>();
    virtual HRESULT STDMETHODCALLTYPE Activate(
        _In_ LPCWSTR appUserModelId,
        _In_ LPCWSTR invokedArgs,
        _In_reads_(dataCount) const NOTIFICATION_USER_INPUT_DATA* data,
        ULONG dataCount) override
    {
        std::wstring argString(invokedArgs);
        std::wcout << L"NotificationActivator::Activate(): Arguments: " << argString << std::endl;
        std::vector<std::wstring> arguments;
        std::wstringstream wss(argString);
        std::wstring temp;
        while(std::getline(wss, temp, L'&'))
            arguments.push_back(temp);
            std::wcout << L"NotificationActivator::Activate(): Split argument: " << temp << std::endl;
        
        HRESULT hr = S_OK;
        // Clicked the notification
        if (argString.find(L"action=viewItem") != std::string::npos)
        {
            std::cout << "View item activated." << std::endl;
            hr = S_OK;
        }
        else
        {
            // TODO: Check if window is open
            hr = S_OK;
            if (SUCCEEDED(hr))
            {
                // Clicked the button "Mark previous done" or something like that
                if (argString.find(L"completePrevious") != std::string::npos)
                {
                    std::cout << "Complete previous item activated" << std::endl;
                    hr = S_OK;
                    if (arguments.size() == 2)
                    {
                        // Convert to NORMAL string
                        std::string notificationString;
                        for(char x : arguments.back())
                            notificationString += x;
                        int notificationID;
                        auto [ptr, ec] = std::from_chars(notificationString.data(), notificationString.data()+notificationString.size(), notificationID);
                        if (ec != std::errc{})
                        {
                            // TODO: something
                        }
                        else
                        {
                            std::wcout << "Notification ID: " << notificationID << std::endl;
                            notificationActivatedEvent.invoke(NotificationActivation::PreviousMarkedDone, (size_t)notificationID);
                        }
                    }
                }
                // Clicked "Dismiss"
                else if (argString.find(L"dismiss") != std::string::npos)
                {
                    // do nothing?
                    hr = S_OK;
                    std::cout << "Dismissed notification." << std::endl;
                }
                // Open the app
                else
                {
                    // Nothing to do 
                }
            }
        }

        if (FAILED(hr))
        {
            std::cout << "Notification activation failed: " << hr << std::endl;
        }

        return S_OK;
    }

    ~NotificationActivator()
    {
        // If window isn't open
        // TODO
        // Background activation
        // exit(0);
    }
};

// Flag class as COM creatable
CoCreatableClass(NotificationActivator);

// Windows notification handler implementation.
// Uses notificationcompat library (from Microsoft Learn) to create notifications through WRL.
class NotificationHandlerWinImpl : public NotificationHandlerImpl
{
    private:
        inline static const std::vector<std::string> m_scheduleProgressMessages =
        {
            "Awesome!",
            "Good job :)",
            "Working...",
            "Just a few left!",
            "Do you like ice cream?"
        };
        static constexpr const char* const m_notificationFormat = 
        "<toast>"
            "<visual>"
                "<binding template='ToastGeneric'>"
                    "<text hint-maxLines='1'>{}</text>"
                    "<text>{}</text>"
                "</binding>"
            "</visual>"
        "</toast>";
        static constexpr const char* const m_itemNotificationFormat = 
        "<toast launch='action=viewItem&amp;itemName={}' duration='{}'>"
            "<visual>"
                "<binding template='ToastGeneric'>"
                    "<text hint-maxLines='1'>'{}' is beginning.</text>"
                    "<text>{} - {}</text>"
                    "<progress title='Schedule progress' value='{}' valueStringOverride='{}/{} items' status='{}'/>"
                "</binding>"
            "</visual>"
            "<actions>"
                "<action content='Mark previous done' arguments='completePrevious&amp;{}' activationType='foreground'/>"
                "<action activationType='system' arguments='dismiss' content=''/>"
            "</actions>"
        "</toast>";

        size_t m_notificationID = 0;
        std::vector<NotificationInfo> m_notificationHistory = {};

        std::function<void(NotificationActivation, size_t)> notificationActivatedListener = [&](NotificationActivation activationType, size_t notificationID)
        {
            if (notificationID < m_notificationHistory.size())
            {
                notificationActivatedEvent.invoke(activationType, m_notificationHistory[notificationID]);
            }
        };
        // Show a notification using a prefilled XML string
        bool showNotificationWithXmlString(const std::wstring& xml);
    public:
        bool init() override;
        bool showNotification(const std::string& title, const std::string& content, unsigned int timeout_sec) override;
        bool showItemNotification(const std::string& name, const ClockTimeWrapper& beginning, const ClockTimeWrapper& end, const ItemNotificationData& notificationData) override;
};