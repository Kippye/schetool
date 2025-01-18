#pragma once
#include "DesktopNotificationManagerCompat.h"
#include <NotificationActivationCallback.h>

#include "time_wrapper.h"
#include "notification_handler_base_impl.h"
#include <string>

class DECLSPEC_UUID("79A832A4-47BC-46CD-998A-73DCD7CAF255") NotificationActivator WrlSealed // WrlFinal
    : public Microsoft::WRL::RuntimeClass<Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>, INotificationActivationCallback>
{
public:
    virtual HRESULT STDMETHODCALLTYPE Activate(
        _In_ LPCWSTR appUserModelId,
        _In_ LPCWSTR invokedArgs,
        _In_reads_(dataCount) const NOTIFICATION_USER_INPUT_DATA* data,
        ULONG dataCount) override
    {
        // TODO: Handle activation
        return 0;
    }
};

// Flag class as COM creatable
CoCreatableClass(NotificationActivator);

// Windows notification handler implementation.
// Uses notificationcompat library (from Microsoft Learn) to create notifications through WRL.
class NotificationHandlerWinImpl : public NotificationHandlerImpl
{        
    private:
        static constexpr const char* const m_notificationFormat = 
        "<toast>"
            "<visual>"
                "<binding template='ToastGeneric'>"
                    "<text hint-maxLines='1'>{}</text>"
                    "<text>{}</text>"
                "</binding>"
            "</visual>"
        "</toast>";
        static constexpr const char* const m_elementNotificationFormat = 
        "<toast>"
            "<visual>"
                "<binding template='ToastGeneric'>"
                    "<text hint-maxLines='1'>'{}' is beginning.</text>"
                    "<text>{} - {}</text>"
                "</binding>"
            "</visual>"
        "</toast>";

        // Show a notification using a prefilled XML string
        bool showNotificationWithXmlString(const std::wstring& xml);
    public:
        bool init() override;
        bool showNotification(const std::string& title, const std::string& content, unsigned int timeout_sec) override;
        bool showElementNotification(const std::string& name, const ClockTimeWrapper& beginning, const ClockTimeWrapper& end) override;
};