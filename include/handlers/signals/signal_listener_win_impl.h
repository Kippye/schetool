#pragma once

#include "signals/signal_listener_impl_base.h"

// Signal handler implementation for Windows.
// Uses a Console Control Handler to catch console signals.
// Uses a Subclass Procedure for other signals. This might cause performance issues since it's an extra step on the way to GLFW rendering.
class SignalListenerWinImpl : public SignalListenerImpl {
    public:
        SignalListenerWinImpl(Window& window) : SignalListenerImpl(window) {
        }
        // Set up the signal listener.
        bool init() override;
        // Get the last signal and clear it.
        Signal getLastSignal() const override;
};