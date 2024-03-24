#pragma once

#include <glm.hpp>
#include <vector>
#include <window.h>
#include <camera.h>
#include <interface.h>
#include <shader.h>

class Render
{
	private:
		Window* m_windowManager;
		Camera* m_camera;
		Interface* m_interface;
		Shader shader;

		float timeCounter = 0.0f;
		double timeSinceStart = 0.0;
	public:
		double deltaTime = 0.0, lastFrame = 0.0, FPS = 0.0;

		glm::mat4 projection = glm::mat4(1.0f);

	public:
		void init(Window*, Camera*, Interface*);
		void render();
		void terminate();
};
