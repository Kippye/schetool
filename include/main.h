#pragma once

#include "io_handler.h"
#include "window.h"
#include "textures.h"
#include "render.h"
#include "input.h"
#include "schedule.h"
#include "interface.h"
#include "time_handler.h"
#include "signal_listener.h"
#include "notification_handler.h"

#ifdef SCHETOOL_LINUX
#include <libnotify/notify.h>
#endif

class Program
{
	public:
		Window windowManager;
		TextureLoader textureLoader;
		IO_Handler ioHandler;
		Render render;
		Input input;
		Interface programInterface;
		Schedule schedule;
        TimeHandler timeHandler;
        SignalListener signalListener;
		NotificationHandler notificationHandler;
		#ifdef SCHETOOL_LINUX
		GMainContext* gContext = nullptr;
		GMainLoop* notifyLoop = nullptr;
		#endif

		static bool quitProgram;

		Program();
        void handleSignal(Signal signal);
		void loop();
};
