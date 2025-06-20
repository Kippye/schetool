#include "input.h"

void Input::init(Window* windowManager) {
    m_windowManager = windowManager;

    for (size_t i = 0; i <= INPUT_EVENT_SC_REDO; i++) {
        m_listeners.insert({(INPUT_EVENT)i, std::vector<std::function<void()>>{}});
        m_eventStates.insert({(INPUT_EVENT)i, false});
        m_eventLastFrameStates.insert({(INPUT_EVENT)i, false});
    }

    m_windowManager->key_callback = [this](auto self, int key, int scancode, int action, int mods) {
        this->key_event(self->window, key, scancode, action, mods);
    };

    m_windowManager->mouse_button_callback = [this](auto self, int button, int action, int mods) {
        this->mouse_button_event(self->window, button, action, mods);
    };

    m_windowManager->cursor_pos_callback = [this](auto self, double xPos, double yPos) {
        this->cursor_pos_event(self->window, xPos, yPos);
    };

    m_windowManager->scroll_callback = [this](auto self, double xOffset, double yOffset) {
        this->scroll_event(self->window, xOffset, yOffset);
    };
}

void Input::processInput(GLFWwindow* window) {
    for (auto event : m_eventLastFrameStates) {
        m_eventLastFrameStates.at(event.first) = m_eventStates.at(event.first);
        m_eventStates.at(event.first) = false;
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
    if (m_guiWantKeyboard) {
        return;
    }

    // reset mouse movement as it only updates when the mouse is ACTUALLY moved
    mouseMoveX = 0;
    mouseMoveY = 0;
}

std::vector<InputShortcut> Input::getEventShortcuts(INPUT_EVENT event) const {
    std::vector<InputShortcut> eventShortcuts = {};

    for (const InputShortcut& shortcut : m_shortcuts) {
        if (shortcut.event == event) {
            eventShortcuts.push_back(shortcut);
        }
    }
    return eventShortcuts;
}

std::vector<std::string> Input::getShortcutStrings(const std::vector<InputShortcut>& shortcuts) {
    std::vector<std::string> shortcutStrings = {};

    for (const InputShortcut& shortcut : shortcuts) {
        shortcutStrings.push_back(shortcut.getShortcutString());
    }
    return shortcutStrings;
}

void Input::addEventListener(INPUT_EVENT event, const std::function<void()>& listener) {
    if (listener) {
        m_listeners.at(event).push_back(listener);
    }
}

size_t Input::getEventListenerCount(INPUT_EVENT event) {
    return m_listeners.at(event).size();
}

void Input::invokeEvent(INPUT_EVENT event) {
    auto& listeners = m_listeners.at(event);

    for (auto& listener : listeners) {
        listener();
    }
    m_eventStates.at(event) = true;
}

bool Input::getEventInvokedLastFrame(INPUT_EVENT event) {
    return m_eventLastFrameStates.at(event);
}

void Input::setGuiWantKeyboard(bool to) {
    m_guiWantKeyboard = to;
}

void Input::key_event(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (m_guiWantKeyboard) {
            return;
        }
        for (const InputShortcut& shortcut : m_shortcuts) {
            if (key == shortcut.key &&
                shortcut.hasRequiredMods(buttonStates.ctrlDown, buttonStates.altDown, buttonStates.shiftDown))
            {
                invokeEvent(shortcut.event);
            }
        }
    } else if (action == GLFW_RELEASE) {
        switch (key) {
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

void Input::mouse_button_event(GLFWwindow* window, int key, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (key == GLFW_MOUSE_BUTTON_LEFT) {
            m_windowManager->hasFocus = true;
        } else if (key == GLFW_MOUSE_BUTTON_RIGHT) {
            m_windowManager->hasFocus = true;
        }
    } else if (action == GLFW_RELEASE) {
        switch (key) {
            case (GLFW_MOUSE_BUTTON_LEFT):
                buttonStates.lmbDown = false;
                break;
            case (GLFW_MOUSE_BUTTON_RIGHT):
                buttonStates.rmbDown = false;
                break;
        }
    }
}

void Input::cursor_pos_event(GLFWwindow* window, double xPos, double yPos) {
    if (m_windowManager->hasFocus) {
        mouseMoveX = xPos - mousePosX;
        mouseMoveY = yPos - mousePosY;
        mousePosX = xPos;
        mousePosY = yPos;
    }
}

void Input::scroll_event(GLFWwindow* window, double xOffset, double yOffset) {
    return;
}