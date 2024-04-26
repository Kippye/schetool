#include <main.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#ifdef NDEBUG
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

	// SelectOptions options;

	// SelectContainer test(options);
	// test.listenToCallback();
	
	// std::cout << test.m_options << std::endl;

	// SelectContainer* other = new SelectContainer(test);
	// other->listenToCallback();

	// std::cout << other->m_options << std::endl;

	// SelectContainer wowie;

	// std::cout << wowie.m_options << std::endl;

	// wowie = *other;
	// wowie.listenToCallback();

	// std::cout << wowie.m_options << std::endl;

	// delete other;
	// std::cout << "TEST OVER" << std::endl;

	// setup and initialize components
	windowManager.init(&textureLoader);
	input.init(&windowManager);
	ioHandler.init(&schedule, input);
	render.init(&windowManager, &interface);
	interface.init(&windowManager, &input, &schedule, &ioHandler);
	schedule.init(input);

	schedule.createDefaultSchedule();

	#ifdef DEBUG
		int seed = 1714051310; //1713956679;
		EditHistoryTest editHistoryTest = EditHistoryTest(1000, seed == 0 ? time(NULL) : seed, &schedule, 0);
		if (editHistoryTest.begin())
		{
			std::cout << "TESTS PASSED!" << std::endl;
		}
	#endif

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