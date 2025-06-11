#ifdef _WIN32
#include "signals/signal_listener_win_impl.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#include <CommCtrl.h>

#include <stdio.h>
#include <iostream>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include "time_wrapper.h"

Signal gLastSignal = Signal::None;
WNDPROC gGlfwWndProc;
// UINT_PTR gSubClassID = 420;

BOOL WINAPI CtrlHandler(DWORD fdwCtrlType) {
    switch (fdwCtrlType) {
        // Handle the CTRL-C signal.
        case CTRL_C_EVENT:
            gLastSignal = Signal::Interrupt;
            return TRUE;

        // CTRL-CLOSE: confirm that the user wants to exit.
        case CTRL_CLOSE_EVENT:
            gLastSignal = Signal::Close;
            return TRUE;

        // Pass other signals to the next handler.
        case CTRL_BREAK_EVENT:
            return TRUE;

        case CTRL_LOGOFF_EVENT:
            gLastSignal = Signal::Logoff;
            return TRUE;

        case CTRL_SHUTDOWN_EVENT:
            gLastSignal = Signal::Shutdown;
            return TRUE;

        default:
            return FALSE;
    }
}

LRESULT SubWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam
                   // UINT_PTR uIdSubclass,
                   // DWORD_PTR dwRefData
) {
    switch (uMsg) {
        case WM_PAINT:  // Don't even print this spam
            break;
        case WM_CLOSE:
            gLastSignal = Signal::Close;
            printf("SubWndProc(): WM_CLOSE\n");
            return 1;
            break;

        case WM_QUERYENDSESSION: {
            // Computer is shutting down
            if (lParam == 0) {
                gLastSignal = Signal::Shutdown;
                auto time = TimeWrapper(std::chrono::system_clock::now()).getLocalTime();
                auto floorMinutes = std::chrono::floor<std::chrono::minutes>(time);
                auto secs = std::chrono::seconds(std::chrono::floor<std::chrono::seconds>(time - floorMinutes));
                auto floorSecs = std::chrono::floor<std::chrono::seconds>(time);
                auto millis = std::chrono::milliseconds(std::chrono::floor<std::chrono::milliseconds>(time - floorSecs));
                std::string timeString =
                    std::format("{}:{}:{}ms", TimeWrapper::getString(time, TIME_FORMAT_TIME), secs.count(), millis.count());
                printf("SubWndProc(): Returning Shutdown at %s (WM_QUERYENDSESSION)\n", timeString.c_str());
                return 1;
            }
            // User is logging off
            if ((lParam & ENDSESSION_LOGOFF) == ENDSESSION_LOGOFF) {
                gLastSignal = Signal::Logoff;
                printf("SubWndProc(): Returning Logoff (WM_QUERYENDSESSION)\n");
            }
            break;
        }

        case WM_ENDSESSION: {
            // Set Signal::Shutdown NOW so the program has time to handle things before the actual shutdown
            gLastSignal = Signal::Shutdown;
            auto time = TimeWrapper(std::chrono::system_clock::now()).getLocalTime();
            auto floorMinutes = std::chrono::floor<std::chrono::minutes>(time);
            auto secs = std::chrono::seconds(std::chrono::floor<std::chrono::seconds>(time - floorMinutes));
            auto floorSecs = std::chrono::floor<std::chrono::seconds>(time);
            auto millis = std::chrono::milliseconds(std::chrono::floor<std::chrono::milliseconds>(time - floorSecs));
            std::string timeString =
                std::format("{}:{}:{}ms", TimeWrapper::getString(time, TIME_FORMAT_TIME), secs.count(), millis.count());
            printf("SubWndProc(): Got Endsession (shutdown in 5 seconds!) at %s after WM_QUERYENDSESSION (WM_ENDSESSION)\n",
                   timeString.c_str());
        }

        case WM_QUIT:
            gLastSignal = Signal::Close;
            break;

        default:
            // printf("SubWndProc(): Other signal with value: %d\n", uMsg);
            break;
    }

    return CallWindowProc(gGlfwWndProc, hWnd, uMsg, wParam, lParam);
    // return 0;DefWindowProcA(hWnd, uMsg, wParam, lParam);
}

bool SignalListenerWinImpl::init() {
    gGlfwWndProc = (WNDPROC)GetWindowLongPtr(glfwGetWin32Window(m_window.window), GWLP_WNDPROC);
    SetWindowLongPtr(glfwGetWin32Window(m_window.window), GWLP_WNDPROC, (LONG_PTR)SubWndProc);
    // if (SetWindowSubclass(glfwGetWin32Window(m_window.window), Subclassproc, gSubClassID, (DWORD_PTR)nullptr))
    // {
    //     // printf("Window subclass successfully created!\n");
    // }

    if (SetConsoleCtrlHandler(CtrlHandler, TRUE)) {
        m_haveValidListener = true;
    } else {
        printf("\nERROR: SignalListenerWinImpl::init(): Could not set console control handler");
        m_haveValidListener = false;
    }
    printf("Initialised Windows signal handler.\n");
    return m_haveValidListener;
}

Signal SignalListenerWinImpl::getLastSignal() const {
    Signal lastSignal = gLastSignal;
    gLastSignal = Signal::None;
    return lastSignal;
}
#endif