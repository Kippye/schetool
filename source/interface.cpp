#include "imgui.h"
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

    addGUI(*(new ScheduleGui("ScheduleGUI", m_schedule)));
}

void Interface::addGUI(Gui& gui)
{
    m_guis.insert({gui.getID(), &gui});
}

void Interface::draw()
{
	guiWantKeyboard = false;

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// TEMP
	ImGui::Begin("IOButtonWindow", NULL);
		if (ImGui::Button("Save"))
		{
			m_ioHandler->writeSchedule("C:\\Users\\Remoa\\Documents\\Devil\\schetool\\test.blf");
		}
		if (ImGui::Button("Load"))
		{
			m_ioHandler->readSchedule("C:\\Users\\Remoa\\Documents\\Devil\\schetool\\test.blf");
		}
	ImGui::End();

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

// TODO: can't this just be its own Gui too?
void Interface::openFileDialog(GUI_PROMPT type)
{
    return;
}

void Interface::checkFileDialog()
{
    return;
}
