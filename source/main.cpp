#include "main.h"
#include <GLFW/glfw3.h>
#include <locale>

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
    // use user-preferred locale from OS
    std::setlocale(LC_ALL, ""); 

	// TODO: load user preferences here!

	// setup and initialize components
	windowManager.init();
    textureLoader.init();
    windowManager.loadIcon(textureLoader);
	input.init(&windowManager);
	interface.init(&windowManager, &input, textureLoader);
	render.init(&windowManager, &interface);
	schedule.init(input, interface);
	ioHandler.init(&schedule, &windowManager, input, interface);
    timeHandler.init(ioHandler, schedule);

	schedule.createDefaultSchedule();

    ioHandler.openMostRecentFile();
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
        timeHandler.timeTick();

		glfwPollEvents();
	}
	
	std::cout << "Terminating program..." << std::endl;
	windowManager.terminate();
}

#ifndef PERFORM_UNIT_TESTS
int main()
{
	Program program = Program();
	program.loop();
	return 0;
}
#endif