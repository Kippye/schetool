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

// DWORD CTRL_NONE = 100;
// DWORD gLastCtrlType = 100;
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

// LRESULT CALLBACK WindowProc(
//     __in HWND hWindow,
//     __in UINT uMsg,
//     __in WPARAM wParam,
//     __in LPARAM lParam)
// {
//     switch (uMsg)
//     {
//         case WM_CLOSE:
//             gLastCtrlType = CTRL_CLOSE_EVENT;
//             // DestroyWindow(hWindow);
//             break;
//         case WM_DESTROY:
//             gLastCtrlType = CTRL_C_EVENT;
//             // PostQuitMessage(0);
//             break;
//         default:
//             return DefWindowProc(hWindow, uMsg, wParam, lParam);
//     }

//     return 0;
// }

// LRESULT CALLBACK SubWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) 
// {	
    // switch(msg)
    // {
    //     case WM_CLOSE:
    //     gLastSignal = Signal::Close;
    //     printf("SubWndProc(): WM_CLOSE\n");
    //     break;

    //     case WM_QUERYENDSESSION:
    //     {
    //         // Computer is shutting down
    //         if (lParam == 0)
    //         {
    //             gLastSignal = Signal::Shutdown;
    //             printf("SubWndProc(): Returning Shutdown (WM_QUERYENDSESSION)\n");
    //         }
    //         // User is logging off
    //         if ((lParam & ENDSESSION_LOGOFF) == ENDSESSION_LOGOFF)
    //         {
    //             gLastSignal = Signal::Logoff;
    //             printf("SubWndProc(): Returning Logoff (WM_QUERYENDSESSION)\n");
    //         }
    //         break;
    //     }

    //     case WM_QUIT:
    //     gLastSignal = Signal::Close;
    //     printf("SubWndProc(): Returning Close (WM_QUIT)\n");
    //     break;

    //     default:
    //     // printf("SubWndProc(): Other signal with value: %d\n", msg);
    //     break;
    // }
    // return CallWindowProc(gGlfwWndProc, hWnd, msg, wParam, lParam);
// }

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
                auto time = std::chrono::system_clock::now();
                auto secs = std::chrono::floor<std::chrono::seconds>(time);
                auto millis = std::chrono::milliseconds(std::chrono::floor<std::chrono::milliseconds>(time - secs));
                std::string timeString = TimeWrapper::getString(time, TIME_FORMAT_TIME).append(std::format(" + {}ms", millis.count()));
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
            auto time = std::chrono::system_clock::now();
            auto secs = std::chrono::floor<std::chrono::seconds>(time);
            auto millis = std::chrono::milliseconds(std::chrono::floor<std::chrono::milliseconds>(time - secs));
            std::string timeString = TimeWrapper::getString(time, TIME_FORMAT_TIME).append(std::format(" + {}ms", millis.count()));
            printf("Subclassproc(): Got Endsession (shutdown in 5 seconds!) at %s after WM_QUERYENDSESSION (WM_ENDSESSION)\n", timeString.c_str());
        }

        case WM_QUIT:
        gLastSignal = Signal::Close;
        printf("Subclassproc(): Returning Close (WM_QUIT)\n");
        break;

        default:
        printf("Subclassproc(): Other signal with value: %d\n", uMsg);
        break;
    }

    return CallWindowProc(gGlfwWndProc, hWnd, uMsg, wParam, lParam);
    // return 0;DefWindowProcA(hWnd, uMsg, wParam, lParam);
}

bool SignalHandlerWinImpl::init()
{
    printf("INIT");
    // gGlfwWndProc = (WNDPROC)GetWindowLongPtr(glfwGetWin32Window(m_window.window), GWLP_WNDPROC);
    // std::cout << gGlfwWndProc << std::endl;
    // gGlfwWndProc = (WNDPROC)SetWindowLongPtr(glfwGetWin32Window(m_window.window), GWLP_WNDPROC, (LONG_PTR)SubWndProc);
    // std::cout << gGlfwWndProc << std::endl;
    gGlfwWndProc = (WNDPROC)GetWindowLongPtr(glfwGetWin32Window(m_window.window), GWLP_WNDPROC);
    if (SetWindowSubclass(glfwGetWin32Window(m_window.window), Subclassproc, gSubClassID, NULL))
    {
        printf("Window subclass successfully created!\n");
    }

    // auto hkprc = (HOOKPROC)GetProcAddress(hinstDLL, "SysMessageProc"); 
    // HHOOK SetWindowsHookExA(
    //     WH_CALLWNDPROC,
    //     HOOKPROC  lpfn,
    //     NULL,
    //     0
    // );
    // Calling the CallNextHookEx function function to chain to the next hook procedure is optional, but it is highly recommended
    // Before terminating, an application must call the UnhookWindowsHookEx function function to free system resources associated with the hook.
    if (SetConsoleCtrlHandler(CtrlHandler, TRUE))
    {
        m_haveValidListener = true;
    }
    else
    {
        printf("\nERROR: Could not set control handler");
        m_haveValidListener = false;
    }
    return m_haveValidListener;
}

Signal SignalHandlerWinImpl::getLastSignal() const
{
    // Signal lastSignal = Signal::None;
    // switch (gLastCtrlType)
    // {
    //     case CTRL_C_EVENT:
    //     lastSignal = Signal::Interrupt;
    //     printf("getLastSignal(): Returning interrupt\n");
    //     break;

    //     case CTRL_CLOSE_EVENT:
    //     lastSignal = Signal::Close;
    //     break;

    //     case CTRL_LOGOFF_EVENT:
    //     lastSignal = Signal::Logoff;
    //     break;

    //     case CTRL_SHUTDOWN_EVENT:
    //     lastSignal = Signal::Shutdown;
    //     break;

    //     default:
    //     break;
    // }    
    // gLastCtrlType = CTRL_NONE;
    // MSG msg;
    // printf("BEFOREPEEK");
    // if (PeekMessage(&msg, /*glfwGetWin32Window(m_window.window)*/ NULL, 0, WM_QUIT + 1, PM_NOREMOVE))
    // {
    //     std::cout << msg.message << std::endl;
    //     switch (msg.message)
    //     {
    //         case WM_CLOSE:
    //         lastSignal = Signal::Close;
    //         printf("getLastSignal(): Returning Close (WM_CLOSE)\n");
    //         break;

    //         case WM_QUERYENDSESSION:
    //         {
    //             // Computer is shutting down
    //             if (msg.lParam == 0)
    //             {
    //                 lastSignal = Signal::Shutdown;
    //                 printf("getLastSignal(): Returning Shutdown (WM_QUERYENDSESSION)\n");
    //             }
    //             // User is logging off
    //             if ((msg.lParam & ENDSESSION_LOGOFF) == ENDSESSION_LOGOFF)
    //             {
    //                 lastSignal = Signal::Logoff;
    //                 printf("getLastSignal(): Returning Logoff (WM_QUERYENDSESSION)\n");
    //             }
    //             break;
    //         }

    //         case WM_QUIT:
    //         lastSignal = Signal::Close;
    //         printf("getLastSignal(): Returning Close (WM_QUIT)\n");
    //         break;

    //         default:
    //         printf("getLastSignal(): Other signal with value: %d\n", msg.message);
    //         break;
    //     }
    // }
    // printf("AFTERPEEK\n");

    Signal lastSignal = gLastSignal;
    gLastSignal = Signal::None;
    return lastSignal;
}