#include <iostream>
extern "C" {
#include <glad.h>
}
#include <GLFW/glfw3.h>

#include <window.h>
#include <generated/program_info.h>

void Window::init()
{
	m_titleBase = program_info::PROGRAM_NAME;
	m_titleBase = m_titleBase.append(" ").append(program_info::ProgramVersion::getCurrent()
		.getString()
		#ifdef DEBUG
		.append(std::string(" (DEBUG) "))
		#endif
	);

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);
	glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE); //might be needed for bigger monitors?

	window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, m_titleBase.c_str(), NULL, NULL);
	setTitle(m_titleBase);

	if (window == NULL)
	{
		std::cout << "Could not create a GLFW window :(" << std::endl;
		glfwTerminate();
		return;
	}

	// make the window current and maximize 8)
	glfwMakeContextCurrent(window);
	glfwMaximizeWindow(window);
	glfwSetWindowSizeLimits(window, WINDOW_MIN_WIDTH, WINDOW_MIN_HEIGHT, GLFW_DONT_CARE, GLFW_DONT_CARE);
	glfwSetWindowUserPointer(window, this);
	// load address of OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Could not initialize GLAD ( not glad :( )" << std::endl;
		return;
	}

	// load and create cursors
	// cursors[NORMAL] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
	// GLFWimage drawCursor = GLFWimage();
	// drawCursor.pixels = program->textureLoader.loadTextureData("cursor_draw.png", &drawCursor.width, &drawCursor.height, program->textureLoader.textureFolder, false);
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
	this->framebuffer_size_callback = [](auto self, int width, int height)
	{
		if (width > 0 && height > 0)
		{
			self->SCREEN_WIDTH = width;
			self->SCREEN_HEIGHT = height;
			glViewport(0, 0, width, height);
		}
	};

	this->window_focus_callback = [](auto self, int focused)
	{
		if (focused)
		{
			//program->file_system.updateTextures();
			//glfwSetInputMode(self->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else
		{
			//glfwSetInputMode(self->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		self->hasFocus = focused;
	};

	this->window_close_callback = [](auto self)
	{
		self->shouldClose = true;
        self->windowCloseEvent.invoke();
		std::cout << "Program close requested." << std::endl;
	};
}

void Window::loadIcon(TextureLoader& textureLoader)
{
	GLFWimage images[1] = { GLFWimage() } ;
	images[0].pixels = textureLoader.loadTextureData(textureLoader.getRelativePathFromTextureFolder("icon.png"), &images[0].width, &images[0].height, nullptr, false);
    if (images[0].pixels)
    {
        glfwSetWindowIcon(window, 1, images);
    }
    else
    {
        printf("Window::init(..): Failed to load program icon from path: %s\n", (textureLoader.textureFolder + "icon.png").c_str());
    }
}

void Window::setCursor(CURSOR_TYPE cursor)
{
	glfwSetCursor(window, cursors[cursor]);
}

void Window::setTitle(const std::string& title)
{
	m_title = title;
	glfwSetWindowTitle(window, title.c_str());
}

void Window::setTitleSuffix(const std::string& suffix)
{
	std::string newTitle = m_titleBase;
	setTitle(newTitle.append(suffix));
}

std::string Window::getTitle()
{
	return m_title;
}

void Window::terminate()
{
    windowCloseEvent.invoke();
	glfwDestroyWindow(window);
	glfwTerminate();
}
