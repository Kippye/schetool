#include <iostream>
#include <glm/gtx/norm.hpp>
#include <input.h>

void Input::init(Window* windowManager)
{ 
	m_windowManager = windowManager;
	
	for (size_t i = 0; i <= INPUT_CALLBACK_SC_REDO; i++)
	{
		m_callbacks.insert({(INPUT_CALLBACK)i, std::vector<std::function<void()>>{}});
		m_callbackStates.insert({(INPUT_CALLBACK)i, false});
		m_callbackLastFrameStates.insert({(INPUT_CALLBACK)i, false});
	}

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
	for (auto callback: m_callbackLastFrameStates)
	{
		m_callbackLastFrameStates.at(callback.first) = m_callbackStates.at(callback.first);
		m_callbackStates.at(callback.first) = false;
	}

	// modifiers
	buttonStates.shiftDown = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS);
	buttonStates.ctrlDown = (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS);
	buttonStates.altDown = (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS);

	buttonStates.lmbDownLast = buttonStates.lmbDown;
	buttonStates.lmbDown = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
	buttonStates.rmbDownLast = buttonStates.rmbDown;
	buttonStates.rmbDown = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;

	/// check any situations in which we would not want to control the camera or send inputs to other listeners
	if (m_guiWantKeyboard) { return; }

	// reset mouse movement as it only updates when the mouse is ACTUALLY moved
	mouseMovement = glm::vec2(0.0f);
}

void Input::addCallbackListener(INPUT_CALLBACK callback, std::function<void()>& listener)
{
    if (listener)
    {
	    m_callbacks.at(callback).push_back(listener);
    }
}

size_t Input::getCallbackListenerCount(INPUT_CALLBACK callback)
{
    return m_callbacks.at(callback).size();
}

void Input::invokeCallback(INPUT_CALLBACK callback)
{
	auto& listeners = m_callbacks.at(callback);

	for (auto& listener : listeners)
	{
		listener();
	}
	m_callbackStates.at(callback) = true;
}

bool Input::getCallbackInvokedLastFrame(INPUT_CALLBACK callback)
{
	return m_callbackLastFrameStates.at(callback);
}

void Input::setGuiWantKeyboard(bool to)
{
	m_guiWantKeyboard = to;
}

void Input::key_event(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		if (m_guiWantKeyboard) { return; }
		for (const InputShortcut& shortcut: m_shortcuts)
		{
			if (key == shortcut.key && shortcut.hasRequiredMods(buttonStates.ctrlDown, buttonStates.altDown, buttonStates.shiftDown))
			{
				invokeCallback(shortcut.callback);
			}
		}
	}
	else if (action == GLFW_RELEASE)
	{
		switch (key)
		{
			case (GLFW_KEY_LEFT_SHIFT):
			buttonStates.shiftDown = false;
			break;
			case (GLFW_KEY_LEFT_CONTROL):
			buttonStates.ctrlDown = false;
			break;
			case (GLFW_KEY_LEFT_ALT):
			buttonStates.altDown = false;
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
			buttonStates.lmbDown = false;
			break;
			case (GLFW_MOUSE_BUTTON_RIGHT):
			buttonStates.rmbDown = false;
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