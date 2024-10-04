#pragma once

#include "io_handler.h"
#include "window.h"
#include "textures.h"
#include "render.h"
#include "input.h"
#include "schedule.h"
#include "interface.h"
#include "time_handler.h"
#include <csignal>

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

		static bool quitProgram;

		Program();
        static void signalHandler (int signum)
        {
            std::cout << "Program received signal: " << signum << std::endl;

            switch(signum)
            {
                // Start the program close process when the OS orders it. Hopefully the autosave can be applied before.
                case(SIGTERM):
                {
                    quitProgram = true;
                    break;
                }
                default:
                break;
            }
        };
		void loop();
};
