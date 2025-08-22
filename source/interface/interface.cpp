#include "interface.h"
#include "start_page_gui.h"
#include "main_menu_bar/main_menu_bar_gui.h"
#include "view_tab_bar_gui.h"
#include "edit_history_gui.h"
#include "autosave_popup_gui.h"

void Interface::init(Window* windowManager, Input* input, TextureLoader& textureLoader) {
    m_windowManager = windowManager;
    m_input = input;
    m_guiTextures = std::make_unique<GuiTextures>(textureLoader);

    IMGUI_CHECKVERSION();
    // imgui setup
    imGui = ImGui::CreateContext();
    imGuiIO = &ImGui::GetIO();
    // TODO: Investigate if there is some better way than to put this line here randomly
    // Extra: This was added so that numeric inputs can use the locale's decimal point (either , or .)
    ImGui::GetPlatformIO().Platform_LocaleDecimalPoint = *localeconv()->decimal_point;
    // Disable automatically making an "imgui.ini" file in the working directory.
    imGuiIO->IniFilename = NULL;
    // Note that there is no actual saving rate.
    // This is just so imgui doesn't take 5 seconds after a change before it tells me that a change needs to be saved.
    imGuiIO->IniSavingRate = 0.1f;
    // Disable the (largely useless for this program) window nav with CTRL + TAB
    imGui->ConfigNavWindowingKeyNext = 0;
    imGui->ConfigNavWindowingKeyPrev = 0;
    // set up platform / renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(m_windowManager->getGlfwWindow(), true);
    ImGui_ImplOpenGL3_Init(windowManager->getGlslVersionString().c_str());
    // Load fonts
    m_styleHandler.loadFontSizes("./fonts/Noto_Sans_Mono/NotoSansMono-VariableFont.ttf");

    // ADD GUIS
    addGui<StartPageGui>("StartPageGui");
    addGui<MainMenuBarGui>("MainMenuBarGui", m_styleHandler);
    addGui<ViewTabBarGui>("ViewTabBarGui");
    // simple popups
    addGui<AutosavePopupGui>("AutosavePopupGui");
#if DEBUG
    addGui<EditHistoryGui>("EditHistoryGui");
#endif
}

void Interface::initEventListeners(std::shared_ptr<PreferencesIO> preferencesIO) {
    if (preferencesIO) {
        preferencesIO->preferencesChangedEvent.addListener([&](Preferences preferences) {
            m_styleHandler.applyStyle(preferences.getStyle());
            m_styleHandler.setFontSize(preferences.getFontSize());
            m_preferences = preferences;
        });
    }
}

void Interface::applyDefaultStyle() {
    // Apply the default style
    m_styleHandler.applyStyle(InterfaceStyleHandler::getDefaultStyle());
}

void Interface::addGui(std::shared_ptr<Gui> gui) {
    m_guis.insert({gui->getID(), gui});
}

void Interface::draw(float deltaTime) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    m_input->setGuiWantKeyboard(imGuiIO->WantCaptureKeyboard);

    m_styleHandler.transitionStyle(deltaTime);

    // Apply font
    ImGui::PushFont(m_styleHandler.getFontData(m_styleHandler.getFontSize()));
    // Create GuiDrawArgs to pass
    GuiDrawArgs drawArgs = GuiDrawArgs(m_windowManager->getSize(), *m_input, *m_guiTextures.get(), m_preferences);
    for (auto& [id, gui] : m_guis) {
        if (gui->getVisible()) {
            gui->draw(drawArgs);
        }
    }

    guiHovered = imGuiIO->WantCaptureMouse;
    guiFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow);

#if defined(DEBUG) || defined(_DEBUG)
    ImGui::ShowDemoWindow();
#endif
    ImGui::PopFont();
    ImGui::Render();

    // check for right / middle click defocus
    if (imGui->HoveredWindow == NULL && imGui->NavWindow != NULL &&
        (imGuiIO->MouseClicked[1] ||
         imGuiIO->MouseClicked[2]) /* could cause issues, who cares? && GetFrontMostPopupModal() == NULL*/)
    {
        ImGui::FocusWindow(NULL);
    }

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}