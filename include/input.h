#pragma once

#include <glm/glm.hpp>
#include <window.h>
#include <map>
#include <vector>

enum INPUT_CALLBACK
{
	INPUT_CALLBACK_SC_RENAME,
	INPUT_CALLBACK_SC_NEW,
	INPUT_CALLBACK_SC_SAVE,
	INPUT_CALLBACK_SC_OPEN,
	INPUT_CALLBACK_SC_UNDO,
	INPUT_CALLBACK_SC_REDO,
};

struct InputShortcut
{
	INPUT_CALLBACK callback;
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
			InputShortcut{INPUT_CALLBACK_SC_RENAME, GLFW_KEY_F2, true},
			InputShortcut{INPUT_CALLBACK_SC_NEW, GLFW_KEY_S, true},
			InputShortcut{INPUT_CALLBACK_SC_SAVE, GLFW_KEY_S, true},
			InputShortcut{INPUT_CALLBACK_SC_OPEN, GLFW_KEY_O, true},
			InputShortcut{INPUT_CALLBACK_SC_UNDO, GLFW_KEY_Z, true},
			InputShortcut{INPUT_CALLBACK_SC_REDO, GLFW_KEY_Y, true},
		};
		std::map<INPUT_CALLBACK, std::vector<std::function<void()>>> m_callbacks = {};
		std::map<INPUT_CALLBACK, bool> m_callbackStates = {};
		std::map<INPUT_CALLBACK, bool> m_callbackLastFrameStates = {};
	public:
		glm::vec2 mousePos;
		glm::vec2 mouseMovement;
		ButtonStates buttonStates;
		const float mouseRepeatDelay = 0.05f;

		void init(Window*);
		void processInput(GLFWwindow* window);
		void addCallbackListener(INPUT_CALLBACK callback, std::function<void()>& listener);
		void invokeCallback(INPUT_CALLBACK callback);
		bool getCallbackInvokedLastFrame(INPUT_CALLBACK callback);

		void setGuiWantKeyboard(bool to);

		void key_event(GLFWwindow* window, int key, int scancode, int action, int mods);
		void mouse_button_event(GLFWwindow* window, int button, int action, int mods);
		void cursor_pos_event(GLFWwindow* window, double xPos, double yPos);
		void scroll_event(GLFWwindow* window, double xOffset, double yOffset);
};
