#include "calendar/calendar_gui.h"
#include "main_menu_bar/main_menu_bar_gui.h"
#include "view_tab_bar_gui.h"
#include "gui_templates.h"
#include "util.h"
#include "filters/filter_rule.h"
#include "calendar/calendar_item_window_subgui.h"
#include <algorithm>
#include <format>

CalendarGui::CalendarGui(const char* ID, const ScheduleCore& scheduleCore, ScheduleEvents& scheduleEvents)
    : Gui(ID), m_scheduleCore(scheduleCore) {
    scheduleEvents.viewedDateChanged.addListener(viewedDateChangedListener);
    scheduleEvents.rowAdded.addListener(rowAddedListener);
    scheduleEvents.rowRemoved.addListener(rowRemovedListener);
    scheduleEvents.editUndone.addListener(editUndoneListener);
    scheduleEvents.editRedone.addListener(editRedoneListener);
    addSubGui(new CalendarItemWindowSubGui("CalendarItemWindowSubGui", scheduleCore));
    m_itemWindowSubGui = getSubGui<CalendarItemWindowSubGui>("CalendarItemWindowSubGui");

    // Add events from CalendarItemWindowSubGui to pipe through
    setElementValueBool.addEvent(m_itemWindowSubGui->setElementValueBool);
    setElementValueNumber.addEvent(m_itemWindowSubGui->setElementValueNumber);
    setElementValueDecimal.addEvent(m_itemWindowSubGui->setElementValueDecimal);
    setElementValueText.addEvent(m_itemWindowSubGui->setElementValueText);
    setElementValueSelect.addEvent(m_itemWindowSubGui->setElementValueSelect);
    setElementValueWeekday.addEvent(m_itemWindowSubGui->setElementValueWeekday);
    setElementValueTime.addEvent(m_itemWindowSubGui->setElementValueTime);
    setElementValueDate.addEvent(m_itemWindowSubGui->setElementValueDate);
    // column add / remove
    addDefaultColumn.addEvent(m_itemWindowSubGui->addDefaultColumn);
    removeColumn.addEvent(m_itemWindowSubGui->removeColumn);
    duplicateColumn.addEvent(m_itemWindowSubGui->duplicateColumn);
    // column modification
    setColumnType.addEvent(m_itemWindowSubGui->setColumnType);
    setColumnName.addEvent(m_itemWindowSubGui->setColumnName);
    setColumnResetOption.addEvent(m_itemWindowSubGui->setColumnResetOption);
    modifyColumnSelectOptions.addEvent(m_itemWindowSubGui->modifyColumnSelectOptions);
    // entire column modification
    resetColumn.addEvent(m_itemWindowSubGui->resetColumn);
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

        drawWeekdayHeaders(ImGui::GetContentRegionAvail().x / 7.0f);

        drawCalendarTable(guiTextures);
    }
    ImGui::PopStyleVar();  // WindowRounding = 0.0f
    ImGui::End();

    if (m_itemWindowSubGui) {
        m_itemWindowSubGui->draw(windowSize, input, guiTextures);
        if (m_openItemWindowAtRow.has_value()) {
            m_itemWindowSubGui->open(m_openItemWindowAtRow.value());
            m_openItemWindowAtRow.reset();
        }
    }
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

