#include <GLFW/glfw3.h>
#include <input.h>
#include <iostream>
#include <glm/gtx/norm.hpp>

void Input::init(Window* windowManager, Camera* camera, Interface* interface)
{ 
	m_windowManager = windowManager;
	m_camera = camera;
	m_interface = interface;

	m_windowManager->key_callback = [=](auto self, int key, int scancode, int action, int mods)
	{
		this->key_event(self->window, key, scancode, action, mods);
	};

	m_windowManager->mouse_button_callback = [=](auto self, int button, int action, int mods)
	{
		this->mouse_button_event(self->window, button, action, mods);
	};

	m_windowManager->cursor_pos_callback = [=](auto self, double xPos, double yPos)
	{
		this->cursor_pos_event(self->window, xPos, yPos);
	};

	m_windowManager->scroll_callback = [=](auto self, double xOffset, double yOffset)
	{
		this->scroll_event(self->window, xOffset, yOffset);
	};
}

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
	if (m_interface->guiWantKeyboard) { return; }

	glm::vec3 cameraPosBefore = m_camera->cameraPos;

	if (cameraPosBefore == m_camera->cameraPos)
	{
		m_camera->lastMovement = glm::vec3(0.0f);
	}

	// reset mouse movement as it only updates when the mouse is ACTUALLY moved
	mouseMovement = glm::vec2(0.0f);
}

void Input::key_event(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		if (m_interface->guiWantKeyboard) { return; }
		// switch (key)
		// {
		// 	/// program manipulators

		// 	// shortcuts
		// }
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
			m_windowManager->hasFocus = true;
		}
		else if (key == GLFW_MOUSE_BUTTON_RIGHT)
		{
			m_windowManager->hasFocus = true;
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
	if (m_windowManager->hasFocus)
	{
		mouseMovement.x = (float)xPos - mousePos.x;
		mouseMovement.y = (float)yPos - mousePos.y;
		mousePos.x = (float)xPos;
		mousePos.y = (float)yPos;
	}
}

void Input::scroll_event(GLFWwindow* window, double xOffset, double yOffset)
{
	return;
}
