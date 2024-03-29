#include "imgui.h"
#include <main_menu_bar_gui.h>
#include <iostream>
#include <interface.h>
#include <window.h>
#include <gui.h>
#include <schedule_gui.h>

void Interface::init(Window* windowManager, Schedule* schedule, IO_Handler* ioHandler)
{
	m_windowManager = windowManager;
	m_schedule = schedule;
	m_ioHandler = ioHandler;

    IMGUI_CHECKVERSION();
	// imgui setup
	imGui = ImGui::CreateContext();
	imGuiIO = &ImGui::GetIO();
	// set up platform / renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(m_windowManager->window, true);
	ImGui_ImplOpenGL3_Init("#version 430");
	imGuiIO->Fonts->AddFontFromFileTTF("./fonts/Noto_Sans_Mono/NotoSansMono-VariableFont.ttf", 16.0f);

	// ADD GUIS
    addGUI(*(new MainMenuBarGui("MainMenuBarGui", m_ioHandler)));
    addGUI(*(new ScheduleGui("ScheduleGui", m_schedule)));
}

void Interface::addGUI(Gui& gui)
{
    m_guis.insert({gui.getID(), &gui});
}

// NOTE: Returns nullptr if the Gui was not found
Gui* Interface::getGuiByID(const std::string& ID)
{
	// No Gui with that ID
	if (m_guis.find(ID) == m_guis.end())
	{
		return nullptr;
	}
	else
	{
		return m_guis.at(ID);
	}
}

void Interface::draw()
{
	guiWantKeyboard = false;

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

    for (auto &id_gui : m_guis)
    {
        id_gui.second->draw(*m_windowManager);
    }

	guiHovered = imGuiIO->WantCaptureMouse;
	guiFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow);

	//ImGui::ShowDemoWindow();
	ImGui::Render();
	
	// check for right / middle click defocus
	if (imGui->HoveredWindow == NULL && imGui->NavWindow != NULL && (imGuiIO->MouseClicked[1] || imGuiIO->MouseClicked[2]) /* could cause issues, who cares? && GetFrontMostPopupModal() == NULL*/)
	{
		ImGui::FocusWindow(NULL);
	}

	// TEMP here? change the Window's title
	if (m_schedule->getEditedSinceWrite())
	{
		// has to be changed to add *
		if (m_windowManager->title == std::string(m_windowManager->titleBase))
		{
			m_windowManager->setTitle(std::string(m_windowManager->titleBase).append(std::string(" *")).c_str());
		}
	}
	else
	{
		if (m_windowManager->title.c_str() != m_windowManager->titleBase)
		{
			m_windowManager->setTitle(m_windowManager->titleBase);
		}
	}

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Interface::openMainMenuBarScheduleNameModal()
{
	Gui* gui = getGuiByID("MainMenuBarGui");

	if (gui != nullptr)
	{
		((MainMenuBarGui*)gui)->openNewScheduleNameModal(NAME_PROMPT_NEW);
	}
}