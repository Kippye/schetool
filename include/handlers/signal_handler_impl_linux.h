#pragma once

#include "signal_handler_impl_base.h"

// Signal handler implementation for Linux.
// Note that on Linux, closing the program and shutting down the OS both result in Signal::Close
class SignalHandlerLinuxImpl : public SignalHandlerImpl
{
    public:
        SignalHandlerLinuxImpl(Window& window) : SignalHandlerImpl(window)
        {}
        // Set up the signal listener.
        bool init() override;
        // Get the last signal and clear it.
        Signal getLastSignal() const override;
};