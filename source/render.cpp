#include <GLFW/glfw3.h>

#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <main.h>
#include <render.h>
#include <chrono>

class Program;
extern Program program;

void Render::setup()
{
	// [debug] glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//~ // general settings
	glEnable(GL_DEPTH_TEST);
	// add blending for transparency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// every shader and render call will now use this shader program
	shader = Shader("shaders/tile.vert", "shaders/tile.geom", "shaders/tile.frag");

	shader.use();
	shader.setInt("texture1", 0);
}

void Render::render()
{
	// render loop
	float currentFrame = glfwGetTime();
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
	//glClearColor(program.editor.backgroundColor.r, program.editor.backgroundColor.g, program.editor.backgroundColor.b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// use the general purpose shader
	shader.use();

	// set uniforms
	shader.setMat4("view", program.camera.view);
	shader.setMat4("projection", program.camera.projection);
	shader.setFloat("time", timeSinceStart);

	glClear(GL_DEPTH_BUFFER_BIT);

	program.gui.drawGui();

	glfwSwapBuffers(program.windowManager.window);
}

void Render::terminate()
{
	// dead
}
