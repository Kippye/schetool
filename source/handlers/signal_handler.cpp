#include "signal_handler.h"
#ifndef _WIN32
#include "signal_handler_linux_impl.h"
#else
#include "signal_handler_win_impl.h"
#endif

void SignalHandler::init(Window& window)
{
    #ifdef _WIN32 // Windows-specific implementation
    m_implementation = std::make_shared<SignalHandlerWinImpl>(window);
    if (m_implementation->init()) {}
    #else // Linux implementation (We don't do mac for now :/)
    m_implementation = std::make_shared<SignalHandlerLinuxImpl>(window);
    if (m_implementation->init()) {}
    #endif
}

Signal SignalHandler::listenForSignals()
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
Signal SignalHandler::getSignal()
{
    return m_lastSignal;
}

// Get the current signal and clear it (shortcut for getSignal() + clearSignal()).
Signal SignalHandler::popSignal()
{
    Signal lastSignal = m_lastSignal;
    m_lastSignal = Signal::None;
    return lastSignal;
}

// Call at the end of the frame to clear the current signal.
void SignalHandler::clearSignal()
{
    m_lastSignal = Signal::None;
}