#pragma once

#include <glm/glm.hpp>
#include <window.h>
#include <interface.h>

class Window;
class Interface;

class Input
{
	private:
		Window* m_windowManager;
		Interface* m_interface;
		bool firstMouseMovement = true;
	public:
		glm::vec2 mousePos;
		glm::vec2 mouseMovement;
		bool ctrl_down = false, shift_down = false, alt_down = false, lmb_down = false, rmb_down = false, lmb_down_last = false, rmb_down_last = false;
		const float mouseRepeatDelay = 0.05f;

		void init(Window*, Interface*);
		void processInput(GLFWwindow* window);

		void key_event(GLFWwindow* window, int key, int scancode, int action, int mods);
		void mouse_button_event(GLFWwindow* window, int button, int action, int mods);
		void cursor_pos_event(GLFWwindow* window, double xPos, double yPos);
		void scroll_event(GLFWwindow* window, double xOffset, double yOffset);
};
