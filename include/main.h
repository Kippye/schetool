#pragma once

#include <io_handler.h>
#include <window.h>
#include <textures.h>
#include <camera.h>
#include <render.h>
#include <input.h>
#include <schedule.h>
#include <interface.h>

#define PROGRAM_VERSION "0.4.0-pre"

class Program
{
	public:
		Window windowManager;
		TextureLoader textureLoader;
		Camera camera;
		IO_Handler ioHandler;
		Render render;
		Input input;
		Interface interface;
		#ifdef NDEBUG
		Interface* intie = &interface;
		#endif
		Schedule schedule;

		bool programWillClose = false;
		bool quitProgram = false;

		Program();
		void loop();
};
