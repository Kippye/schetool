#pragma once

enum class Signal {
    None,
    Interrupt,
    Close,
    Logoff,
    Shutdown,
    Abort,
    Segfault
};