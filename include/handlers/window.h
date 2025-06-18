#pragma once

extern "C" {
#include <glad.h>
}
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <functional>
#include <string>
#include <map>
#include "window_size.h"
#include "textures.h"
#include "event.h"

enum CURSOR_TYPE {
    NORMAL,
    DRAW
};

class Window {
    private:
        size_t m_windowWidth = 800;
        size_t m_windowHeight = 600;
        static std::map<std::string, std::string> m_versionGlToGLSL;
        std::string m_glVersionString = "";
        std::string m_glslVersionString = "#version 140";
        std::string m_titleBase = "";
        std::string m_title = "";
        // window events
        std::function<void(Window*, int, int)> framebuffer_size_callback;
        std::function<void(Window*, int)> window_focus_callback;
        std::function<void(Window*)> window_close_callback;
        const size_t WINDOW_MIN_WIDTH = 220;
        const size_t WINDOW_MIN_HEIGHT = 160;

    public:
        GLFWwindow* window;
        bool firstMouseMovement = true;
        bool hasFocus = true;
        bool shouldClose = false;
        Event<> windowCloseEvent;

        std::string getGlVersionString() const;
        std::string getGlslVersionString() const;
        // Window functions
        // Get window size as a readonly class
        WindowSize getSize() const;
        // Load and set the window's icon
        void loadIcon(TextureLoader& textureLoader);
        void setTitle(std::string title);
        void setTitleSuffix(const std::string& suffix);
        std::string getTitle() const;
        // Lifecycle functions
        void init();
        // Call the windowCloseEvent, destroy the window and terminate glfw.
        void terminate();

        // input events
        std::function<void(Window*, int, int, int, int)> key_callback;
        std::function<void(Window*, int, int, int)> mouse_button_callback;
        std::function<void(Window*, double, double)> scroll_callback;
        std::function<void(Window*, double, double)> cursor_pos_callback;
};