void CalendarGui::drawCalendarTable(GuiTextures& guiTextures) {
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

    const float minSquareHeight = ImGui::GetContentRegionAvail().y / 6.0f;
    ImGuiTableFlags tableFlags =
        ImGuiTableFlags_ScrollY | ImGuiTableFlags_Borders | ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_NoSavedSettings;
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
            m_currentTableCoords = {static_cast<size_t>(ImGui::TableGetColumnIndex()),
                                    static_cast<size_t>(ImGui::TableGetRowIndex())};
            drawCalendarDayContent(
                guiTextures,
                dayIndex,
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
            m_currentTableCoords = {static_cast<size_t>(ImGui::TableGetColumnIndex()),
                                    static_cast<size_t>(ImGui::TableGetRowIndex())};
            drawCalendarDayContent(guiTextures, dayIndex, m_viewedMonth.getMonthUTC(), i + 1);
        }
        // Days from next month
        for (size_t i = 0; i < 6 - dayOfTheWeekLast; i++) {
            int nextMonth = m_viewedMonth.getMonthUTC() == 12 ? 1 : m_viewedMonth.getMonthUTC() + 1;
            if (ImGui::TableGetColumnIndex() == 6) {
                ImGui::TableNextRow(rowFlags, minSquareHeight);
            }
            ImGui::TableSetColumnIndex(dayIndex % 7);
            m_currentTableCoords = {static_cast<size_t>(ImGui::TableGetColumnIndex()),
                                    static_cast<size_t>(ImGui::TableGetRowIndex())};
            drawCalendarDayContent(guiTextures, dayIndex, nextMonth, i + 1);
        }

        ImGui::EndTable();
    }
}

void CalendarGui::drawCalendarDayContent(GuiTextures& guiTextures, size_t& dayIndex, int month, int dayNumber) {
    ImGuiStyle& style = ImGui::GetStyle();
    unsigned int pushedVarCount = 0;
    unsigned int calendarDayYear = m_viewedMonth.getYearUTC();
    // Calendar day is from the previous or next year
    if (m_viewedMonth.getMonthUTC() == 1 && month == 12) {
        calendarDayYear--;
    } else if (m_viewedMonth.getMonthUTC() == 12 && month == 1) {
        calendarDayYear++;
    }
    DateWrapper calendarDayDate = DateWrapper(calendarDayYear, month, dayNumber);
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
    TimeWrapper calendarDayTime = TimeWrapper(calendarDayDate);
    std::string dayNumberText = dayNumber == 1 ? calendarDayTime.getDynamicFmtStringUTC("{:%b} 1") : std::to_string(dayNumber);
    ImGui::Text("%s", dayNumberText.c_str());
    const float calendayDayTextWidth = ImGui::GetItemRectSize().x;
    // + button to add a calendar item to this calendar day
    const ImVec2 label_size = ImGui::CalcTextSize("+");
    const float addItemButtonSize =
        ImGui::CalcItemSize(
            ImVec2(0.0f, 0.0f), label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f)
            .y;
    ImGui::SameLine(0.0f, ImGui::GetColumnWidth(ImGui::TableGetColumnIndex()) - calendayDayTextWidth - addItemButtonSize);
    // Adds a row and then sets its Date column value to this calendar day's date
    if (ImGui::Button(std::format("+##addCalendarItem{};{}", month, dayNumber).c_str(),
                      ImVec2(addItemButtonSize, addItemButtonSize)))
    {
        size_t rowsBefore = m_scheduleCore.getRowCount();
        addRow.invoke(m_scheduleCore.getRowCount());
        // The row was actually added. We can't be TOTALLY sure, but probably it was?
        if (m_scheduleCore.getRowCount() == rowsBefore + 1) {
            size_t dateColumnIndex = m_scheduleCore.getFlaggedColumnIndex(ScheduleColumnFlags_Date);
            setElementValueDate.invoke(dateColumnIndex, rowsBefore, DateContainer(calendarDayTime));
            // Also open the item window subgui for this item so it can be quickly edited
            m_openItemWindowAtRow = rowsBefore;
        }
    }
    drawCalendarDayItems(guiTextures, DateContainer(calendarDayTime));
    ImGui::PopStyleVar(pushedVarCount);
    dayIndex++;
}

