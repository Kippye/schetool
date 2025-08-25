#include "calendar/calendar_gui.h"
#include "calendar/calendar_item_window_subgui.h"
#include "main_menu_bar/main_menu_bar_gui.h"
#include "schedule/filter_editor_subgui.h"
#include "view_tab_bar_gui.h"
#include "gui_templates.h"
#include "util.h"
#include "filters/filter_rule.h"
#include "element_display_templates.h"
#include <algorithm>
#include <format>

bool CalendarGui::getIsTableCellRectHovered(ImGuiTable* table, int col, int row) const {
    // Not a valid cell so obviously it can't be hovered
    if (col < 0 || col >= ImGui::TableGetColumnCount() || row < 0 || row >= m_tableRowHeights.size()) {
        return false;
    }

    ImRect cellRect = ImGui::TableGetCellBgRect(table, col);
    cellRect.Max = ImVec2(cellRect.Max.x, cellRect.Min.y + m_prevTableRowHeights.at(row));

    return ImGui::IsMouseHoveringRect(cellRect.Min, cellRect.Max, false);
}

CalendarGui::CalendarGui(const char* ID, const ScheduleCore& scheduleCore, ScheduleEvents& scheduleEvents)
    : Gui(ID), m_scheduleCore(scheduleCore) {
    scheduleEvents.viewedDateChanged.addListener(viewedDateChangedListener);
    scheduleEvents.rowAdded.addListener(rowAddedListener);
    scheduleEvents.rowRemoved.addListener(rowRemovedListener);
    scheduleEvents.editUndone.addListener(editUndoneListener);
    scheduleEvents.editRedone.addListener(editRedoneListener);
    addSubGui(new CalendarItemWindowSubGui("CalendarItemWindowSubGui", scheduleCore, scheduleEvents));
    m_itemWindowSubGui = getSubGui<CalendarItemWindowSubGui>("CalendarItemWindowSubGui");
    addSubGui(new FilterEditorSubGui("FilterEditorSubGui", m_scheduleCore, scheduleEvents));

    // Add events from CalendarItemWindowSubGui to pipe through
    setElementValueBool.addEvent(m_itemWindowSubGui->setElementValueBool);
    setElementValueNumber.addEvent(m_itemWindowSubGui->setElementValueNumber);
    setElementValueDecimal.addEvent(m_itemWindowSubGui->setElementValueDecimal);
    setElementValueText.addEvent(m_itemWindowSubGui->setElementValueText);
    setElementValueSelect.addEvent(m_itemWindowSubGui->setElementValueSelect);
    setElementValueMultiselect.addEvent(m_itemWindowSubGui->setElementValueMultiselect);
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
    setColumnOrder.addEvent(m_itemWindowSubGui->setColumnOrder);
    modifyColumnSelectOptions.addEvent(m_itemWindowSubGui->modifyColumnSelectOptions);
    // entire column modification
    resetColumn.addEvent(m_itemWindowSubGui->resetColumn);
    // Link up filter event pipes
    auto filterEditor = getSubGui<FilterEditorSubGui>("FilterEditorSubGui");
    addColumnFilterGroup.addEvent(filterEditor->addColumnFilterGroup);
    setColumnFilterGroupName.addEvent(filterEditor->setColumnFilterGroupName);
    setColumnFilterGroupOperator.addEvent(filterEditor->setColumnFilterGroupOperator);
    setColumnFilterGroupEnabled.addEvent(filterEditor->setColumnFilterGroupEnabled);
    removeColumnFilterGroup.addEvent(filterEditor->removeColumnFilterGroup);

    addColumnFilter.addEvent(filterEditor->addColumnFilter);
    setColumnFilterOperator.addEvent(filterEditor->setColumnFilterOperator);
    removeColumnFilter.addEvent(filterEditor->removeColumnFilter);

    addColumnFilterRule.addEvent(filterEditor->addColumnFilterRule);
    editColumnFilterRule.addEvent(filterEditor->editColumnFilterRule);
    removeColumnFilterRule.addEvent(filterEditor->removeColumnFilterRule);
}

