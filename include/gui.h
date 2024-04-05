#pragma once
#include <glm/glm.hpp>
#include <string>
#include <window.h>
#include <input.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

class Gui
{
    protected:
        std::string m_ID;
    public:
        Gui();
        Gui(const char* ID);

        glm::vec2 position;
        glm::vec2 size;
        bool visible;
        float opacity;

        std::string getID();
        virtual void draw(Window& window, Input& input);
};