#ifdef _WIN32
#include "signal_handler_impl_win.h"

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

// TEMP?
#include "time_wrapper.h"

Signal gLastSignal = Signal::None;
WNDPROC gGlfwWndProc;
UINT_PTR gSubClassID = 420;

BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
{
    switch (fdwCtrlType)
    {
        // Handle the CTRL-C signal.
        case CTRL_C_EVENT:
            gLastSignal = Signal::Interrupt;
            printf("Ctrl-C event\n\n");
        return TRUE;

        // CTRL-CLOSE: confirm that the user wants to exit.
        case CTRL_CLOSE_EVENT:
            gLastSignal = Signal::Close;
            printf("Ctrl-Close event\n\n");
        return TRUE;

        // Pass other signals to the next handler.
        case CTRL_BREAK_EVENT:
            printf("Ctrl-Break event\n\n");
        return TRUE;

        case CTRL_LOGOFF_EVENT:
            gLastSignal = Signal::Logoff;
            printf("Ctrl-Logoff event\n\n");
        return TRUE;

        case CTRL_SHUTDOWN_EVENT:
            gLastSignal = Signal::Shutdown;
            printf("Ctrl-Shutdown event\n\n");
        return TRUE;

        default:
            return FALSE;
    }
}

LRESULT Subclassproc(
    HWND hWnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam,
    UINT_PTR uIdSubclass,
    DWORD_PTR dwRefData
)
{
    switch(uMsg)
    {
        case WM_PAINT: // Don't even print this spam
        break;
        case WM_CLOSE:
        gLastSignal = Signal::Close;
        printf("Subclassproc(): WM_CLOSE\n");
        return 1;
        // break;

        case WM_QUERYENDSESSION:
        {
            // Computer is shutting down
            if (lParam == 0)
            {
                gLastSignal = Signal::Shutdown;
                auto time = TimeWrapper(std::chrono::system_clock::now()).getLocalTime();
                auto floorMinutes = std::chrono::floor<std::chrono::minutes>(time);
                auto secs = std::chrono::seconds(std::chrono::floor<std::chrono::seconds>(time - floorMinutes));
                auto floorSecs = std::chrono::floor<std::chrono::seconds>(time);
                auto millis = std::chrono::milliseconds(std::chrono::floor<std::chrono::milliseconds>(time - floorSecs));
                std::string timeString = std::format("{}:{}:{}ms", TimeWrapper::getString(time, TIME_FORMAT_TIME), secs.count(), millis.count());
                printf("Subclassproc(): Returning Shutdown at %s (WM_QUERYENDSESSION)\n", timeString.c_str());
                return 1;
            }
            // User is logging off
            if ((lParam & ENDSESSION_LOGOFF) == ENDSESSION_LOGOFF)
            {
                gLastSignal = Signal::Logoff;
                printf("Subclassproc(): Returning Logoff (WM_QUERYENDSESSION)\n");
            }
            break;
        }

        case WM_ENDSESSION:
        {
            // Set Signal::Shutdown NOW so the program has time to handle things before the actual shutdown
            gLastSignal = Signal::Shutdown;
            auto time = TimeWrapper(std::chrono::system_clock::now()).getLocalTime();
            auto floorMinutes = std::chrono::floor<std::chrono::minutes>(time);
            auto secs = std::chrono::seconds(std::chrono::floor<std::chrono::seconds>(time - floorMinutes));
            auto floorSecs = std::chrono::floor<std::chrono::seconds>(time);
            auto millis = std::chrono::milliseconds(std::chrono::floor<std::chrono::milliseconds>(time - floorSecs));
            std::string timeString = std::format("{}:{}:{}ms", TimeWrapper::getString(time, TIME_FORMAT_TIME), secs.count(), millis.count());
            printf("Subclassproc(): Got Endsession (shutdown in 5 seconds!) at %s after WM_QUERYENDSESSION (WM_ENDSESSION)\n", timeString.c_str());
        }

        case WM_QUIT:
        gLastSignal = Signal::Close;
        printf("Subclassproc(): Returning Close (WM_QUIT)\n");
        break;

        default:
        // printf("Subclassproc(): Other signal with value: %d\n", uMsg);
        break;
    }

    return CallWindowProc(gGlfwWndProc, hWnd, uMsg, wParam, lParam);
    // return 0;DefWindowProcA(hWnd, uMsg, wParam, lParam);
}

bool SignalHandlerWinImpl::init()
{
    printf("Initialising Signal Handler Windows implementation.\n");
    gGlfwWndProc = (WNDPROC)GetWindowLongPtr(glfwGetWin32Window(m_window.window), GWLP_WNDPROC);
    if (SetWindowSubclass(glfwGetWin32Window(m_window.window), Subclassproc, gSubClassID, (DWORD_PTR)nullptr))
    {
        // printf("Window subclass successfully created!\n");
    }

    if (SetConsoleCtrlHandler(CtrlHandler, TRUE))
    {
        m_haveValidListener = true;
    }
    else
    {
        printf("\nERROR: SignalHandlerWinImpl::init(): Could not set console control handler");
        m_haveValidListener = false;
    }
    return m_haveValidListener;
}

Signal SignalHandlerWinImpl::getLastSignal() const
{
    Signal lastSignal = gLastSignal;
    gLastSignal = Signal::None;
    return lastSignal;
}
#endif