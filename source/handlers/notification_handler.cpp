#include "notification_handler.h"
#ifndef _WIN32
#include "notification_handler_linux_impl.h"
#else
//#include "signal_handler_impl_win.h"
#endif

void NotificationHandler::init()
{
    #ifdef _WIN32 // Windows-specific implementation
    //m_implementation = std::make_shared<Notificat>(window);
    if (m_implementation->init()) {}
    #else // Linux implementation (We don't do mac for now :/)
    m_implementation = std::make_shared<NotificationHandlerLinuxImpl>();
    if (m_implementation->init()) {}
    #endif
}

bool NotificationHandler::showElementNotification(const std::string& name, const ClockTimeWrapper& beginning, const ClockTimeWrapper& end)
{
    if (!m_implementation) { return false; }
    if (m_implementation->getIsInitialised() == false){ return false; }
    
    return m_implementation->showElementNotification(name, beginning, end);
}