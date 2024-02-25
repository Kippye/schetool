#pragma once

#include <window.h>
#include <textures.h>
#include <camera.h>
#include <render.h>
#include <input.h>
#include <interface.h>

// TEMP?
#include <schedule.h>

#define PROGRAM_VERSION "0.1.0"

// class Window;
// class TextureLoader;
// class Camera;
// class Render;
// class Input;
// class Interface;
// class Schedule;

class Program
{
	public:
		Window windowManager;
		TextureLoader textureLoader;
		Camera camera;
		Render render;
		Input input;
		Interface interface;

		// TEMP
		Schedule schedule;

		bool programWillClose = false;
		bool quitProgram = false;

		// Program(const Program&) = default;
		// Program& operator=(const Program&) = default;
		Program();
		void loop();
};
