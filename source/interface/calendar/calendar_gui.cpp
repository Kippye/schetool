#include "calendar/calendar_gui.h"
#include "main_menu_bar/main_menu_bar_gui.h"
#include "view_tab_bar_gui.h"

CalendarGui::CalendarGui(const char* ID) : Gui(ID) {
}

void CalendarGui::draw(const WindowSize& windowSize, Input& input, GuiTextures& guiTextures) {
    ImGuiStyle style = ImGui::GetStyle();

    const float offsetFromTop = MainMenuBarGui::getHeight() + ViewTabBarGui::getHeight();

    ImGui::SetNextWindowSize(ImVec2((float)windowSize.getWidth(), (float)windowSize.getHeight() - offsetFromTop));
    ImGui::SetNextWindowContentSize(ImVec2((float)windowSize.getWidth(), (float)windowSize.getHeight() - offsetFromTop) -
                                    style.WindowPadding * 2.0f);
    ImGui::SetNextWindowPos(ImVec2(0.0f, offsetFromTop));

    ImGui::Begin(m_ID.c_str(),
                 NULL,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);
    ImGui::End();
}