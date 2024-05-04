#include <iostream>
#include <interface.h>
#include <schedule_gui.h>
#include <main_menu_bar_gui.h>
#include <edit_history_gui.h>

void Interface::init(Window* windowManager, Input* input)
{
	m_windowManager = windowManager;
	m_input = input;

    IMGUI_CHECKVERSION();
	// imgui setup
	imGui = ImGui::CreateContext();
	imGuiIO = &ImGui::GetIO();
	// set up platform / renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(m_windowManager->window, true);
	ImGui_ImplOpenGL3_Init("#version 430");
	imGuiIO->Fonts->AddFontFromFileTTF("./fonts/Noto_Sans_Mono/NotoSansMono-VariableFont.ttf", 16.0f);

	// ADD GUIS
	
	addGui(std::shared_ptr<MainMenuBarGui>(new MainMenuBarGui("MainMenuBarGui")));
    addGui(std::shared_ptr<ScheduleGui>(new ScheduleGui("ScheduleGui")));
	#if DEBUG
	//addGui(std::shared_ptr<EditHistoryGui>(new EditHistoryGui("EditHistoryGui", m_schedule)));
	#endif
}

void Interface::addGui(std::shared_ptr<Gui> gui)
{
    m_guis.insert({gui->getID(), gui});
}

// NOTE: Returns nullptr if the Gui was not found
std::shared_ptr<Gui> Interface::getGuiByID(const std::string& ID)
{
	// No Gui with that ID
	if (m_guis.find(ID) == m_guis.end())
	{
		printf("Interface::getGuiByID(ID: %s): No Gui found with the provided ID. Returning nullptr.\n", ID.c_str());
		return nullptr;
	}
	else
	{
		return m_guis.at(ID);
	}
}

void Interface::draw()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	m_input->setGuiWantKeyboard(imGuiIO->WantCaptureKeyboard);

    for (auto &id_gui : m_guis)
    {
        id_gui.second->draw(*m_windowManager, *m_input);
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

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}