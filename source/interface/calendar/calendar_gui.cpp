#include "calendar/calendar_gui.h"
#include "main_menu_bar/main_menu_bar_gui.h"
#include "view_tab_bar_gui.h"
#include "gui_templates.h"
#include "util.h"
#include <format>

CalendarGui::CalendarGui(const char* ID) : Gui(ID) {
}

void CalendarGui::draw(const WindowSize& windowSize, Input& input, GuiTextures& guiTextures) {
    // If for whatever reason the TimeWrapper containing the viewed month + year becomes empty, fill it with the current date.
    if (m_viewedMonth.getIsEmpty()) {
        m_viewedMonth = TimeWrapper(TimeWrapper::getCurrentTime().getDateUTC());
    }

    ImGuiStyle& style = ImGui::GetStyle();

    const float offsetFromTop = MainMenuBarGui::getHeight() + ViewTabBarGui::getHeight();

    ImGui::SetNextWindowSize(ImVec2((float)windowSize.getWidth(), (float)windowSize.getHeight() - offsetFromTop));
    ImGui::SetNextWindowContentSize(ImVec2((float)windowSize.getWidth(), (float)windowSize.getHeight() - offsetFromTop) -
                                    style.WindowPadding * 2.0f);
    ImGui::SetNextWindowPos(ImVec2(0.0f, offsetFromTop));

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    if (ImGui::Begin(m_ID.c_str(),
                     NULL,
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                         ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus))
    {
        TimeWrapper formatTime;
        std::string monthName = m_viewedMonth.getDynamicFmtStringUTC("{:%B}");
        const float monthDropdownWidth = ImGui::CalcTextSize("September").x * 1.5f;
        ImGui::SetNextItemWidth(monthDropdownWidth);
        // Display the viewed month with a dropdown to set it to any month
        if (ImGui::BeginCombo("##CalendarViewedMonth", monthName.c_str(), ImGuiComboFlags_NoArrowButton)) {
            for (size_t i = 1; i <= 12; i++) {
                bool isSelected = i == m_viewedMonth.getMonth();

                formatTime.setMonthUTC(i);
                monthName = formatTime.getDynamicFmtStringUTC("{:%B}");
                if (ImGui::Selectable(monthName.c_str(), isSelected)) {
                    m_viewedMonth.setMonthUTC(i);
                }

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
        ImGui::SameLine();
        std::string yearText = m_viewedMonth.getDynamicFmtStringUTC("{:%Y}");
        if (ImGui::Button(yearText.c_str())) {
            // TODO: Edit it somehow
        }
        // MONTH SELECTION & INCREMENTING
        ImGui::SameLine();
        // Calculate offset from the right edge: Arrow + [ ] + Button + [] + Arrow + WindowSpacing
        const float todayButtonWidth = ImGui::CalcTextSize("Today").x * 1.5f;
        const float offsetFromRight =
            ImGui::GetFrameHeight() + todayButtonWidth + ImGui::GetFrameHeight() + style.ItemSpacing.x * 2.0f;
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() - offsetFromRight);
        if (ImGui::ArrowButton("##PreviousMonth", ImGuiDir_Left)) {
            m_viewedMonth.addMonths(-1);
        }
        ImGui::SameLine();
        ImGui::SetNextItemWidth(todayButtonWidth);
        if (ImGui::Button("Today##CalendarTodayButton")) {
            m_viewedMonth = TimeWrapper(TimeWrapper::getCurrentTime().getDateUTC());
        }
        ImGui::SameLine();
        if (ImGui::ArrowButton("##NextMonth", ImGuiDir_Right)) {
            m_viewedMonth.addMonths(1);
        }

        drawCalendarTable();
    }
    ImGui::PopStyleVar();  // WindowRounding = 0.0f
    ImGui::End();
}

void CalendarGui::drawWeekdayHeaders(float width) {
    // July of 2024 has days 1-7 being Monday - Sunday
    // TODO: Probably figure out something else if i ever want to add support for Sunday - Saturday weeks
    TimeWrapper formatTime = TimeWrapper(DateWrapper(2024, 7, 1));
    ImGui::PushStyleColor(ImGuiCol_Button, gui_colors::colorInvisible);
    for (size_t i = 1; i <= 7; i++) {
        std::string weekdayName = formatTime.getDynamicFmtStringUTC("{:%A}");
        // Can't fit full weekday name, must use abbreviation
        if (ImGui::CalcTextSize(weekdayName.c_str()).x > width) {
            weekdayName = formatTime.getDynamicFmtString("{:%a}");
        }
        gui_templates::TextWithBackground(ImVec2(width, 0.0f), "%s", weekdayName.c_str());
        if (i != 7) {
            ImGui::SameLine(0.0f, gui_sizes::date_editor::monthDayButtonSpacing.x);
            formatTime.addDays(1);
        }
    }
    ImGui::PopStyleColor();
}

void CalendarGui::drawCalendarTable() {
    drawWeekdayHeaders(ImGui::GetContentRegionAvail().x / 7.0f);
    ImGuiStyle& style = ImGui::GetStyle();

    // MONTH DAYS
    size_t dayIndex = 0;
    unsigned int daysInMonth = mytime::get_month_day_count(m_viewedMonth.getYearUTC(), m_viewedMonth.getMonthUTC());

    TimeWrapper firstOfTheMonth = TimeWrapper(m_viewedMonth.getDateUTC());
    firstOfTheMonth.setMonthDayUTC(1);
    // Day of the week that the first day of the month is
    int dayOfTheWeekFirst = firstOfTheMonth.getWeekdayUTC(WeekStart::Monday, Base::Zero);

    TimeWrapper lastOfTheMonth = TimeWrapper(m_viewedMonth.getDateUTC());
    lastOfTheMonth.setMonthDayUTC(daysInMonth);
    // Day of the week that the last day of the month is
    int dayOfTheWeekLast = lastOfTheMonth.getWeekdayUTC(WeekStart::Monday, Base::Zero);

    TimeWrapper formatTime;
    auto addCalendarDay = [&](int month, int dayDisplayNumber) {
        unsigned int pushedColorCount = 0;
        unsigned int pushedVarCount = 0;
        unsigned int calendarDayYear = m_viewedMonth.getYearUTC();
        // Calendar day is from the previous or next year
        if (m_viewedMonth.getMonthUTC() == 1 && month == 12) {
            calendarDayYear--;
        } else if (m_viewedMonth.getMonthUTC() == 12 && month == 1) {
            calendarDayYear++;
        }
        DateWrapper calendarDayDate = DateWrapper(calendarDayYear, month, dayDisplayNumber);
        // Highlight today's date
        if (calendarDayDate == TimeWrapper::getCurrentTime().getLocalDate()) {
            ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg,
                                   ImGui::GetColorU32(gui_color_calculations::getTableCellHighlightColor(
                                       style.Colors[ImGuiCol_WindowBg], style.Colors[ImGuiCol_Text])));
        }
        // Display days from other months as slightly darker
        if (month != m_viewedMonth.getMonthUTC()) {
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.65f);
            pushedVarCount++;
        }
        formatTime.setMonthUTC(month);
        std::string dayNumberText =
            dayDisplayNumber == 1 ? formatTime.getDynamicFmtStringUTC("{:%b} 1") : std::to_string(dayDisplayNumber);
        ImGui::Text("%s", dayNumberText.c_str());
        ImGui::PopStyleColor(pushedColorCount);
        ImGui::PopStyleVar(pushedVarCount);
        dayIndex++;
    };

    const float minSquareHeight = ImGui::GetContentRegionAvail().y / 6.0f;
    ImGuiTableFlags tableFlags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_Borders | ImGuiTableFlags_SizingStretchSame;
    ImGuiTableRowFlags rowFlags = ImGuiTableRowFlags_None;

    if (ImGui::BeginTable("CalendarTable", 7, tableFlags, ImGui::GetContentRegionAvail())) {
        // If i add the weekday text as part of the table:
        // ImGui::TableSetupScrollFreeze(7, 1);
        ImGui::TableNextRow(rowFlags, minSquareHeight);
        // Days from previous month
        for (size_t i = dayOfTheWeekFirst; i > 0; i--) {
            if (ImGui::TableGetColumnIndex() == 6) {
                ImGui::TableNextRow(rowFlags, minSquareHeight);
            }
            ImGui::TableSetColumnIndex(dayIndex % 7);
            int previousMonth = m_viewedMonth.getMonthUTC() == 1 ? 12 : m_viewedMonth.getMonthUTC() - 1;
            addCalendarDay(
                previousMonth,
                mytime::get_month_day_count(previousMonth < 12 ? m_viewedMonth.getYearUTC() : m_viewedMonth.getYearUTC() - 1,
                                            previousMonth) -
                    (i - 1));
        }
        // Days of the viewed month
        for (size_t i = 0; i < daysInMonth; i++) {
            if (ImGui::TableGetColumnIndex() == 6) {
                ImGui::TableNextRow(rowFlags, minSquareHeight);
            }
            ImGui::TableSetColumnIndex(dayIndex % 7);
            addCalendarDay(m_viewedMonth.getMonthUTC(), i + 1);
        }
        // Days from next month
        for (size_t i = 0; i < 6 - dayOfTheWeekLast; i++) {
            int nextMonth = m_viewedMonth.getMonthUTC() == 12 ? 1 : m_viewedMonth.getMonthUTC() + 1;
            if (ImGui::TableGetColumnIndex() == 6) {
                ImGui::TableNextRow(rowFlags, minSquareHeight);
            }
            ImGui::TableSetColumnIndex(dayIndex % 7);
            addCalendarDay(nextMonth, i + 1);
        }

        ImGui::EndTable();
    }
}