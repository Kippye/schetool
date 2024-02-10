#pragma once

#include <shader.h>
#include <glm.hpp>
#include <vector>

class Render
{
	private:
		Shader shader;

		float timeCounter = 0.0f;
		double timeSinceStart = 0.0;
	public:
		float deltaTime = 0.0f, lastFrame = 0.0f, FPS = 0.0f;

		glm::mat4 projection = glm::mat4(1.0f);

	public:
		void setup();
		void render();
		void terminate();
};
