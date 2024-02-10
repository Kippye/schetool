#pragma once

#include <window.h>
#include <textures.h>
#include <camera.h>
#include <render.h>
#include <input.h>
#include <gui.h>

#define PROGRAM_VERSION "0.1.0"

class Window;
class TextureLoader;
class Camera;
class Render;
class Input;

class Program
{
	public:
		Window& windowManager = *(new Window());
		TextureLoader& textureLoader = *(new TextureLoader());
		Camera& camera = *(new Camera());
		Render& render = *(new Render());
		Input& input = *(new Input());
		Gui& gui = *(new Gui());

		bool programWillClose = false;
		bool quitProgram = false;

		Program(const Program&) = default;
		Program& operator=(const Program&) = default;
		Program();
		void loop();
};
