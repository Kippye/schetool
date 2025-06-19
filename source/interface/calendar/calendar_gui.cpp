#include "calendar/calendar_gui.h"
#include "main_menu_bar/main_menu_bar_gui.h"

CalendarGui::CalendarGui(const char* ID) : Gui(ID) {
}

void CalendarGui::draw(const WindowSize& windowSize, Input& input, GuiTextures& guiTextures) {
    setVisible(false);
    ImGuiStyle style = ImGui::GetStyle();
    ImGui::SetNextWindowSize(ImVec2((float)windowSize.getWidth(), (float)windowSize.getHeight()));
    ImGui::SetNextWindowContentSize(ImVec2((float)windowSize.getWidth(), (float)windowSize.getHeight()) -
                                    style.WindowPadding * 2.0f);
    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));

    ImGui::Begin(m_ID.c_str(),
                 NULL,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);
    ImGui::End();
    // Add menu bar height as offset
    ImGui::SetCursorPosY(MainMenuBarGui::getHeight());
}