#pragma once

extern "C" {
#include <glad.h>
}
#include <GLFW/glfw3.h>

#include <functional>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <string>
#include <input.h>
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
	public:
		GLFWwindow* window;
		const char* titleBase = "schetool";
		std::string title = "";
		CURSOR_TYPE cursor = NORMAL;
		GLFWcursor* cursors[2] = {};
		int SCREEN_WIDTH = 800;
		int SCREEN_HEIGHT = 600;
		bool firstMouseMovement = true;
		bool hasFocus = true;
		bool shouldClose = false;
	// window functions
	void setCursor(CURSOR_TYPE _cursor);
	void setTitle(const char* _title);
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
