#include <main.h>
#include <GLFW/glfw3.h>

// Windows Release build
#if defined(NDEBUG) && (defined (_WIN32) || defined (_WIN64))
	#define WIN_RELEASE
#endif

#ifdef WIN_RELEASE
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
	// TODO: load user preferences here!

	// setup and initialize components
	windowManager.init(&textureLoader);
	input.init(&windowManager);
	ioHandler.init(&schedule, input);
	render.init(&windowManager, &interface);
	interface.init(&windowManager, &input, &schedule, &ioHandler);
	schedule.init(input);

	schedule.createDefaultSchedule();

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

int main()
{
	Program program = Program();
	program.loop();
	return 0;
}