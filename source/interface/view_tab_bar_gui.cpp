#include "view_tab_bar_gui.h"
#include "main_menu_bar/main_menu_bar_gui.h"
#include "gui_templates.h"
// TEMP
#include <iostream>

float ViewTabBarGui::height = 0.0f;

ViewTabBarGui::ViewTabBarGui(const char* ID) : Gui(ID) {
}

void ViewTabBarGui::draw(const WindowSize& windowSize, Input& input, GuiTextures& guiTextures) {
    ImGuiStyle style = ImGui::GetStyle();
    ImFont* dateSelectorFontData = InterfaceStyleHandler::getFontData(
        InterfaceStyleHandler::getFontSize() == FontSize::Large ? FontSize::Large
                                                                : (FontSize)((int)InterfaceStyleHandler::getFontSize() + 1));

    ImGui::PushFont(dateSelectorFontData);
    ImVec2 dateButtonSize = ImGui::CalcTextSize("TEST") + style.FramePadding * 2.0f;
    ImGui::PopFont();
    ImVec2 tabSize = ImGui::TabItemCalcSize("TAB", false);
    ImGui::SetNextWindowSize(ImVec2((float)windowSize.getWidth(), dateButtonSize.y));
    ImGui::SetNextWindowPos(ImVec2(0.0f, MainMenuBarGui::getHeight()));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(style.WindowPadding.x, 0.0f));
    if (ImGui::Begin(m_ID.c_str(),
                     NULL,
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                         ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus))
    {
        ImGui::PushStyleVar(ImGuiStyleVar_TabBarBorderSize, 0.0f);
        ImGui::SetCursorPosY(ImGui::GetWindowHeight() - tabSize.y);
        if (ImGui::BeginTabBar("ViewTabBar", ImGuiTabBarFlags_DrawSelectedOverline)) {
            if (ImGui::BeginTabItem("Table")) {
                ScheduleView newSelectedView = ScheduleView::Table;
                if (m_selectedView != newSelectedView) {
                    viewSwitched.invoke(newSelectedView);
                }
                m_selectedView = newSelectedView;
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Calendar")) {
                ScheduleView newSelectedView = ScheduleView::Calendar;
                if (m_selectedView != newSelectedView) {
                    viewSwitched.invoke(newSelectedView);
                }
                m_selectedView = newSelectedView;
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        ImGui::PopStyleVar();
        ImGui::SameLine();
        // Current date text
        const TimeWrapper& currentDate =
            m_viewedDateOverride.getIsEmpty() == false ? m_viewedDateOverride : TimeWrapper::getCurrentTime();
        const std::string_view currentDateFmt = currentDate.getMonthDayUTC() < 10 ? "{:%A,%e. %B %Y}" : "{:%A, %e. %B %Y}";
        std::string viewedDateText = m_viewedDateOverride.getIsEmpty() == true
            ? currentDate.getDynamicFmtString(currentDateFmt)
            : currentDate.getDynamicFmtStringUTC(currentDateFmt);
        ImGui::PushFont(dateSelectorFontData);
        ImGui::SetCursorPos(ImVec2(
            ImGui::GetWindowWidth() / 2.0f - gui_size_calculations::getTextButtonWidth(viewedDateText.c_str()) / 2.0f, 0.0f));
        auto currentTimeUTC = TimeWrapper::getCurrentTime().getTimeUTC();
        ImGui::PushStyleVar(
            ImGuiStyleVar_Alpha,
            m_viewedDateOverride.getIsEmpty() ? 1.0f
                                              : 0.25f +
                    std::abs(std::sin(std::chrono::milliseconds(std::chrono::floor<std::chrono::milliseconds>(currentTimeUTC) -
                                                                std::chrono::floor<std::chrono::days>(currentTimeUTC))
                                          .count() /
                                      800.f)));
        if (ImGui::Button(std::format("{}##ScheduleViewDateButton", viewedDateText).c_str())) {
            m_openDateSelectPopup = true;
        }
        ImGui::PopStyleVar();
        ImGui::PopFont();
        // Show a reset button when viewing a different date
        // Preset button size because it uses a preset size texture variant
        const float resetButtonSize =
            24.0f;  //ImGui::CalcItemSize(ImVec2(0, 0), labelSize.x + style.ItemInnerSpacing.x * 2.0f, labelSize.y + style.ItemInnerSpacing.y * 2.0f).y;
        if (m_viewedDateOverride.getIsEmpty() == false) {
            ImGui::SameLine();
            ImGui::SetCursorScreenPos(
                ImVec2(ImGui::GetCursorScreenPos().x,
                       ImGui::GetItemRectMin().y + ImGui::GetItemRectSize().y / 2.0f - resetButtonSize / 2.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2());
            if (gui_templates::ImageButtonStyleColored("##ResetToTodayButton",
                                                       guiTextures.getOrLoad("icon_reset_24px").ImID,
                                                       ImVec2(resetButtonSize, resetButtonSize)))
            {
                clearDateOverride();
            }
            ImGui::PopStyleVar();
        }
    }
    ImGui::PopStyleVar(2);
    height = ImGui::GetWindowHeight();
    ImGui::End();

    // Viewed date selector popup
    if (ImGui::BeginPopup("Schedule Date Selector")) {
        // Display date editor to edit m_viewedDateOverride.
        // If the current date was selected, just clear m_viewedDateOverride again.
        if (gui_templates::DateEditor(m_viewedDateOverride, m_dateSelectorYear, m_dateSelectorMonth)) {
            if (m_viewedDateOverride.getDateUTC() == TimeWrapper::getCurrentTime().getLocalDate()) {
                clearDateOverride();
            } else {
                viewedDateChanged.invoke(m_viewedDateOverride);
            }
        }
        ImGui::EndPopup();
    }
    if (m_openDateSelectPopup) {
        TimeWrapper currentTime = TimeWrapper::getCurrentTime();
        m_dateSelectorYear = currentTime.getYearUTC();
        m_dateSelectorMonth = currentTime.getMonthUTC();
        ImGui::OpenPopup("Schedule Date Selector");
        m_openDateSelectPopup = false;
    }
}

void ViewTabBarGui::clearDateOverride() {
    m_viewedDateOverride.clear();
    viewedDateChanged.invoke(m_viewedDateOverride);
}

float ViewTabBarGui::getHeight() {
    return height;
}  // STATIC