void CalendarGui::draw(GuiDrawArgs& args) {
    // If for whatever reason the TimeWrapper containing the viewed month + year becomes empty, fill it with the current date.
    if (m_viewedMonth.getIsEmpty()) {
        m_viewedMonth = TimeWrapper(TimeWrapper::getCurrentTime().getDateUTC());
    }

    ImGuiStyle& style = ImGui::GetStyle();

    const float offsetFromTop = MainMenuBarGui::getHeight() + ViewTabBarGui::getHeight();

    ImGui::SetNextWindowSize(ImVec2((float)args.windowSize.getWidth(), (float)args.windowSize.getHeight() - offsetFromTop));
    ImGui::SetNextWindowContentSize(
        ImVec2((float)args.windowSize.getWidth(), (float)args.windowSize.getHeight() - offsetFromTop) -
        style.WindowPadding * 2.0f);
    ImGui::SetNextWindowPos(ImVec2(0.0f, offsetFromTop));

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    if (ImGui::Begin(m_ID.c_str(),
                     NULL,
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                         ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus))
    {
        ImGui::PopStyleVar();  // WindowRounding = 0.0f
        const float SCHEDULE_TOP_MARGIN = offsetFromTop;
        const float CHILD_WINDOW_WIDTH = (float)args.windowSize.getWidth();
        const float CHILD_WINDOW_HEIGHT = (float)(args.windowSize.getHeight() - SCHEDULE_TOP_MARGIN);

        ImGui::SetNextWindowPos(ImVec2(0.0, SCHEDULE_TOP_MARGIN));
        ImGui::PushStyleColor(ImGuiCol_Border, gui_colors::colorInvisible);
        ImGui::PushStyleColor(ImGuiCol_BorderShadow, gui_colors::colorInvisible);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, gui_style_vars::windowEdgePadding);
        ImGui::BeginChild("CalendarPanel", ImVec2(CHILD_WINDOW_WIDTH, CHILD_WINDOW_HEIGHT), true);
        ImGui::PopStyleVar();
        ImGui::PopStyleColor(2);

        TimeWrapper formatTime;
        std::string monthName = m_viewedMonth.getDynamicFmtStringUTC("{:%B}");
        ImGui::SetNextItemWidth(ImGui::CalcTextSize("September").x * 1.5f);
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
        ImGui::SetNextItemWidth(ImGui::CalcTextSize(yearText.c_str()).x + style.FramePadding.x * 2.0f);
        if (ImGui::BeginCombo("##CalendarViewedYear", yearText.c_str(), ImGuiComboFlags_NoArrowButton)) {
            const unsigned int viewedYear = m_viewedMonth.getYearUTC();
            const unsigned int lowestDisplayedYear = TimeWrapper::limitYearToValidRange((viewedYear - 20) - (viewedYear % 10));
            const unsigned int maxDisplayedYear = TimeWrapper::limitYearToValidRange((viewedYear + 20) - (viewedYear % 10));
            for (unsigned int year = lowestDisplayedYear; year <= maxDisplayedYear; year++) {
                bool isSelected = year == viewedYear;

                formatTime.setYearUTC(year);
                monthName = formatTime.getDynamicFmtStringUTC("{:%Y}");
                if (ImGui::Selectable(monthName.c_str(), isSelected)) {
                    m_viewedMonth.setYearUTC(year);
                }

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
        ImGui::SameLine();
        // FILTER GROUPS
        const float todayButtonWidth = ImGui::CalcTextSize("Today").x * 1.5f;
        const float offsetFromRight =
            ImGui::GetFrameHeight() + todayButtonWidth + ImGui::GetFrameHeight() + style.ItemSpacing.x * 2.0f;
        if (auto filterEditor = getSubGui<FilterEditorSubGui>("FilterEditorSubGui")) {
            // Available space for all filter buttons
            const float availableSpace = ImGui::GetWindowWidth() - (ImGui::GetCursorScreenPos().x + offsetFromRight);
            const float filterListButtonWidth = gui_size_calculations::getTextButtonWidth("+ 99 more");
            const float plusLabelSize = ImGui::CalcTextSize("+").y;
            const float filterAddButtonSize = ImGui::CalcItemSize(ImVec2(), plusLabelSize, plusLabelSize).y;
            // We will draw n filter buttons, the add button and sometimes a button between the 2
            // This means there will be (n + 3) * ItemSpacing.x as well
            const float filterGroupButtonsSpace =
                std::max(0.0f, availableSpace - filterListButtonWidth - filterAddButtonSize - 2 * style.ItemSpacing.x);
            const short buttonsToDisplay = std::min(6, (int)std::floor(filterGroupButtonsSpace / gui_sizes::emptyLabelSize.x));
            const float filterButtonWidth =
                (filterGroupButtonsSpace - buttonsToDisplay * style.ItemSpacing.x) / buttonsToDisplay;
            size_t drawnButtonCount = 0;
            size_t totalFilterGroupCount = 0;
            // DATA TO PASS TO FILTER EDITOR
            bool openFilterEditor = false;
            size_t filterEditorColumn = m_scheduleCore.getColumnCount();
            size_t filterEditorGroupIndex = 0;
            ImRect itemAvoidRect;

            // LAMBDA: Draws buttons for every FilterGroup in the column. Sets data to pass to filterEditor if a button is clicked.
            auto drawFilterGroupButton = [&](size_t col, size_t filterGroupIndex, float buttonWidth, bool sameLine = true) {
                const Column& currentColumn = m_scheduleCore.getColumnConst(col);
                const auto& filterGroup = currentColumn.getFilterGroupConst(filterGroupIndex);
                if (filterGroup.getIsEnabled() == false) {
                    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, gui_colors::inactiveAlpha);
                }
                // FilterGroup button with the column name and the FilterGroup name
                std::string buttonText = std::format("{} ({})", filterGroup.getName(), currentColumn.name);
                if (ImGui::Button(std::format("{}##{};{}", buttonText, col, filterGroupIndex).c_str(), ImVec2(buttonWidth, 0)))
                {
                    openFilterEditor = true;
                    filterEditorColumn = col;
                    filterEditorGroupIndex = filterGroupIndex;
                    itemAvoidRect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
                }
                if (filterGroup.getIsEnabled() == false) {
                    ImGui::PopStyleVar();
                }
                if (ImGui::BeginItemTooltip()) {
                    ImGui::Text("%s", buttonText.c_str());
                    ImGui::EndTooltip();
                }

                if (sameLine &&
                    !(col == m_scheduleCore.getColumnCount() - 1 &&
                      filterGroupIndex == currentColumn.getFilterGroupCount() - 1))
                {
                    ImGui::SameLine();
                }
                drawnButtonCount++;
            };

            for (size_t col = 0; col < m_scheduleCore.getColumnCount(); col++) {
                const auto& columnFilterGroups = m_scheduleCore.getColumnConst(col).getFilterGroupsConst();

                for (size_t i = 0; i < columnFilterGroups.size(); i++) {
                    // Draw up to 8 buttons
                    if (drawnButtonCount < buttonsToDisplay) {
                        drawFilterGroupButton(col, i, filterButtonWidth);
                    }
                    // Count all filter groups for later
                    totalFilterGroupCount++;
                }
            }
            // There are too many filter groups to draw in a row. Show the excess as a counter button
            // The button opens a list popup when pressed
            if (totalFilterGroupCount > drawnButtonCount) {
                if (ImGui::Button(
                        std::format("+ {} more##OpenFilterGroupListButton", totalFilterGroupCount - drawnButtonCount).c_str(),
                        ImVec2(filterListButtonWidth, 0.0f)))
                {
                    ImGui::OpenPopup("FilterGroupListPopup");
                }
                if (ImGui::BeginPopup("FilterGroupListPopup")) {
                    // We need to skip every filter group that already has a button
                    size_t skippedCount = 0;
                    const float listButtonWidth = ImGui::CalcTextSize("M").x * filter_consts::FILTER_GROUP_NAME_MAX_LENGTH;
                    for (size_t col = 0; col < m_scheduleCore.getColumnCount(); col++) {
                        const Column& column = m_scheduleCore.getColumnConst(col);
                        for (size_t i = 0; i < column.getFilterGroupCount(); i++) {
                            if (skippedCount >= drawnButtonCount) {
                                drawFilterGroupButton(col, i, listButtonWidth, false);
                            }
                            skippedCount++;
                        }
                    }
                    ImGui::EndPopup();
                }
                ImGui::SameLine();
            }
            filterEditor->draw(args);
            // Open filter editor if needed.
            // NOTE: All the data here is set in drawFilterGroupButton() lambda!
            if (openFilterEditor) {
                filterEditor->openGroupEdit(filterEditorColumn, filterEditorGroupIndex, itemAvoidRect);
            }
            ImGui::SameLine();
            bool createGroupOpenFilterEditor = false;
            GuiTextureInfo addIcon;
            args.guiTextures.exists("icon_add", addIcon);
            gui_templates::ImageButtonStyleColored(
                "addFilterGroup", addIcon.ImID, ImVec2(filterAddButtonSize, filterAddButtonSize));
            if (ImGui::BeginPopupContextItem("AddFilterGroupColumnSelection", ImGuiPopupFlags_MouseButtonLeft)) {
                ImGui::Text("%s", "Select property");
                for (size_t col = 0; col < m_scheduleCore.getColumnCount(); col++) {
                    const Column& column = m_scheduleCore.getColumnConst(col);
                    if (ImGui::Button(
                            std::format("{} ({})##{}", column.name, schedule_consts::scheduleTypeNames.at(column.type), col)
                                .c_str()))
                    {
                        createGroupOpenFilterEditor = true;
                        filterEditorColumn = col;
                    }
                }
                ImGui::EndPopup();
            }
            if (createGroupOpenFilterEditor) {
                filterEditor->createGroupAndEdit(filterEditorColumn, ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax()));
            }
        }
        // MONTH SELECTION & INCREMENTING
        ImGui::SameLine();
        // Move cursor using previously calculated offset from the right edge: Arrow + [ ] + Button + [] + Arrow + WindowSpacing
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

        drawCalendarTable(args);
        ImGui::EndChild();
    } else {
        ImGui::PopStyleVar();  // WindowRounding = 0.0f
    }
    ImGui::End();

    if (m_itemWindowSubGui) {
        m_itemWindowSubGui->draw(args);
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

void CalendarGui::drawCalendarTable(GuiDrawArgs& drawArgs) {
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

    // Reset table row heights
    m_tableRowHeights.assign(6, 0.0f);

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
                drawArgs,
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
            drawCalendarDayContent(drawArgs, dayIndex, m_viewedMonth.getMonthUTC(), i + 1);
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
            drawCalendarDayContent(drawArgs, dayIndex, nextMonth, i + 1);
        }

        // Draw dragged item at the cursor
        if (m_dragDropState.has_value() && ImGui::TableGetColumnCount() > 0) {
            ImGui::SetNextWindowPos(ImGui::GetMousePos() - m_draggedItemCursorOffset);
            ImGui::SetNextWindowContentSize(ImVec2(ImGui::GetCurrentTable()->Columns[0].WidthAuto,
                                                   m_prevTableRowHeights.at(m_dragDropState->sourceCell.row())));
            if (ImGui::Begin("DraggedItemDisplay",
                             nullptr,
                             ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoDecoration |
                                 ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground |
                                 ImGuiWindowFlags_NoSavedSettings))
            {
                drawDraggedItemDisplay(m_dragDropState->itemRow);
                ImGui::End();
            }
        }
        ImGui::EndTable();

        // When the mouse is released, any active child windows get unactivated
        if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
            m_activeItemChildID.reset();
        }
        // Copy row heights AFTER calculating all of them
        m_prevTableRowHeights = m_tableRowHeights;
    }
}

