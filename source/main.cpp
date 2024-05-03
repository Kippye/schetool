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

#ifdef DEBUG
#include <tests/test_edit_history.h>
#endif

Program::Program()
{
	// TODO: load user preferences here!

	// setup and initialize components
	windowManager.init(&textureLoader, PROGRAM_NAME, PROGRAM_VERSION);
	input.init(&windowManager);
	interface.init(&windowManager, &input);
	ioHandler.init(&schedule, &windowManager, input, interface);
	render.init(&windowManager, &interface);
	schedule.init(input, interface);

	schedule.createDefaultSchedule();

	#ifdef DEBUG
		// int seed = 0; //1713956679;
		// EditHistoryTest editHistoryTest = EditHistoryTest(1000, seed == 0 ? time(NULL) : seed, &schedule, 0);
		// if (editHistoryTest.begin())
		// {
		// 	std::cout << "TESTS PASSED!" << std::endl;
		// }
	#endif

	// There are pre-existing Schedules. Open the most recently edited one.
	if (ioHandler.getScheduleStemNames().size() > 0)
	{
		// std::cout << "reading.." << ioHandler.getLastEditedScheduleStemName() << std::endl;
		ioHandler.readSchedule(ioHandler.getLastEditedScheduleStemName().c_str());
	}
	// There are no Schedule files. Ask Interface to ask the MainMenuBarGui to start the process for creating a new Schedule file. Yes. This is stupid.
	else
	{
		std::dynamic_pointer_cast<MainMenuBarGui>(interface.getGuiByID("MainMenuBarGui"))->openScheduleNameModal(NAME_PROMPT_NEW);
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

#ifdef WINRELEASE
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
