#include <iostream>
extern "C" {
#include <glad.h>
}
#include <GLFW/glfw3.h>

#include "window.h"
#include <generated/program_info.h>
#include <format>

std::map<std::string, std::string> Window::m_versionGlToGLSL = {
    {"2.0", "#version 110"},
    {"2.1", "#version 120"},
    {"3.0", "#version 130"},
    {"3.1", "#version 140"},
    {"3.2", "#version 150"},
    {"3.3", "#version 330 core"},
    {"4.0", "#version 400 core"},
    {"4.1", "#version 410 core"},
    {"4.2", "#version 410 core"},
    {"4.3", "#version 430 core"},
    {"4.4", "#version 440 core"},
    {"4.5", "#version 450 core"},
    {"4.6", "#version 460 core"},
};

void Window::init() {
    m_titleBase = std::format("{} {}{}",
                              program_info::PROGRAM_NAME,
                              program_info::ProgramVersion::getCurrent().getString(),
#ifdef DEBUG
                              " (DEBUG)"
#else
                              ""
#endif
    );

    int major, minor, rev;
    glfwGetVersion(&major, &minor, &rev);
    std::cout << "GLFW version " << std::format("{}.{}.{}", major, minor, rev) << std::endl;
    glfwInit();
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);  //might be needed for bigger monitors?
    glfwWindowHintString(GLFW_WAYLAND_APP_ID, "schetool");

    m_window = glfwCreateWindow(m_windowWidth, m_windowHeight, m_titleBase.c_str(), NULL, NULL);
    if (m_window == NULL) {
        std::cout << "GLFW window creation failed." << std::endl;
        const char* description;
        int code = glfwGetError(&description);
        if (code != GLFW_NO_ERROR) {
            std::cout << "Error code: " << code;
            if (description) {
                std::cout << ". Description: " << description;
            }
            std::cout << std::endl;
        }
        glfwTerminate();
        return;
    }

    setTitle(m_titleBase);
    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(0);
    glfwMaximizeWindow(m_window);
    glfwSetWindowSizeLimits(m_window, WINDOW_MIN_WIDTH, WINDOW_MIN_HEIGHT, GLFW_DONT_CARE, GLFW_DONT_CARE);
    glfwSetWindowUserPointer(m_window, this);
    // Load address of OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "GLAD initialization failed." << std::endl;
        window_close_callback(this);
        return;
    }
    auto glVersionString = glGetString(GL_VERSION);
    std::cout << "OpenGL version " << glVersionString << std::endl;
    std::string glVersionNormalString = std::string(reinterpret_cast<const char*>(glVersionString));
    // Get the first 3 characters (x.y) and get rid of the rest.
    m_glVersionString = glVersionNormalString.substr(0, 3);
    // Try to map the OpenGL version to its GLSL version
    if (m_versionGlToGLSL.count(m_glVersionString) != 0) {
        m_glslVersionString = m_versionGlToGLSL.at(m_glVersionString);
        std::cout << "Chose suitable GLSL version: " << m_glslVersionString << std::endl;
    } else {
        std::cout << "No mapping from OpenGL version string " << m_glVersionString
                  << " to GLSL version. Falling back to default GLSL version: " << m_glslVersionString << std::endl;
    }

    // Set up the viewport (xpos, ypos, w, h)
    int currentWidth, currentHeight;
    glfwGetWindowSize(m_window, &currentWidth, &currentHeight);
    m_windowWidth = currentWidth;
    m_windowHeight = currentHeight;
    glViewport(0, 0, m_windowWidth, m_windowHeight);

#define genericCallback(functionName)                                       \
    [](GLFWwindow* win, auto... args) {                                     \
        auto pointer = static_cast<Window*>(glfwGetWindowUserPointer(win)); \
        if (pointer->functionName)                                          \
            pointer->functionName(pointer, args...);                        \
    }

    // Window callbacks
    glfwSetFramebufferSizeCallback(m_window, genericCallback(framebuffer_size_callback));
    glfwSetWindowFocusCallback(m_window, genericCallback(window_focus_callback));
    glfwSetWindowCloseCallback(m_window, genericCallback(window_close_callback));
    // Input callbacks (not handled here)
    glfwSetKeyCallback(m_window, genericCallback(key_callback));
    glfwSetMouseButtonCallback(m_window, genericCallback(mouse_button_callback));
    glfwSetCursorPosCallback(m_window, genericCallback(cursor_pos_callback));
    glfwSetScrollCallback(m_window, genericCallback(scroll_callback));

    // Linking callback events
    this->framebuffer_size_callback = [this](auto self, int width, int height) {
        if (width > 0 && height > 0) {
            this->m_windowWidth = width;
            this->m_windowHeight = height;
            glViewport(0, 0, width, height);
        }
    };

    this->window_focus_callback = [](auto self, int focused) {
        if (focused) {
            self->giveFocus();
        } else {
            self->takeFocus();
        }
    };

    this->window_close_callback = [this](auto self) {
        this->m_shouldClose = true;
        this->windowCloseEvent.invoke();
    };
}

GLFWwindow* Window::getGlfwWindow() {
    return m_window;
}

std::string Window::getGlVersionString() const {
    return m_glVersionString;
}

std::string Window::getGlslVersionString() const {
    return m_glslVersionString;
}

WindowSize Window::getSize() const {
    return WindowSize(m_windowWidth, m_windowHeight);
}

void Window::loadIcon(TextureLoader& textureLoader) {
    GLFWimage images[1] = {GLFWimage()};
    images[0].pixels = textureLoader.loadTextureData(
        textureLoader.getRelativePathFromTextureFolder("icon.png"), &images[0].width, &images[0].height, nullptr, false);
    if (images[0].pixels) {
        glfwSetWindowIcon(m_window, 1, images);
        const char* desc;
        auto error = glfwGetError(&desc);
        if (error != GLFW_NO_ERROR) {
            std::cout << "GLFW error when setting window icon: " << error << ". Description: '" << desc << "'." << std::endl;
        }
    } else {
        std::cout << "Window::loadIcon(): Failed to load program icon from path: " << textureLoader.textureFolder << "icon.png"
                  << std::endl;
    }
}

void Window::setTitle(std::string title) {
    m_title = title;
    glfwSetWindowTitle(m_window, m_title.c_str());
}

void Window::setTitleSuffix(const std::string& suffix) {
    std::string newTitle = m_titleBase;
    newTitle.append(suffix);
    setTitle(newTitle);
}

std::string Window::getTitle() const {
    return m_title;
}

bool Window::getHasFocus() const {
    return m_hasFocus;
}

void Window::giveFocus() {
    m_hasFocus = true;
}

void Window::takeFocus() {
    m_hasFocus = false;
}

void Window::swapBuffers() {
    glfwSwapBuffers(m_window);
}

bool Window::getShouldClose() const {
    return m_shouldClose | glfwWindowShouldClose(m_window);
}

void Window::cancelClose() {
    m_shouldClose = false;
    glfwSetWindowShouldClose(m_window, GLFW_FALSE);
}

void Window::terminate() {
    std::cout << "Window::terminate(): Invoking windowCloseEvent and terminating GLFW window." << std::endl;
    windowCloseEvent.invoke();
    glfwDestroyWindow(m_window);
    glfwTerminate();
}