void CalendarGui::drawCalendarDayContent(GuiDrawArgs& drawArgs, size_t& dayIndex, int month, int dayNumber) {
    ImGuiStyle& style = ImGui::GetStyle();
    unsigned int pushedVarCount = 0;
    bool isDisabled = false;
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
    // Highlight the cell that would currently be the drag drop target
    if (m_dragDropState.has_value() && m_hoveredCellCoords == m_currentTableCoords) {
        ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, ImGui::GetColorU32(style.Colors[ImGuiCol_ButtonHovered]));
    }
    // Display days from other months as slightly darker
    if (month != m_viewedMonth.getMonthUTC()) {
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.65f);
        pushedVarCount++;
    }
    // Disable days from invalid years
    if (calendarDayDate.getYear() != TimeWrapper::limitYearToValidRange(calendarDayDate.getYear())) {
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        isDisabled = true;
    }
    // Align the first column's text to frame padding.
    // This is to avoid the first column being vertically misaligned compared to every other column.
    if (ImGui::TableGetColumnIndex() == 0) {
        ImGui::AlignTextToFramePadding();
    }
    TimeWrapper calendarDayTime = TimeWrapper(calendarDayDate);
    std::string dayNumberText = dayNumber == 1 ? calendarDayTime.getDynamicFmtStringUTC("{:%b} 1") : std::to_string(dayNumber);
    ImGui::Text("%s", dayNumberText.c_str());
    const float calendarDayTextWidth = ImGui::GetItemRectSize().x;
    // + button to add a calendar item to this calendar day
    const float addItemButtonSize = ImGui::CalcTextSize("+").y;
    ImGui::SameLine(0.0f,
                    ImGui::GetColumnWidth(ImGui::TableGetColumnIndex()) - calendarDayTextWidth - addItemButtonSize -
                        style.FramePadding.x * 2.0f);
    bool isTableCellHovered = (ImGui::TableGetHoveredColumn() == ImGui::TableGetColumnIndex() &&
                               ImGui::TableGetHoveredRow() == ImGui::TableGetRowIndex());
    if (!isTableCellHovered) {
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.0f);
    }
    GuiTextureInfo addIcon;
    drawArgs.guiTextures.exists("icon_add", addIcon);
    // Adds a row and then sets its Date column value to this calendar day's date
    if (gui_templates::ImageButtonStyleColored(std::format("addCalendarItem{};{}", month, dayNumber).c_str(),
                                               addIcon.ImID,
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
    if (isDisabled) {
        ImGui::PopItemFlag();
        ImGui::SetItemTooltip("This year is out of the supported year range.");
    }
    if (!isTableCellHovered) {
        ImGui::PopStyleVar();
    }

    // Drop the dragged item here if this is the target cell
    if (m_dragDropState.has_value() && m_dragDropState->dropCell == m_currentTableCoords) {
        if (isDisabled == false)  // Can't drop to a disabled cell :()
        {
            setElementValueDate.invoke(m_scheduleCore.getFlaggedColumnIndex(ScheduleColumnFlags_Date),
                                       m_dragDropState->itemRow,
                                       DateContainer(calendarDayTime));
        }
        m_dragDropState.reset();
    }

    drawCalendarDayItems(drawArgs, DateContainer(calendarDayTime));
    ImGui::PopStyleVar(pushedVarCount);
    dayIndex++;
}

// TODO: Optimization - Could precalculate a list of rows for each calendar date and pass it here.
// That way, each calendar day won't have to loop through EVERY row in the schedule (could be quite a hit when multiplied by 31)
void CalendarGui::drawCalendarDayItems(GuiDrawArgs& drawArgs, const DateContainer& calendarDayDate) {
    const size_t dateColumnIndex = m_scheduleCore.getFlaggedColumnIndex(ScheduleColumnFlags_Date);

    FilterRule<DateContainer> isThisDate = FilterRule<DateContainer>(calendarDayDate);
    std::vector<size_t> sortedRowIndices = m_scheduleCore.getSortedRowIndices();
    for (size_t unsortedRow = 0; unsortedRow < sortedRowIndices.size(); unsortedRow++) {
        size_t row = sortedRowIndices[unsortedRow];
        // This row is NOT on the current calendar day date
        if (!isThisDate.checkPasses(m_scheduleCore.getElementConst(dateColumnIndex, row))) {
            continue;
        }
        // CHECK FILTERS BEFORE DRAWING ITEM / ROW
        if (!m_scheduleCore.checkPassesAllFilters(row, m_scheduleDateOverride, {dateColumnIndex})) {
            continue;
        }
        bool itemRemoved = false;
        std::string itemIdSuffix = std::format(
            "{};{};{}", row, calendarDayDate.getTimeConst().getMonthUTC(), calendarDayDate.getTimeConst().getMonthDayUTC());
        drawCalendarDayItem(drawArgs, row, itemIdSuffix, itemRemoved);
        if (itemRemoved) {
            removeRow.invoke(row);
            // JIC (Just In Case)
            m_hoveredItemChildID.reset();
            m_activeItemChildID.reset();
            m_dragDropState.reset();
            return;
        }
    }
    m_tableRowHeights.at(ImGui::TableGetRowIndex()) =  // The table row's height is the maximum value of:
        std::max({m_tableRowHeights.at(ImGui::TableGetRowIndex()),  // Row height from a previous column
                  ImGui::GetCurrentTable()->RowPosY2 - ImGui::GetCurrentTable()->RowPosY1,  // or imgui's calculated row height
                  ImGui::GetCursorScreenPos().y -
                      ImGui::GetCurrentTable()->RowPosY1});  // or the amount the drawing cursor has moved vertically
    // Hovering this table cell
    if (getIsTableCellRectHovered(ImGui::GetCurrentTable())) {
        m_hoveredCellCoords = {static_cast<size_t>(ImGui::TableGetColumnIndex()),
                               static_cast<size_t>(ImGui::TableGetRowIndex())};
    } else if (m_hoveredCellCoords.has_value() && m_hoveredCellCoords == m_currentTableCoords) {
        // Clear when the previously hovered cell is no longer hovered
        m_hoveredCellCoords.reset();
    }
}

void CalendarGui::drawCalendarDayItem(GuiDrawArgs& drawArgs, size_t itemRow, const std::string& idSuffix, bool& wasRemoved) {
    wasRemoved = false;
    ImGuiStyle& style = ImGui::GetStyle();

    const size_t dateColumnIndex = m_scheduleCore.getFlaggedColumnIndex(ScheduleColumnFlags_Date);
    std::string childLabelString = std::string("CalendarItem##") + idSuffix;
    unsigned int pushedColorCount = 0;
    ImVec4 stateBackgroundColor = gui_colors::scheduleItemStateColors.at(m_scheduleCore.getRowItemState(itemRow));

    if (m_hoveredItemChildID.has_value() && m_hoveredItemChildID.value() == ImGui::GetID(childLabelString.c_str())) {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, style.Colors[ImGuiCol_ButtonHovered]);
        pushedColorCount++;
        // The button is pressed but not dragging
        if (ImGui::IsMouseDown(ImGuiMouseButton_Left) && !ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
            ImGui::PushStyleColor(ImGuiCol_ChildBg, style.Colors[ImGuiCol_ButtonActive]);
            pushedColorCount++;
        }
    } else if (drawArgs.preferences.getRowHighlightingEnabled() && stateBackgroundColor.w > 0.0f) {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, stateBackgroundColor);
        pushedColorCount++;
    } else {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, style.Colors[ImGuiCol_WindowBg]);
        pushedColorCount++;
    }
    // NOTE: While in the child scope, you CANNOT use ImGui::TableGetColumn/Row, as they will be "0"
    // Use m_currentTableCoords instead!
    if (ImGui::BeginChild(childLabelString.c_str(),
                          ImVec2(),
                          ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_Borders,
                          ImGuiWindowFlags_NoScrollWithMouse))
    {
        const ImGuiID childID = ImGui::GetCurrentWindow()->ChildId;
        const size_t nameColumnIndex = m_scheduleCore.getFlaggedColumnIndex(ScheduleColumnFlags_Name);
        drawItemProperty({nameColumnIndex, itemRow});

        // Remove item / itemRow button (only visible while the item is hovered)
        if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem)) {
            const float removeButtonSize = ImGui::CalcTextSize("X").y;
            ImGui::SameLine(style.WindowPadding.x + ImGui::GetCurrentWindow()->ContentRegionRect.GetSize().x -
                            removeButtonSize);
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2());
            if (gui_templates::ImageButtonStyleColored(std::format("##RemoveCalendarItem{}", idSuffix).c_str(),
                                                       drawArgs.guiTextures.getOrLoad("icon_remove").ImID,
                                                       ImVec2(removeButtonSize, removeButtonSize)))
            {
                wasRemoved = true;
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
                TimeContainer startTime = m_scheduleCore.getElementValueConstRef<TimeContainer>(col, itemRow);
                TimeContainer endTime = m_scheduleCore.getElementValueConstRef<TimeContainer>(
                    m_scheduleCore.getFlaggedColumnIndex(ScheduleColumnFlags_End), itemRow);
                std::string timeText = std::format("{} - {}", startTime.getString(), endTime.getString());
                ImGui::Text("%s", timeText.c_str());
            } else {
                drawItemProperty({col, itemRow});
            }
        }

        // The child window of this item is being hovered
        if (ImGui::IsWindowHovered()) {
            m_hoveredItemChildID = childID;
            // This child window was clicked -> mark it as active
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                m_activeItemChildID = childID;
                m_draggedItemCursorOffset = ImGui::GetMousePos() - ImGui::GetCurrentWindow()->Rect().Min;
            }
        } else if (m_hoveredItemChildID == childID) {  // No longer being hovered
            m_hoveredItemChildID.reset();
        }
        // LMB was released
        if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
            // Hovering this child and NOT dragging an item or hovering an enabled item in front of it -> open its window
            if (m_hoveredItemChildID == childID && m_dragDropState.has_value() == false &&
                (!ImGui::IsAnyItemHovered() || ImGui::GetCurrentContext()->HoveredIdIsDisabled))
            {
                m_openItemWindowAtRow = itemRow;
            }
        }
        // Active but not hovered -> start drag & drop for this item
        if (m_activeItemChildID == childID && m_hoveredItemChildID != childID) {
            m_dragDropState = DragDropState(itemRow, m_currentTableCoords);
        }
    }
    ImGui::EndChild();
    // Dragging this item, but it is no longer active -> drop it
    if (m_activeItemChildID.has_value() == false && m_dragDropState.has_value() && m_dragDropState->itemRow == itemRow) {
        if (m_hoveredCellCoords.has_value()) {
            const auto [hoveredCol, hoveredRow] = m_hoveredCellCoords->getAsPair();
            bool isThisCell = (hoveredCol == m_currentTableCoords.column() && hoveredRow == m_currentTableCoords.row());
            if (!isThisCell) {
                m_dragDropState->dropCell = {static_cast<size_t>(hoveredCol), static_cast<size_t>(hoveredRow)};
            } else {  // Dropping on the same cell -> cancel the drag & drop
                m_dragDropState.reset();
            }
        } else {  // Dropping the dragged item somewhere invalid -> cancel the drag & drop
            m_dragDropState.reset();
        }
    }
    ImGui::PopStyleColor(pushedColorCount);
}

void CalendarGui::drawDraggedItemDisplay(size_t itemRow) {
    const size_t dateColumnIndex = m_scheduleCore.getFlaggedColumnIndex(ScheduleColumnFlags_Date);

    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetStyle().Colors[ImGuiCol_WindowBg]);  // Add background
    if (ImGui::BeginChild("DraggedCalendarItemDisplay",
                          ImVec2(),
                          ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_Borders,
                          ImGuiWindowFlags_NoInputs))
    {
        const size_t nameColumnIndex = m_scheduleCore.getFlaggedColumnIndex(ScheduleColumnFlags_Name);
        drawItemProperty({nameColumnIndex, itemRow});

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
                TimeContainer startTime = m_scheduleCore.getElementValueConstRef<TimeContainer>(col, itemRow);
                TimeContainer endTime = m_scheduleCore.getElementValueConstRef<TimeContainer>(
                    m_scheduleCore.getFlaggedColumnIndex(ScheduleColumnFlags_End), itemRow);
                std::string timeText = std::format("{} - {}", startTime.getString(), endTime.getString());
                ImGui::Text("%s", timeText.c_str());
            } else {
                drawItemProperty({col, itemRow});
            }
        }
    }
    ImGui::PopStyleColor();
    ImGui::EndChild();
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