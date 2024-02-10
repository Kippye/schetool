#include <main.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

Program program;

Program::Program()
{
	// setup and initialize components
	// TODO: load user preferences here!
	// TODO: load last opened schedule file here!

	windowManager.initialize();
	input.setup();
	render.setup();

	// TODO: load textures
	//file_system.loadGUITextures();
	//file_system.updateTextures();
	gui.guiInit(&windowManager);
}

void Program::loop()
{
	while (quitProgram == false)
	{
		input.processInput(windowManager.window);
		//std::cout << "Processed input" << std::endl;
		// update the camera view direction (not really needed but eh)
		camera.updateView();
		//std::cout << "Updated camera view" << std::endl;
		if (glfwWindowShouldClose(windowManager.window))
		{
			programWillClose = quitProgram = true;
		}
		render.render();
		//std::cout << "Rendered" << std::endl;
		glfwPollEvents();
		//std::cout << "Polled GLFW events" << std::endl;
	}
	
	std::cout << "Terminating program..." << std::endl;
	render.terminate();
	windowManager.terminate();
	// file_system.trySaveConfigs();
}

int main()
{
	program.loop();
	return 0;
}
