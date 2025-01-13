#pragma once

#include "window.h"
#include <map>
#include <vector>

enum INPUT_EVENT
{
	INPUT_EVENT_SC_RENAME,
	INPUT_EVENT_SC_NEW,
	INPUT_EVENT_SC_SAVE,
	INPUT_EVENT_SC_OPEN,
	INPUT_EVENT_SC_UNDO,
	INPUT_EVENT_SC_REDO,
};

struct InputShortcut
{
	INPUT_EVENT event;
	int key;
	bool ctrlRequired;
	bool altRequired;
	bool shiftRequired;

	bool hasRequiredMods(bool ctrlDown, bool altDown, bool shiftDown) const
	{
		return (ctrlDown == ctrlRequired && altDown == altRequired && shiftDown == shiftRequired);
	}
};

struct ButtonStates
{
	bool ctrlDown, 
	shiftDown, 
	altDown, 
	lmbDown, 
	rmbDown, 
	lmbDownLast, 
	rmbDownLast;
};

class Input
{
	private:
		Window* m_windowManager;
		bool m_guiWantKeyboard;
		bool m_firstMouseMovement = true;
		std::vector<InputShortcut> m_shortcuts =
		{
			InputShortcut{INPUT_EVENT_SC_RENAME, GLFW_KEY_F2, true},
			InputShortcut{INPUT_EVENT_SC_NEW, GLFW_KEY_N, true},
			InputShortcut{INPUT_EVENT_SC_SAVE, GLFW_KEY_S, true},
			InputShortcut{INPUT_EVENT_SC_OPEN, GLFW_KEY_O, true},
			InputShortcut{INPUT_EVENT_SC_UNDO, GLFW_KEY_Z, true},
			InputShortcut{INPUT_EVENT_SC_REDO, GLFW_KEY_Y, true},
		};
		std::map<INPUT_EVENT, std::vector<std::function<void()>>> m_listeners = {};
		std::map<INPUT_EVENT, bool> m_eventStates = {};
		std::map<INPUT_EVENT, bool> m_eventLastFrameStates = {};
	public:
        double mousePosX = 0, mousePosY = 0;
        double mouseMoveX = 0, mouseMoveY = 0;
		ButtonStates buttonStates;
		const float mouseRepeatDelay = 0.05f;

		void init(Window*);
		void processInput(GLFWwindow* window);

		void addEventListener(INPUT_EVENT callback, const std::function<void()>& listener);
        size_t getEventListenerCount(INPUT_EVENT callback);
		void invokeEvent(INPUT_EVENT callback);
		bool getEventInvokedLastFrame(INPUT_EVENT callback);

		void setGuiWantKeyboard(bool to);

		void key_event(GLFWwindow* window, int key, int scancode, int action, int mods);
		void mouse_button_event(GLFWwindow* window, int button, int action, int mods);
		void cursor_pos_event(GLFWwindow* window, double xPos, double yPos);
		void scroll_event(GLFWwindow* window, double xOffset, double yOffset);
};
