#include "view_tab_bar_gui.h"
#include "main_menu_bar/main_menu_bar_gui.h"
#include "gui_templates.h"

float ViewTabBarGui::height = 0.0f;

ViewTabBarGui::ViewTabBarGui(const char* ID) : Gui(ID) {
}

void ViewTabBarGui::draw(GuiDrawArgs& args) {
    // View switch shortcuts
    {
        ScheduleView newView = m_selectedTab;
        if (args.input.getEventInvokedLastFrame(INPUT_EVENT_SC_VIEW_NEXT)) {
            newView = (ScheduleView)(m_selectedTab + 1);
        }
        if (args.input.getEventInvokedLastFrame(INPUT_EVENT_SC_VIEW_PREV)) {
            // Remove 1 from current view enum or (if the current view == 0) the last view enum
            newView = (ScheduleView)((m_selectedTab == (ScheduleView)0 ? ScheduleView::Last : m_selectedTab) - 1);
        }
        // Wrap view to valid range
        if (newView >= ScheduleView::Last) {
            newView = (ScheduleView)0;
        }
        if (m_selectedTab != newView) {
            viewSwitched.invoke(newView);
        }
        m_selectedTab = newView;
    }

    ImGuiStyle style = ImGui::GetStyle();
    ImFont* dateSelectorFontData = InterfaceStyleHandler::getFontData(
        InterfaceStyleHandler::getFontSize() == FontSize::Large ? FontSize::Large
                                                                : (FontSize)((int)InterfaceStyleHandler::getFontSize() + 1));

    ImGui::PushFont(dateSelectorFontData);
    ImVec2 dateButtonSize = ImGui::CalcTextSize("TEST") + style.FramePadding * 2.0f;
    ImGui::PopFont();
    ImVec2 tabSize = ImGui::TabItemCalcSize("TAB", false);
    ImGui::SetNextWindowSize(ImVec2((float)args.windowSize.getWidth(), dateButtonSize.y));
    ImGui::SetNextWindowPos(ImVec2(0.0f, MainMenuBarGui::getHeight()));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(gui_style_vars::windowEdgePadding.x, 0.0f));
    if (ImGui::Begin(m_ID.c_str(),
                     NULL,
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                         ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus))
    {
        ImGui::PushStyleVar(ImGuiStyleVar_TabBarBorderSize, 0.0f);
        ImGui::SetCursorPosY(ImGui::GetWindowHeight() - tabSize.y);
        if (ImGui::BeginTabBar("ViewTabBar", ImGuiTabBarFlags_DrawSelectedOverline)) {
            if (ImGui::BeginTabItem(
                    "Table",
                    NULL,
                    m_forceTabSelectedIndex.has_value() && m_forceTabSelectedIndex.value() == ScheduleView::Table
                        ? ImGuiTabItemFlags_SetSelected
                        : 0))
            {
                ImGui::EndTabItem();
            }
            if (ImGui::IsItemClicked()) {
                ScheduleView newSelectedTab = ScheduleView::Table;
                if (m_selectedTab != newSelectedTab) {
                    viewSwitched.invoke(newSelectedTab);
                }
                m_selectedTab = newSelectedTab;
            }

            if (ImGui::BeginTabItem(
                    "Calendar",
                    NULL,
                    m_forceTabSelectedIndex.has_value() && m_forceTabSelectedIndex.value() == ScheduleView::Calendar
                        ? ImGuiTabItemFlags_SetSelected
                        : 0))
            {
                ImGui::EndTabItem();
            }
            if (ImGui::IsItemClicked()) {
                ScheduleView newSelectedTab = ScheduleView::Calendar;
                if (m_selectedTab != newSelectedTab) {
                    viewSwitched.invoke(newSelectedTab);
                }
                m_selectedTab = newSelectedTab;
            }
            m_forceTabSelectedIndex.reset();
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
        const float resetButtonSize = 16.0f;
        if (m_viewedDateOverride.getIsEmpty() == false) {
            ImGui::SameLine();
            ImGui::SetCursorScreenPos(ImVec2(
                ImGui::GetCursorScreenPos().x,
                ImGui::GetItemRectMin().y + ImGui::GetItemRectSize().y / 2.0f - resetButtonSize / 2.0f - style.FramePadding.y));
            if (gui_templates::ImageButtonStyleColored("##ResetToTodayButton",
                                                       args.guiTextures.getOrLoad("icon_undo").ImID,
                                                       ImVec2(resetButtonSize, resetButtonSize)))
            {
                clearDateOverride();
            }
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

void ViewTabBarGui::setSelectedView(ScheduleView view) {
    m_selectedTab = view;
    m_forceTabSelectedIndex = view;
}

void ViewTabBarGui::clearDateOverride() {
    m_viewedDateOverride.clear();
    viewedDateChanged.invoke(m_viewedDateOverride);
}

float ViewTabBarGui::getHeight() {
    return height;
}  // STATIC