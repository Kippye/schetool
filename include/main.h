#pragma once

#include "io_handler.h"
#include "window.h"
#include "textures.h"
#include "render.h"
#include "input.h"
#include "schedule.h"
#include "interface.h"
#include "time_handler.h"
#include "signal_handler.h"
#include "notification_handler.h"

class Program
{
	public:
		Window windowManager;
		TextureLoader textureLoader;
		IO_Handler ioHandler;
		Render render;
		Input input;
		Interface interface;
		Schedule schedule;
        TimeHandler timeHandler;
        SignalHandler signalHandler;
		NotificationHandler notificationHandler;

		static bool quitProgram;

		Program();
        void handleSignal(Signal signal);
		void loop();
};
