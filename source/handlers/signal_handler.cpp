#include "signal_handler.h"
#ifdef WIN32
#include "signal_handler_impl_win.h"
#endif

void SignalHandler::init(Window& window)
{
    #ifdef WIN32 // Windows-specific implementation
    m_implementation = std::make_shared<SignalHandlerWinImpl>(window);
    if (m_implementation->init())
    {

    }
    #else // Linux implementation (We don't do mac for now :/)

    #endif
}

Signal SignalHandler::listenForSignals()
{
    m_lastSignal = m_implementation->getLastSignal();
    // printf("Last signal is: %zu\n", (size_t)m_lastSignal);
    return m_lastSignal;
}

// Check the current signal.
Signal SignalHandler::getSignal()
{
    return m_lastSignal;
}

// Get the current signal and clear it.
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