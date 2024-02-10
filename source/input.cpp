#include <GLFW/glfw3.h>
#include <input.h>
#include <main.h>
#include <iostream>
#include <glm/gtx/norm.hpp>

class Program;
extern Program program;

void Input::setup(){ /* nothing to see here */ }

void Input::processInput(GLFWwindow* window)
{
	// modifiers
	shift_down = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS);
	ctrl_down = (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS);
	alt_down = (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS);

	lmb_down_last = lmb_down;
	lmb_down = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
	rmb_down_last = rmb_down;
	rmb_down = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;

	/// check any situations in which we would not want to control the camera or send inputs to other listeners
	if (program.gui.guiWantKeyboard) { return; }

	glm::vec3 cameraPosBefore = program.camera.cameraPos;

	if (cameraPosBefore == program.camera.cameraPos)
	{
		program.camera.lastMovement = glm::vec3(0.0f);
	}

	// reset mouse movement as it only updates when the mouse is ACTUALLY moved
	mouseMovement = glm::vec2(0.0f);
}

void Input::key_event(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		if (program.gui.guiWantKeyboard) { return; }
		switch (key)
		{
			/// program manipulators

			// shortcuts
		}
	}
	else if (action == GLFW_RELEASE)
	{
		switch (key)
		{
			case (GLFW_KEY_LEFT_SHIFT):
			shift_down = false;
			break;
			case (GLFW_KEY_LEFT_CONTROL):
			ctrl_down = false;
			break;
			case (GLFW_KEY_LEFT_ALT):
			alt_down = false;
			break;
		}
	}
}

void Input::mouse_button_event(GLFWwindow* window, int key, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_MOUSE_BUTTON_LEFT)
		{
			program.windowManager.hasFocus = true;
		}
		else if (key == GLFW_MOUSE_BUTTON_RIGHT)
		{
			program.windowManager.hasFocus = true;
		}
	}
	else if (action == GLFW_RELEASE)
	{
		switch (key)
		{
			case (GLFW_MOUSE_BUTTON_LEFT):
			lmb_down = false;
			break;
			case (GLFW_MOUSE_BUTTON_RIGHT):
			rmb_down = false;
			break;
		}
	}
}

void Input::cursor_pos_event(GLFWwindow* window, double xPos, double yPos)
{
	if (program.windowManager.hasFocus)
	{
		mouseMovement.x = xPos - mousePos.x;
		mouseMovement.y = yPos - mousePos.y;
		mousePos.x = xPos;
		mousePos.y = yPos;
	}
}

void Input::scroll_event(GLFWwindow* window, double xOffset, double yOffset)
{
	return;
}
