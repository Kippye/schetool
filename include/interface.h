#pragma once

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <GLFW/glfw3.h>

#include <window.h>
#include <io_handler.h>
#include <input.h>
#include <gui.h>
#include <schedule.h>

#include <string>
#include <map>
#include <memory>

class Interface
{
	private:
		Window* m_windowManager;
		Schedule* m_schedule;
		Input* m_input;
		IO_Handler* m_ioHandler;
		std::map<std::string, std::shared_ptr<Gui>> m_guis = {};
	public:
		bool guiFocused = false, guiHovered = false, guiWantKeyboard = false;

		ImGuiIO* imGuiIO;
		ImGuiContext* imGui;

	public:
		void init(Window*, Input*, Schedule*, IO_Handler*);
		void addGUI(std::shared_ptr<Gui> gui);
		std::shared_ptr<Gui> getGuiByID(const std::string& ID);
		void draw();
		void openMainMenuBarScheduleNameModal();
};
