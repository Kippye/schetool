#pragma once

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <GLFW/glfw3.h>

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
	public:
		bool guiFocused = false, guiHovered = false, guiWantKeyboard = false;

		ImGuiIO* imGuiIO;
		ImGuiContext* imGui;

	public:
		void init(Window*, Input*);
		void addGui(std::shared_ptr<Gui> gui);
		std::shared_ptr<Gui> getGuiByID(const std::string& ID);
		void draw();
};
