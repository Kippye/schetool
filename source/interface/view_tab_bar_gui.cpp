#include "view_tab_bar_gui.h"
#include "main_menu_bar/main_menu_bar_gui.h"

ViewTabBarGui::ViewTabBarGui(const char* ID) : Gui(ID) {
}

void ViewTabBarGui::draw(const WindowSize& windowSize, Input& input, GuiTextures& guiTextures) {
    ImGuiStyle style = ImGui::GetStyle();

    ImVec2 tabItemSize = ImGui::TabItemCalcSize("TABITEM", false);
    ImGui::SetNextWindowSize(ImVec2((float)windowSize.getWidth(), tabItemSize.y));
    ImGui::SetNextWindowPos(ImVec2(0.0f, MainMenuBarGui::getHeight()));
    if (ImGui::Begin(m_ID.c_str(),
                     NULL,
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                         ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus))
    {
        if (ImGui::BeginTabBar("ViewTabBar", ImGuiTabBarFlags_DrawSelectedOverline)) {
            if (ImGui::BeginTabItem("Schedule")) {
                size_t newSelectedTab = 0;
                if (m_selectedTab != newSelectedTab) {
                    viewSwitched.invoke(newSelectedTab);
                }
                m_selectedTab = newSelectedTab;
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Calendar")) {
                size_t newSelectedTab = 1;
                if (m_selectedTab != newSelectedTab) {
                    viewSwitched.invoke(newSelectedTab);
                }
                m_selectedTab = newSelectedTab;
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
    }
    ImGui::End();
}