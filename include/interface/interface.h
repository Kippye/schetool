#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <window.h>
#include <input.h>
#include <gui.h>

#include <string>
#include <map>
#include <memory>

class Interface
{
	private:
		Window* m_windowManager;
		Input* m_input;
		std::map<std::string, std::shared_ptr<Gui>> m_guis = {};

		void addGui(std::shared_ptr<Gui> gui);
	public:
		bool guiFocused = false, guiHovered = false, guiWantKeyboard = false;

		ImGuiIO* imGuiIO;
		ImGuiContext* imGui;

		void init(Window*, Input*);
        template <typename T, typename... Args>
        std::shared_ptr<T> addGui(Args&&... args)
        {
            std::shared_ptr<T> gui = std::make_shared<T>(std::forward<Args>(args)...);
            addGui(gui);
            return gui;
        }
        template <typename T>
        std::shared_ptr<T> getGuiByID(const std::string& ID)
        {
            static_assert(std::is_base_of_v<Gui, T>, "Interface::getGuiByID<T>: Provided type must derive from Gui!");
            if (m_guis.find(ID) == m_guis.end())
            {
                printf("Interface::getGuiByID(ID: %s): Failed to get a Gui with the ID. Returning shared_ptr to nullptr\n", ID.c_str());
                return std::shared_ptr<T>(nullptr);
            }
            return std::dynamic_pointer_cast<T>(m_guis.at(ID));
        }
		void draw();
};
