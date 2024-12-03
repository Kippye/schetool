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
    signalHandler.init(windowManager);
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

void Program::handleSignal(Signal signal)
{
    switch(signal)
    {
        case(Signal::None):
        {
            return;
        }
        case(Signal::Close):
        {
            std::cout << "Program received signal: Close" << std::endl;
            quitProgram = true;
            break;
        }
        case(Signal::Shutdown):
        {
            std::cout << "Program received signal: Shutdown" << std::endl;
            quitProgram = true;
            break;
        }
        case(Signal::Segfault):
        {
            std::cout << "Program received signal: Segfault" << std::endl;
            // TODO: Exit right away
            break;
        }
        case(Signal::Interrupt):
        {
            std::cout << "Program received signal: Interrupt" << std::endl;
            quitProgram = true;
            break;
        }
        case(Signal::Abort):
        {
            std::cout << "Program received signal: Abort" << std::endl;
            // TODO: Exit right away
            break;
        }
        default:
        std::cout << "Program received unknown signal: " << (int)signal << std::endl;
        break;
    }
}

void Program::loop()
{
	while (quitProgram == false)
	{
		input.processInput(windowManager.window);

		if (glfwWindowShouldClose(windowManager.window))
		{
            std::cout << "GLFW window should close. Quitting program." << std::endl;
			quitProgram = true;
		}
        if (quitProgram)
            break;
		render.render();
		ioHandler.addToAutosaveTimer(render.deltaTime);
        timeHandler.timeTick();

        handleSignal(signalHandler.listenForSignals());
		glfwPollEvents();
	}
	
    auto time = TimeWrapper(std::chrono::system_clock::now()).getLocalTime();
    auto floorMinutes = std::chrono::floor<std::chrono::minutes>(time);
    auto secs = std::chrono::seconds(std::chrono::floor<std::chrono::seconds>(time - floorMinutes));
    auto floorSecs = std::chrono::floor<std::chrono::seconds>(time);
    auto millis = std::chrono::milliseconds(std::chrono::floor<std::chrono::milliseconds>(time - floorSecs));
    std::string timeString = std::format("{}:{}:{}ms", TimeWrapper::getString(time, TIME_FORMAT_TIME), secs.count(), millis.count());
	std::cout << "Terminating program at " << timeString << "..." << std::endl;
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

bool Program::quitProgram = false;