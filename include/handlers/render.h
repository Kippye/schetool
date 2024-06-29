#pragma once

#include <window.h>
#include <interface.h>

class Render
{
	private:
		Window* m_windowManager;
		Interface* m_interface;

		float timeCounter = 0.0f;
		double timeSinceStart = 0.0;
	public:
		double deltaTime = 0.0, lastFrame = 0.0, FPS = 0.0;

	public:
		void init(Window*, Interface*);
		void render();
};
