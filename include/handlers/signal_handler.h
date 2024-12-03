#pragma once

#include "event.h"
#include "signal_constants.h"
#include "signal_handler_impl_base.h"
#include "window.h"
#include <memory>

class SignalHandler
{
    private:
        std::shared_ptr<SignalHandlerImpl> m_implementation;
    protected:
        Signal m_lastSignal = Signal::None;
    public:
        // Initialise the signal handler and its implementation.
        void init(Window& window);
        
        // Listen for signals, return the signal if there is one.
        Signal listenForSignals();
    private:
        // (NOT IMPLEMENTED) Listen to get an update every time a signal is received.
        Event<Signal> signalEvent;
    public:
        // Check the current signal.
        Signal getSignal();
        // Get the current signal and clear it.
        Signal popSignal();
        // Call at the end of the frame to clear the current signal.
        void clearSignal();
};