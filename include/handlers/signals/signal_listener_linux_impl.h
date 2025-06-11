#pragma once

#include "signals/signal_listener_impl_base.h"

// Signal handler implementation for Linux.
// Note that on Linux, closing the program and shutting down the OS both result in Signal::Close
class SignalListenerLinuxImpl : public SignalListenerImpl {
    public:
        SignalListenerLinuxImpl(Window& window) : SignalListenerImpl(window) {
        }
        // Set up the signal listener.
        bool init() override;
        // Get the last signal and clear it.
        Signal getLastSignal() const override;
};