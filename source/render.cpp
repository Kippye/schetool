#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <render.h>
#include <chrono>

void Render::init(Window* windowManager, Interface* interface)
{
	m_windowManager = windowManager;
	m_interface = interface;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Render::render()
{
	double currentFrame = glfwGetTime();
	/// TIME AND TIMERS ///
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	timeCounter += deltaTime;
	timeSinceStart += deltaTime;

	if (timeCounter >= 0.25f)
	{
		FPS = (int)((1.0f / deltaTime) + 0.5f);
		timeCounter = 0.0f;
	}

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glClear(GL_DEPTH_BUFFER_BIT);

	m_interface->draw();

	glfwSwapBuffers(m_windowManager->window);
}