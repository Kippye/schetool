extern "C" {
#include <glad.h>
}
#include <GLFW/glfw3.h>
#include <window.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <stdio.h>
//include <windows.h>
#include <vector>

#include <main.h>
class Program;
extern Program program;

void Window::initialize()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);
	//glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE); //might be needed for bigger monitors?

	window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "schetool", NULL, NULL);

	if (window == NULL)
	{
		std::cout << "Could not create a GLFW window :(" << std::endl;
		glfwTerminate();
		return;
	}
	Window* thisWindow = &(program.windowManager);

	// make the window current and maximize 8)
	glfwMakeContextCurrent(window);
	glfwMaximizeWindow(window);
	glfwSetWindowSizeLimits(window, 0, 0, GLFW_DONT_CARE, GLFW_DONT_CARE);
	glfwSetWindowUserPointer(window, thisWindow);
	// load address of OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Could not initialize GLAD ( not glad :( )" << std::endl;
		return;
	}

	// load and set the window's icon
	GLFWimage images[1] = { GLFWimage() } ;
	images[0].pixels = program.textureLoader.loadTextureData("icon.png", &images[0].width, &images[0].height, program.textureLoader.textureFolder, false);
	glfwSetWindowIcon(window, 1, images);

	// load and create cursors
	// cursors[NORMAL] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
	// GLFWimage drawCursor = GLFWimage();
	// drawCursor.pixels = program.textureLoader.loadTextureData("cursor_draw.png", &drawCursor.width, &drawCursor.height, program.textureLoader.textureFolder, false);
	// cursors[DRAW] = glfwCreateCursor(&drawCursor, 8, 8);

	// set up the viewport (xpos, ypos, w, h)
	int currentWidth, currentHeight;
	glfwGetWindowSize(window, &currentWidth, &currentHeight);
	SCREEN_WIDTH = currentWidth;
	SCREEN_HEIGHT = currentHeight;
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	#define genericCallback(functionName)\
	[](GLFWwindow* win, auto... args) {\
		auto pointer = static_cast<Window*>(glfwGetWindowUserPointer(win));\
		if (pointer->functionName) pointer->functionName(pointer, args...);\
	}

	// window callbacks
	glfwSetFramebufferSizeCallback(window, genericCallback(framebuffer_size_callback));
	glfwSetWindowFocusCallback(window, genericCallback(window_focus_callback));
	glfwSetWindowCloseCallback(window, genericCallback(window_close_callback));
	// input callbacks (not handled here)
	glfwSetKeyCallback(window, genericCallback(key_callback));
	glfwSetMouseButtonCallback(window, genericCallback(mouse_button_callback));
	glfwSetCursorPosCallback(window, genericCallback(cursor_pos_callback));
	glfwSetScrollCallback(window, genericCallback(scroll_callback));

	// linking callback events
	thisWindow->framebuffer_size_callback = [](auto self, int width, int height)
	{
		if (width > 0 && height > 0)
		{
			self->SCREEN_WIDTH = width;
			self->SCREEN_HEIGHT = height;
			glViewport(0, 0, width, height);
		}
	};

	thisWindow->window_focus_callback = [](auto self, int focused)
	{
		if (focused)
		{
			//program.file_system.updateTextures();
			//glfwSetInputMode(self->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else
		{
			//glfwSetInputMode(self->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		self->hasFocus = focused;
	};

	thisWindow->window_close_callback = [](auto self)
	{
		self->shouldClose = true;
		std::cout << "Program close requested." << std::endl;
	};

	thisWindow->key_callback = [](auto self, int key, int scancode, int action, int mods)
	{
		program.input.key_event(self->window, key, scancode, action, mods);
	};

	thisWindow->mouse_button_callback = [](auto self, int button, int action, int mods)
	{
		program.input.mouse_button_event(self->window, button, action, mods);
	};

	thisWindow->cursor_pos_callback = [](auto self, double xPos, double yPos)
	{
		program.input.cursor_pos_event(self->window, xPos, yPos);
	};

	thisWindow->scroll_callback = [](auto self, double xOffset, double yOffset)
	{
		program.input.scroll_event(self->window, xOffset, yOffset);
	};
}

void Window::setCursor(CURSOR_TYPE cursor)
{
	glfwSetCursor(window, cursors[cursor]);
}

void Window::setTitle(const char* _title)
{
	title = _title;

	// no file is opened, just show "blue"
	if (title == "")
	{
		glfwSetWindowTitle(window, titleBase);
	}
}

void Window::terminate()
{
	glfwDestroyWindow(program.windowManager.window);
	glfwTerminate();
}