void CalendarGui::drawCalendarDayItems(GuiTextures& guiTextures, const DateContainer& calendarDayDate) {
    ImGuiStyle& style = ImGui::GetStyle();
    const size_t dateColumnIndex = m_scheduleCore.getFlaggedColumnIndex(ScheduleColumnFlags_Date);
    const Column& dateColumn = m_scheduleCore.getColumnConst(dateColumnIndex);

    FilterRule<DateContainer> isThisDate = FilterRule<DateContainer>(calendarDayDate);
    std::vector<size_t> sortedRowIndices = m_scheduleCore.getSortedRowIndices();
    for (size_t unsortedRow = 0; unsortedRow < sortedRowIndices.size(); unsortedRow++) {
        size_t row = sortedRowIndices[unsortedRow];
        // CHECK FILTERS BEFORE DRAWING ITEM / ROW
        if (!m_scheduleCore.checkPassesAllFilters(row, m_scheduleDateOverride)) {
            continue;
        }
        // This row is NOT on the current calendar day date
        if (!isThisDate.checkPasses(m_scheduleCore.getElementConst(dateColumnIndex, row))) {
            continue;
        }

        std::string childLabelString = std::format("CalendarItem##{};{};{}",
                                                   row,
                                                   calendarDayDate.getTimeConst().getMonthUTC(),
                                                   calendarDayDate.getTimeConst().getMonthDayUTC());
        unsigned int pushedColorCount = 0;
        if (m_hoveredItemChildID.has_value() && m_hoveredItemChildID.value() == ImGui::GetID(childLabelString.c_str())) {
            ImGui::PushStyleColor(ImGuiCol_ChildBg, style.Colors[ImGuiCol_ButtonHovered]);
            pushedColorCount++;
            // The button is pressed but not dragging
            if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && !ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
                ImGui::PushStyleColor(ImGuiCol_ChildBg, style.Colors[ImGuiCol_ButtonActive]);
                pushedColorCount++;
            }
        } else {
            ImGui::PushStyleColor(ImGuiCol_ChildBg, style.Colors[ImGuiCol_WindowBg]);
            pushedColorCount++;
        }
        if (ImGui::BeginChild(childLabelString.c_str(), ImVec2(0, 0), ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_Borders)) {
            const size_t nameColumnIndex = m_scheduleCore.getFlaggedColumnIndex(ScheduleColumnFlags_Name);
            drawItemProperty({nameColumnIndex, row});

            // Remove item / row button (only visible while the item is hovered)
            if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem)) {
                const float removeButtonSize = ImGui::CalcTextSize("X").y;
                ImGui::SameLine(style.WindowPadding.x + ImGui::GetCurrentWindow()->ContentRegionRect.GetSize().x -
                                removeButtonSize);
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2());
                if (gui_templates::ImageButtonStyleColored(std::format("##RemoveCalendarItem{};{};{}",
                                                                       row,
                                                                       calendarDayDate.getTimeConst().getMonthUTC(),
                                                                       calendarDayDate.getTimeConst().getMonthDayUTC())
                                                               .c_str(),
                                                           guiTextures.getOrLoad("icon_remove").ImID,
                                                           ImVec2(removeButtonSize, removeButtonSize)))
                {
                    removeRow.invoke(row);
                    if (m_hoveredItemChildID == ImGui::GetCurrentWindow()->ChildId) {
                        m_hoveredItemChildID.reset();
                    }
                    // Skip drawing rest of the items for this calendar day
                    ImGui::PopStyleColor();
                    ImGui::PopStyleColor(pushedColorCount);
                    ImGui::PopStyleVar();
                    ImGui::EndChild();
                    return;
                }
                ImGui::PopStyleVar();
                ImGui::PopStyleColor();
            }

            for (size_t col = 0; col < m_scheduleCore.getColumnCount(); col++) {
                // The date doesn't need to be shown and the name has already been shown
                if (col == dateColumnIndex || col == nameColumnIndex) {
                    continue;
                }
                ScheduleColumnFlags columnFlags = m_scheduleCore.getColumnConst(col).flags;
                // Duration and End columns are ignored
                if ((columnFlags & ScheduleColumnFlags_Duration) || (columnFlags & ScheduleColumnFlags_End)) {
                    continue;
                }
                // Display the time as "Start - End", e.g. "11:00 - 13:30"
                if (columnFlags & ScheduleColumnFlags_Start) {
                    TimeContainer endTime = m_scheduleCore.getElementValueConstRef<TimeContainer>(
                        m_scheduleCore.getFlaggedColumnIndex(ScheduleColumnFlags_End), row);
                    std::string timeText =
                        std::format("{} - {}", m_scheduleCore.getElementConst(col, row)->getString(), endTime.getString());
                    ImGui::Text("%s", timeText.c_str());
                } else {
                    drawItemProperty({col, row});
                }
            }
            // The child window of this item is being hovered
            if (ImGui::IsWindowHovered()) {
                m_hoveredItemChildID = ImGui::GetCurrentWindow()->ChildId;
                // This child window was clicked -> open the item window for it next frame
                if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                    m_openItemWindowAtRow = row;
                }
            } else if (m_hoveredItemChildID == ImGui::GetCurrentWindow()->ChildId) {
                m_hoveredItemChildID.reset();
            }
        }
        ImGui::EndChild();
        ImGui::PopStyleColor(pushedColorCount);
    }
}

