#pragma once
#include <glm/glm.hpp>
#include <string>
#include <window.h>
#include <input.h>

class Window;
class Input;

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