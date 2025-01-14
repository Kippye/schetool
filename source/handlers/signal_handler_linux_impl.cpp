#include "signal_handler_linux_impl.h"

#include <csignal>
#include <atomic>
#include <stdio.h>
#include <iostream>

std::atomic<int> gLastFrameSignal = -1;

extern "C" void signalHandler(int signum)
{
    gLastFrameSignal.store(signum);
};

bool SignalHandlerLinuxImpl::init()
{
    // Listen to signals
    signal(SIGTERM, signalHandler);
    signal(SIGSEGV, signalHandler);
    signal(SIGINT, signalHandler);
    signal(SIGABRT, signalHandler);
    printf("Initialised Linux signal handler.\n");
    m_haveValidListener = true;
    return m_haveValidListener;
}

Signal SignalHandlerLinuxImpl::getLastSignal() const
{
    Signal lastSignal = Signal::None;

    switch(gLastFrameSignal)
    {
        case SIGTERM:
        lastSignal = Signal::Close;
        printf("getLastSignal(): Returning Close\n");
        break;

        case SIGSEGV:
        lastSignal = Signal::Segfault;
        printf("getLastSignal(): Returning Segfault\n");
        break;

        case SIGINT:
        lastSignal = Signal::Interrupt;
        printf("getLastSignal(): Returning Interrupt\n");
        break;

        case SIGABRT:
        lastSignal = Signal::Abort;
        printf("getLastSignal(): Returning Abort\n");
        break;
    }

    gLastFrameSignal.store(-1) ;
    return lastSignal;
}