void CalendarGui::drawItemProperty(ScheduleCoordinates coords) {
    bool columnEditDisabled = false;
    const Column& column = m_scheduleCore.getColumnConst(coords.column());
    // If viewing a different date and the column has a reset option then show it disabled
    if (m_scheduleDateOverride.getIsEmpty() == false && column.resetOption != ColumnResetOption::Never) {
        columnEditDisabled = true;
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, gui_colors::disabledAlpha);
    }
    switch (column.type) {
        case (SCH_BOOL): {
            bool value = getElementValue<bool>(coords, columnEditDisabled);
            if (element_display_templates::ElementDisplay(value, coords, true)) {
                setElementValueBool.invoke(coords.column(), coords.row(), value);
            }
            ImGui::SameLine();
            ImGui::Text("%s", column.name.c_str());
            break;
        }
        case (SCH_NUMBER): {
            int value = getElementValue<int>(coords, columnEditDisabled);
            element_display_templates::ElementDisplay(value, coords);
            break;
        }
        case (SCH_DECIMAL): {
            double value = getElementValue<double>(coords, columnEditDisabled);
            element_display_templates::ElementDisplay(value, coords);
            break;
        }
        case (SCH_TEXT): {
            std::string value = getElementValue<std::string>(coords, columnEditDisabled);
            element_display_templates::ElementDisplay(value);
            break;
        }
        case (SCH_SELECT): {
            SingleSelectContainer value = getElementValue<SingleSelectContainer>(coords, columnEditDisabled);
            element_display_templates::ElementDisplay(value, m_scheduleCore, coords);
            break;
        }
        case (SCH_MULTISELECT): {
            SelectContainer value = getElementValue<SelectContainer>(coords, columnEditDisabled);
            element_display_templates::ElementDisplay(
                value, m_scheduleCore, coords, ImGui::GetColumnWidth(m_currentTableCoords.column()));
            break;
        }
        case (SCH_WEEKDAY): {
            WeekdayContainer value = getElementValue<WeekdayContainer>(coords, columnEditDisabled);
            element_display_templates::ElementDisplay(value, coords, ImGui::GetColumnWidth(m_currentTableCoords.column()));
            break;
        }
        case (SCH_TIME): {
            TimeContainer value = getElementValue<TimeContainer>(coords, columnEditDisabled);
            element_display_templates::ElementDisplay(value);
            break;
        }
        case (SCH_DATE): {
            DateContainer value = getElementValue<DateContainer>(coords, columnEditDisabled);
            element_display_templates::ElementDisplay(value);
            break;
        }
    }
    if (columnEditDisabled) {
        ImGui::PopItemFlag();
        ImGui::PopStyleVar();
    }
}