#include "signal_listener.h"
#ifndef _WIN32
#include "signal_listener_linux_impl.h"
#else
#include "signal_listener_win_impl.h"
#endif

void SignalListener::init(Window& window)
{
    #ifdef _WIN32 // Windows-specific implementation
    m_implementation = std::make_shared<SignalListenerWinImpl>(window);
    if (m_implementation->init()) {}
    #else // Linux implementation (We don't do mac for now :/)
    m_implementation = std::make_shared<SignalHandlerLinuxImpl>(window);
    if (m_implementation->init()) {}
    #endif
}

Signal SignalListener::listenForSignals()
{
    if (!m_implementation)
    {
        printf("SignalHandler::listenForSignals(): There is no implementation. Returning Signal::None\n");
        return Signal::None;
    }
    if (m_implementation->getIsValid() == false)
    {
        printf("SignalHandler::listenForSignals(): Implementation is invalid. Returning Signal::None\n");
        return Signal::None;
    }
    m_lastSignal = m_implementation->getLastSignal();
    // printf("Last signal is: %zu\n", (size_t)m_lastSignal);
    return m_lastSignal;
}

// Check the current signal.
Signal SignalListener::getSignal()
{
    return m_lastSignal;
}

// Get the current signal and clear it (shortcut for getSignal() + clearSignal()).
Signal SignalListener::popSignal()
{
    Signal lastSignal = m_lastSignal;
    m_lastSignal = Signal::None;
    return lastSignal;
}

// Call at the end of the frame to clear the current signal.
void SignalListener::clearSignal()
{
    m_lastSignal = Signal::None;
}