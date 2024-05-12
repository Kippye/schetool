#pragma once

extern "C" {
#include <glad.h>
}
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <functional>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <textures.h>

enum CURSOR_TYPE
{
	NORMAL,
	DRAW
};

class Window
{
	private:
		TextureLoader* m_textureLoader;
		std::string m_titleBase = "";
		std::string m_title = "";
	public:
		GLFWwindow* window;
		CURSOR_TYPE cursor = NORMAL;
		GLFWcursor* cursors[2] = {};
		int SCREEN_WIDTH = 800;
		int SCREEN_HEIGHT = 600;
		int WINDOW_MIN_WIDTH = 220;
		int WINDOW_MIN_HEIGHT = 160;
		bool firstMouseMovement = true;
		bool hasFocus = true;
		bool shouldClose = false;
	// window functions
	void setCursor(CURSOR_TYPE _cursor);
	void setTitle(const std::string& title);
	void setTitleSuffix(const std::string& suffix);
	std::string getTitle();
	// lifecycle functions
	void init(TextureLoader*);
	void terminate();

	// window events
	std::function<void(Window*, int, int)> framebuffer_size_callback;
	std::function<void(Window*, int)> window_focus_callback;
	std::function<void(Window*)> window_close_callback;
	// input events
	std::function<void(Window*, int, int, int, int)> key_callback;
	std::function<void(Window*, int, int, int)> mouse_button_callback;
	std::function<void(Window*, double, double)> scroll_callback;
	std::function<void(Window*, double, double)> cursor_pos_callback;
};
