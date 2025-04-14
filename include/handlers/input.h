#pragma once

#include "window.h"
#include <map>
#include <vector>
#include <format>

enum INPUT_EVENT {
    INPUT_EVENT_SC_RENAME,
    INPUT_EVENT_SC_NEW,
    INPUT_EVENT_SC_SAVE,
    INPUT_EVENT_SC_OPEN,
    INPUT_EVENT_SC_UNDO,
    INPUT_EVENT_SC_REDO,
};

struct InputShortcut {
        INPUT_EVENT event;
        int key;
        bool ctrlRequired = false;
        bool altRequired = false;
        bool shiftRequired = false;

        bool hasRequiredMods(bool ctrlDown, bool altDown, bool shiftDown) const {
            return (ctrlDown == ctrlRequired && altDown == altRequired && shiftDown == shiftRequired);
        }
        // Return a string in the format "(CTRL) + (ALT) + (SHIFT) + KEY"
        std::string getShortcutString() const {
            auto keyName = glfwGetKeyName(key, glfwGetKeyScancode(key));
            if (keyName == NULL) {
                return "N/A";
            }

            std::string keyNameString = std::string(keyName);
            // Convert key name to uppercase. NOTE: ASCII-Only!
            std::transform(keyNameString.begin(), keyNameString.end(), keyNameString.begin(), ::toupper);

            return std::format("{}{}{}{}",
                               ctrlRequired ? "CTRL+" : "",
                               altRequired ? "ALT+" : "",
                               shiftRequired ? "SHIFT+" : "",
                               keyNameString);
        }
};

struct ButtonStates {
        bool ctrlDown = false, shiftDown = false, altDown = false, lmbDown = false, rmbDown = false, lmbDownLast = false,
             rmbDownLast = false;
};

class Input {
    private:
        Window* m_windowManager;
        bool m_guiWantKeyboard = false;
        bool m_firstMouseMovement = true;
        std::vector<InputShortcut> m_shortcuts = {
            InputShortcut{INPUT_EVENT_SC_RENAME, GLFW_KEY_R, true},
            InputShortcut{INPUT_EVENT_SC_NEW, GLFW_KEY_N, true},
            InputShortcut{INPUT_EVENT_SC_SAVE, GLFW_KEY_S, true},
            InputShortcut{INPUT_EVENT_SC_OPEN, GLFW_KEY_O, true},
            InputShortcut{INPUT_EVENT_SC_UNDO, GLFW_KEY_Z, true},  // CTRL + Z
            InputShortcut{INPUT_EVENT_SC_REDO, GLFW_KEY_Z, true, false, true},  // CTRL + SHIFT + Z
            InputShortcut{INPUT_EVENT_SC_REDO, GLFW_KEY_Y, true},  // CTRL + Y
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
        // Get a vector of all InputShortcuts that activate the given INPUT_EVENT
        std::vector<InputShortcut> getEventShortcuts(INPUT_EVENT event) const;
        // Get a vector of shortcut strings for each InputShortcut in the given vector.
        static std::vector<std::string> getShortcutStrings(const std::vector<InputShortcut>& shortcuts);

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
