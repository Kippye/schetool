#include <iostream>
#include "interface.h"
#include "start_page_gui.h"
#include "main_menu_bar_gui.h"
#include "edit_history_gui.h"
#include "autosave_popup_gui.h"

void Interface::init(Window* windowManager, Input* input, TextureLoader& textureLoader)
{
	m_windowManager = windowManager;
	m_input = input;
    m_guiTextures = std::make_unique<GuiTextures>(textureLoader);

    IMGUI_CHECKVERSION();
	// imgui setup
	imGui = ImGui::CreateContext();
	imGuiIO = &ImGui::GetIO();
	// Disable automatically making an "imgui.ini" file in the working directory.
	imGuiIO->IniFilename = NULL;
	// set up platform / renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(m_windowManager->window, true);
	ImGui_ImplOpenGL3_Init(windowManager->getGlslVersionString().c_str());
    // Load fonts
    m_styleHandler->loadFontSizes("./fonts/Noto_Sans_Mono/NotoSansMono-VariableFont.ttf");

    // Apply the default style
    setStyle(m_styleHandler->getDefaultStyle());

	// ADD GUIS
    addGui<StartPageGui>("StartPageGui");
	auto mainMenuBarGui = addGui<MainMenuBarGui>("MainMenuBarGui", m_styleHandler);
    mainMenuBarGui->setGuiStyleEvent.addListener([&](GuiStyle style) { setStyle(style); });
    mainMenuBarGui->setFontScaleEvent.addListener([&](FontSize fontScale) { m_styleHandler->setFontSize(fontScale); });
    // simple popups
    addGui<AutosavePopupGui>("AutosavePopupGui");
	#if DEBUG
	addGui<EditHistoryGui>("EditHistoryGui");
	#endif
}

void Interface::setStyle(GuiStyle style)
{
    m_styleHandler->applyStyle(style);
}

GuiStyle Interface::getCurrentStyle() const
{
    return m_styleHandler->getCurrentStyle();
}

void Interface::addGui(std::shared_ptr<Gui> gui)
{
    m_guis.insert({gui->getID(), gui});
}

void Interface::draw()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	m_input->setGuiWantKeyboard(imGuiIO->WantCaptureKeyboard);

    // Apply font
    ImGui::PushFont(m_styleHandler->getFontData(m_styleHandler->getFontSize()));
    for (auto &id_gui : m_guis)
    {
        id_gui.second->draw(*m_windowManager, *m_input, *m_guiTextures.get());
    }

	guiHovered = imGuiIO->WantCaptureMouse;
	guiFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow);

    #ifdef DEBUG
	ImGui::ShowDemoWindow();
    #endif
    ImGui::PopFont();
	ImGui::Render();
	
	// check for right / middle click defocus
	if (imGui->HoveredWindow == NULL && imGui->NavWindow != NULL && (imGuiIO->MouseClicked[1] || imGuiIO->MouseClicked[2]) /* could cause issues, who cares? && GetFrontMostPopupModal() == NULL*/)
	{
		ImGui::FocusWindow(NULL);
	}

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}