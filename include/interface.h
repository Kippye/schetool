#pragma once

#include <io_handler.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <GLFW/glfw3.h>

#include <textures.h>
#include <window.h>
#include <gui.h>
#include <schedule.h>

#include <vector>
#include <string>
#include <map>

class Window;
class Gui;
class Schedule;
class IO_Handler;

enum GUI_STYLE
{
	DARK,
	LIGHT
};

enum GUI_PROMPT
{
	DIR,
	OPEN,
	SAVE,
	SAVE_AS
};

// NOTE: for now, gui tweens just work for opacity
struct GuiTween
{
	public:
		float
		start = 0.0f,
		end = 0.0f,
		progress = 0.0f,
		length = 0.0f,
		current = 0.0f;
		bool enabled = false;

	public:
		void Start(float _start, float _end, float _length, bool setToStart = false)
		{
			if (setToStart)
			{
				current = _start;
			}
			progress = 0.0f;
			start = _start > 0.0f ? _start : 0.0f;
			end = _end > 0.0f ? _end : 0.0f;
			length = _length >= 0.0f ? _length : 0.0f;
			enabled = true;
		}

		void Step(float _amount)
		{
			if (!enabled) { return; }

			progress = progress + _amount < length ? progress + _amount : length;

			// lil check here to avoid divide by 0
			if (length == 0) 
			{
				current = end;
			}
			else
			{
				if (start < end) { current = (end - start) * (progress / length); }
				else if (start > end) { current = (start - end) * (1 - (progress / length)); }
				else { current = end; }
			}

			if (progress == length)
			{
				enabled = false;
			}
		}
};

struct GuiData
{
	public:
		float f64_free = 0.1f,
		f64_snap = 1.0f;
		ImVec4 normalButtonColor;
		ImVec4 selectedButtonColor = ImVec4(0.00f, 0.51f, 1.00f, 1.0f);
		ImVec4 contentFolderColor = ImVec4(0.25f, 0.68f, 0.01f, 1.0f);
		ImVec4 buttonDisabled = ImVec4(0.50f, 0.02f, 0.02f, 1.0f);
		ImVec4 buttonDisabledHovered = ImVec4(0.63f, 0.06f, 0.06f, 1.0f);
		ImVec4 buttonDisabledActive = ImVec4(0.72f, 0.06f, 0.06f, 1.0f);
};

class Interface
{
	private:
		Window* m_windowManager;
		Schedule* m_schedule;
		// TEMP?
		IO_Handler* m_ioHandler;
		std::map<std::string, Gui*> m_guis = {};
	public:
		GuiData gd;
		GUI_STYLE guiStyle = DARK;
		bool guiFocused = false, guiHovered = false, guiWantKeyboard = false;
		std::map<std::string, unsigned int> guiTextures = {};
		std::map<std::string, GuiTween*> guiOpacities = 
		{
			// { std::string("Select by texture"), new GuiTween() }
		};

		ImGuiIO* imGuiIO;
		ImGuiContext* imGui;

	public:
		void init(Window*, Schedule*, IO_Handler*);
		void addGUI(Gui& gui);
		void draw();
		void openFileDialog(GUI_PROMPT type);
	private:
 		void checkFileDialog();
};
