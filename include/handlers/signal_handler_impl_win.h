#pragma once

#include "signal_handler_impl_base.h"

class SignalHandlerWinImpl : public SignalHandlerImpl
{
    public:
        SignalHandlerWinImpl(Window& window) : SignalHandlerImpl(window)
        {}
        // Set up the signal listener.
        bool init() override;
        // Get the last signal and clear it.
        Signal getLastSignal() const override;
};