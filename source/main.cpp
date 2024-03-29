#include <main.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
// TEMP
#include <data_converter.h>

#ifdef NDEBUG
#include <Windows.h>
#endif

Program::Program()
{
	// setup and initialize components
	// TODO: load user preferences here!
	// TODO: load last opened schedule file here!

	windowManager.init(&textureLoader);
	camera.init(&windowManager);
	ioHandler.init(&schedule);
	#ifdef NDEBUG
	input.init(&windowManager, &camera, intie);
	render.init(&windowManager, &camera, intie);
	intie->init(&windowManager, &schedule);
	#else
	input.init(&windowManager, &camera, &interface);
	render.init(&windowManager, &camera, &interface);
	interface.init(&windowManager, &schedule, &ioHandler);
	#endif

	// TODO: load textures
	//file_system.loadGUITextures();
	//file_system.updateTextures();

	schedule.test_setup();

	// There are pre-existing Schedules. Open the most recently edited one.
	if (ioHandler.getScheduleStemNames().size() > 0)
	{
		// std::cout << "reading.." << ioHandler.getLastEditedScheduleStemName() << std::endl;
		ioHandler.readSchedule(ioHandler.getLastEditedScheduleStemName().c_str());
	}
	// There are no Schedule files. Ask Interface to ask the MainMenuBarGui to start the process for creating a new Schedule file. Yes. This is stupid.
	else 
	{
		interface.openMainMenuBarScheduleNameModal();
	}
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

#ifdef NDEBUG
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
{
	Program program = Program();
	program.loop();
	return 0;
}
#else
int main()
{
	Program program = Program();
	program.loop();
	return 0;
}
#endif