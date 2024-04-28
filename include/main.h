#pragma once

#include <io_handler.h>
#include <window.h>
#include <textures.h>
#include <render.h>
#include <input.h>
#include <schedule.h>
#include <interface.h>

#define PROGRAM_VERSION "0.5.0"

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

		bool programWillClose = false;
		bool quitProgram = false;

		Program();
		void loop();
};
