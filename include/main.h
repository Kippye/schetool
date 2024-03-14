#pragma once

#include <window.h>
#include <textures.h>
#include <camera.h>
#include <render.h>
#include <input.h>
#include <interface.h>
#include <schedule.h>

#define PROGRAM_VERSION "0.3.0-pre"

class Program
{
	public:
		Window windowManager;
		TextureLoader textureLoader;
		Camera camera;
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
