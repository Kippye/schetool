#include <main.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
// TEMP
#include <data_converter.h>

#ifdef NDEBUG
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#endif

Program::Program()
{
	// setup and initialize components
	// TODO: load user preferences here!

	windowManager.init(&textureLoader);
	ioHandler.init(&schedule);
	input.init(&windowManager, &interface);
	render.init(&windowManager, &interface);
	interface.init(&windowManager, &schedule, &ioHandler);

	schedule.createDefaultSchedule();

	// // There are pre-existing Schedules. Open the most recently edited one.
	// if (ioHandler.getScheduleStemNames().size() > 0)
	// {
	// 	// std::cout << "reading.." << ioHandler.getLastEditedScheduleStemName() << std::endl;
	// 	ioHandler.readSchedule(ioHandler.getLastEditedScheduleStemName().c_str());
	// }
	// // There are no Schedule files. Ask Interface to ask the MainMenuBarGui to start the process for creating a new Schedule file. Yes. This is stupid.
	// else 
	// {
	// 	interface.openMainMenuBarScheduleNameModal();
	// }
}

void Program::loop()
{
	while (quitProgram == false)
	{
		input.processInput(windowManager.window);

		if (glfwWindowShouldClose(windowManager.window))
		{
			programWillClose = quitProgram = true;
		}
		render.render();
		ioHandler.addToAutosaveTimer(render.deltaTime);

		glfwPollEvents();
	}
	
	std::cout << "Terminating program..." << std::endl;
	windowManager.terminate();
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