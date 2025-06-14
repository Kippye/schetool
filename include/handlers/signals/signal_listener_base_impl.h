#pragma once

#include "signal_constants.h"
#include "window.h"

class SignalListenerImpl {
    protected:
        Window& m_window;
        bool m_haveValidListener = false;

    public:
        SignalListenerImpl(Window& window) : m_window(window) {
        }
        // Set up the signal listener.
        virtual bool init() {
            return false;
        };
        // Get the last signal and clear it.
        virtual Signal getLastSignal() const {
            return Signal::None;
        };
        // Check if this signal handler implementation is valid.
        bool getIsValid() const {
            return m_haveValidListener;
        }
};