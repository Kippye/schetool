#include "notification_handler.h"
#ifdef _WIN32
# ifndef __MINGW32__
# include "notification_handler_win_impl.h"
# endif
#else
#include "notification_handler_linux_impl.h"
#endif
#include <iostream>

void NotificationHandler::init()
{
    #ifdef _WIN32 // Windows-specific implementation
    # ifndef __MINGW32__ // does not work for MinGW yet
    m_implementation = std::make_shared<NotificationHandlerWinImpl>();
    if (m_implementation->init()) {}
    # endif
    #else // Linux implementation (We don't do mac for now :/)
    m_implementation = std::make_shared<NotificationHandlerLinuxImpl>();
    if (m_implementation->init()) {}
    #endif
    if (!m_implementation)
    {
        std::cout << "NotificationHandler::init(): No suitable implementations. Notifications will not be shown." << std::endl;
    }
}

bool NotificationHandler::showElementNotification(const std::string& name, const ClockTimeWrapper& beginning, const ClockTimeWrapper& end)
{
    if (!m_implementation) { return false; }
    if (m_implementation->getIsInitialised() == false){ return false; }
    
    return m_implementation->showElementNotification(name, beginning, end);
}