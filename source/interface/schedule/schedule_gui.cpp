
#include <array>
#include <iterator>
#include <string>
#include <cstdio>
#include <algorithm>
#include <format>
#include "schedule_constants.h"
#include "schedule/schedule_gui.h"
#include "schedule/element_editor_subgui.h"
#include "schedule/filter_editor_subgui.h"
#include "gui_templates.h"
#include "gui_constants.h"
#include "schedule_coordinates.h"

ScheduleGui::ScheduleGui(const char* ID,
                         const ScheduleCore& scheduleCore,
                         ScheduleEvents& scheduleEvents,
                         const std::shared_ptr<const MainMenuBarGui> mainMenuBarGui)
    : m_scheduleCore(scheduleCore), Gui(ID), m_mainMenuBarGui(mainMenuBarGui) {
    addSubGui(new ElementEditorSubGui("ElementEditorSubGui", m_scheduleCore));
    addSubGui(new FilterEditorSubGui("FilterEditorSubGui", m_scheduleCore, scheduleEvents));
}

// Checks if the current table cell was clicked to edit.
// NOTE: This very much assumes that the function was called from a valid point in a table!
// Actual applied checks:
// 1. Editing this column isn't disabled
// 2. The left mouse button was clicked
// 3. The current column and row are being hovered
// 4. A column resize border is NOT being hovered
bool ScheduleGui::isEditableElementClicked(bool isEditingDisabled) const {
    return isEditingDisabled == false && ImGui::IsMouseClicked(ImGuiMouseButton_Left) &&
        ImGui::TableGetHoveredColumn() == ImGui::TableGetColumnIndex() &&
        ImGui::TableGetHoveredRow() == ImGui::TableGetRowIndex() && ImGui::GetCurrentTable()->HoveredColumnBorder == -1;
}

void ScheduleGui::draw(Window& window, Input& input, GuiTextures& guiTextures) {
    if (m_visible == false) {
        return;
    }

    ImGuiStyle style = ImGui::GetStyle();
    ImGui::SetNextWindowSize(ImVec2((float)window.SCREEN_WIDTH, (float)window.SCREEN_HEIGHT));
    ImGui::SetNextWindowContentSize(ImVec2((float)window.SCREEN_WIDTH, (float)window.SCREEN_HEIGHT) -
                                    style.WindowPadding * 2.0f);
    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));

    ImGui::Begin(m_ID.c_str(),
                 NULL,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);
    // Add menu bar height as offset
    if (m_mainMenuBarGui) {
        ImGui::SetCursorPosY(m_mainMenuBarGui->getHeight());
    }
    // Current date text
    const TimeWrapper& currentDate =
        m_scheduleDateOverride.getIsEmpty() == false ? m_scheduleDateOverride : TimeWrapper::getCurrentTime();
    const std::string_view currentDateFmt = currentDate.getMonthDayUTC() < 10 ? "{:%A,%e. %B %Y}" : "{:%A, %e. %B %Y}";
    std::string viewedDateText = m_scheduleDateOverride.getIsEmpty() == true
        ? currentDate.getDynamicFmtString(currentDateFmt)
        : currentDate.getDynamicFmtStringUTC(currentDateFmt);
    ImGui::PushFont(InterfaceStyleHandler::getFontData(InterfaceStyleHandler::getFontSize() == FontSize::Large
                                                           ? FontSize::Large
                                                           : (FontSize)((int)InterfaceStyleHandler::getFontSize() + 1)));
    ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2.0f -
                         gui_size_calculations::getTextButtonWidth(viewedDateText.c_str()) / 2.0f);
    auto currentTimeUTC = TimeWrapper::getCurrentTime().getTimeUTC();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(ImGui::GetStyle().WindowPadding.x, 0.0f));
    ImGui::PushStyleVar(
        ImGuiStyleVar_Alpha,
        m_scheduleDateOverride.getIsEmpty() ? 1.0f
                                            : 0.25f +
                std::abs(std::sin(std::chrono::milliseconds(std::chrono::floor<std::chrono::milliseconds>(currentTimeUTC) -
                                                            std::chrono::floor<std::chrono::days>(currentTimeUTC))
                                      .count() /
                                  800.f)));
    if (ImGui::Button(std::format("{}##ScheduleViewDateButton", viewedDateText).c_str())) {
        m_openDateSelectPopup = true;
    }
    const float scheduleHeaderTextHeight = ImGui::GetItemRectSize().y;
    ImGui::PopStyleVar(2);
    ImGui::PopFont();
    // Viewed date selector popup
    if (ImGui::BeginPopup("Schedule Date Selector")) {
        // Display date editor to edit m_scheduleDateOverride.
        // If the current date was selected, just clear m_scheduleDateOverride again.
        if (gui_templates::DateEditor(m_scheduleDateOverride, m_dateSelectorYear, m_dateSelectorMonth) &&
            m_scheduleDateOverride.getDateUTC() == TimeWrapper::getCurrentTime().getLocalDate())
        {
            m_scheduleDateOverride.clear();
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
    // Show a reset button when viewing a different date
    // Preset button size because it uses a preset size texture variant
    const float resetButtonSize =
        24.0f;  //ImGui::CalcItemSize(ImVec2(0, 0), labelSize.x + style.ItemInnerSpacing.x * 2.0f, labelSize.y + style.ItemInnerSpacing.y * 2.0f).y;
    if (m_scheduleDateOverride.getIsEmpty() == false) {
        ImGui::SameLine();
        ImGui::SetCursorPosY(ImGui::GetItemRectMin().y + ImGui::GetItemRectSize().y / 2.0f - resetButtonSize / 2.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2());
        if (gui_templates::ImageButtonStyleColored("##ResetToTodayButton",
                                                   guiTextures.getOrLoad("icon_reset_24px").ImID,
                                                   ImVec2(resetButtonSize, resetButtonSize)))
        {
            m_scheduleDateOverride.clear();
        }
        ImGui::PopStyleVar();
    }

    const float SCHEDULE_TOP_BAR_HEIGHT =
        scheduleHeaderTextHeight + m_mainMenuBarGui->getHeight();  // + style.ItemSpacing.y * 2;
    const float ADD_ROW_BUTTON_HEIGHT = 32.0f;
    const float ADD_COLUMN_BUTTON_WIDTH = 32.0f;
    const float CHILD_WINDOW_WIDTH = (float)(window.SCREEN_WIDTH - ADD_COLUMN_BUTTON_WIDTH - 8);
    const float CHILD_WINDOW_HEIGHT = (float)(window.SCREEN_HEIGHT - SCHEDULE_TOP_BAR_HEIGHT - ADD_ROW_BUTTON_HEIGHT - 16.0f);

    ImGui::SetNextWindowPos(ImVec2(0.0, SCHEDULE_TOP_BAR_HEIGHT));
    ImGui::BeginChild("SchedulePanel", ImVec2(CHILD_WINDOW_WIDTH, CHILD_WINDOW_HEIGHT), true);
    // Avoid imgui 0 column abort by not beginning the table at all if there are no columns in the schedule
    if (m_scheduleCore.getColumnCount() > 0) {
        // DRAW the SCHEDULE TABLE
        drawScheduleTable(window, input, guiTextures);
    }
    ImGui::EndChild();
    ImGui::SameLine();
    bool addColumnButtonDisabled = m_scheduleCore.getColumnCount() >= schedule_consts::COLUMN_MAX_COUNT;
    if (addColumnButtonDisabled) {
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, gui_colors::disabledAlpha);
    }
    ImGui::Button("+", ImVec2(ADD_COLUMN_BUTTON_WIDTH, (float)(CHILD_WINDOW_HEIGHT)));
    if (ImGui::BeginPopupContextItem("SelectAddedColumnTypeContext", ImGuiPopupFlags_MouseButtonLeft)) {
        ImGui::Text("Add column");
        float columnTypeButtonSize = 1.0f;
        for (int colType = 0; colType < SCH_LAST; colType++) {
            float currentTextSize =
                gui_size_calculations::getTextButtonWidth(schedule_consts::scheduleTypeNames.at((SCHEDULE_TYPE)colType));
            columnTypeButtonSize = std::max(columnTypeButtonSize, currentTextSize + ImGui::GetStyle().FramePadding.x * 2.0f);
        }
        for (int colType = 0; colType < SCH_LAST; colType++) {
            if (ImGui::Button(
                    std::format(
                        "{}##AddedColumnTypeButton{}", schedule_consts::scheduleTypeNames.at((SCHEDULE_TYPE)colType), colType)
                        .c_str(),
                    ImVec2(columnTypeButtonSize, 0.0f)))
            {
                addDefaultColumn.invoke(m_scheduleCore.getColumnCount(), (SCHEDULE_TYPE)colType);
                if (!input.buttonStates.ctrlDown) {
                    ImGui::CloseCurrentPopup();
                }
            }
        }
        ImGui::EndPopup();
    }
    if (addColumnButtonDisabled) {
        ImGui::PopItemFlag();
        ImGui::PopStyleVar();
        if (ImGui::BeginItemTooltip()) {
            ImGui::Text("Adding too many columns is not good for you!");
            ImGui::EndTooltip();
        }
    }
    if (ImGui::Button("Add row", ImVec2((float)(window.SCREEN_WIDTH - ADD_COLUMN_BUTTON_WIDTH - 26), ADD_ROW_BUTTON_HEIGHT))) {
        addRow.invoke(m_scheduleCore.getRowCount());
    }
    ImGui::End();
}

void ScheduleGui::drawColumnHeaderContextContent(size_t columnIndex, ImGuiTable* table, ImGuiTableFlags tableFlags) {
    const Column& column = *m_scheduleCore.getColumn(columnIndex);

    // Renaming
    std::string name = column.name.c_str();
    name.reserve(COLUMN_NAME_MAX_LENGTH);
    char* buf = name.data();

    if (ImGui::InputText(
            std::format("##columnName{}", columnIndex).c_str(), buf, name.capacity(), ImGuiInputTextFlags_EnterReturnsTrue))
    {
        setColumnName.invoke(columnIndex, buf);
    }

    // Select type (for non-permanent columns)
    ImGui::Separator();
    if (column.permanent) {
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
    }
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Type:");
    ImGui::SameLine();
    if (std::optional<SCHEDULE_TYPE> newColumnType =
            gui_templates::Dropdown("##ColumnType", column.type, schedule_consts::scheduleTypeNames))
    {
        setColumnType.invoke(columnIndex, newColumnType.value());
    }
    if (column.permanent) {
        ImGui::PopItemFlag();
    }

    ImGui::Separator();

    if (ImGui::MenuItem("Remove", NULL, false, !column.permanent)) {
        removeColumn.invoke(columnIndex);
    }

    if (ImGui::MenuItem("Duplicate", NULL, false, !column.permanent)) {
        duplicateColumn.invoke(columnIndex);
    }

    ImGui::Separator();

    // Reset values
    if (ImGui::MenuItem("Reset default values", NULL, false)) {
        resetColumn.invoke(columnIndex, true);
    }

    // Reset setting dropdown
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Reset column:");
    ImGui::SameLine();
    if (std::optional<ColumnResetOption> newColumnResetOption =
            gui_templates::Dropdown("##ColumnResetSetting", column.resetOption, schedule_consts::columnResetOptionStrings))
    {
        setColumnResetOption.invoke(columnIndex, newColumnResetOption.value());
    }

    ImGui::Separator();

    // Resizing
    if (tableFlags & ImGuiTableFlags_Resizable) {
        if (ImGui::MenuItem("Size column to fit###SizeOne", NULL, false))
            ImGui::TableSetColumnWidthAutoSingle(table, columnIndex);

        const char* size_all_desc;
        //if (table->ColumnsEnabledFixedCount == table->ColumnsEnabledCount && (table->Flags & ImGuiTableFlags_SizingMask_) != ImGuiTableFlags_SizingFixedSame)
        //	size_all_desc = "Size all columns to fit###SizeAll";        // All fixed
        //else
        size_all_desc = "Size all columns to default###SizeAll";  // All stretch or mixed
        if (ImGui::MenuItem(size_all_desc, NULL))
            ImGui::TableSetColumnWidthAutoAll(table);
    }

    // Ordering
    if (tableFlags & ImGuiTableFlags_Reorderable) {
        if (ImGui::MenuItem("Reset order", NULL, false, !table->IsDefaultDisplayOrder))
            table->IsResetDisplayOrderRequest = true;
    }

    ImGui::Separator();

    // Hiding / Visibility
    if (tableFlags & ImGuiTableFlags_Hideable) {
        ImGui::PushItemFlag(ImGuiItemFlags_AutoClosePopups, false);
        for (int otherColumnIndex = 0; otherColumnIndex < table->ColumnsCount; otherColumnIndex++) {
            if (column.permanent) {
                continue;
            }
            ImGuiTableColumn* otherColumn = &table->Columns[otherColumnIndex];
            const char* name = ImGui::TableGetColumnName(table, otherColumnIndex);
            if (name == NULL || name[0] == 0)
                name = "Unnamed";

            // Make sure we can't hide the last active column
            bool menu_item_active = (otherColumn->Flags & ImGuiTableColumnFlags_NoHide) ? false : true;
            if (otherColumn->IsEnabled && table->ColumnsEnabledCount <= 1)
                menu_item_active = false;
            if (ImGui::MenuItem(name, NULL, otherColumn->IsEnabled, menu_item_active))
                otherColumn->IsUserEnabledNextFrame = !otherColumn->IsEnabled;
        }
        ImGui::PopItemFlag();
    }

    if (ImGui::Button("Close"))
        ImGui::CloseCurrentPopup();
}

void ScheduleGui::openRowContextPopup(size_t row) {
    m_rowContextRow = row;
    ImGui::OpenPopup("ScheduleTableRowContextPopup", ImGuiPopupFlags_NoOpenOverExistingPopup);
}

void ScheduleGui::openCellContextPopup(size_t column, size_t row) {
    m_cellContextCoords = {column, row};
    ImGui::OpenPopup("ScheduleTableCellContextPopup", ImGuiPopupFlags_NoOpenOverExistingPopup);
}

void ScheduleGui::closeRowContextPopup() {
    m_rowContextRow.reset();
}

void ScheduleGui::closeCellContextPopup() {
    m_cellContextCoords.reset();
}

void ScheduleGui::drawRowContextContent() {
    if (m_rowContextRow.has_value() == false) {
        closeRowContextPopup();
        return;
    }
    if (ImGui::BeginPopup("ScheduleTableRowContextPopup", ImGuiWindowFlags_NoMove)) {
        if (m_rowContextRow.value() < m_scheduleCore.getRowCount()) {
            if (ImGui::Button("Remove row")) {
                removeRow.invoke(m_rowContextRow.value());
            }
            if (ImGui::Button("Duplicate row")) {
                duplicateRow.invoke(m_rowContextRow.value());
            }
        }

        if (ImGui::Button("Close")) {
            ImGui::CloseCurrentPopup();
            closeRowContextPopup();
        }
        ImGui::EndPopup();
    }
}

void ScheduleGui::drawCellContextContent() {
    if (m_cellContextCoords.has_value() == false) {
        closeCellContextPopup();
        return;
    }
    if (ImGui::BeginPopup("ScheduleTableCellContextPopup", ImGuiWindowFlags_NoMove)) {
        if (m_cellContextCoords->column() < m_scheduleCore.getColumnCount()) {
            auto [col, row] = m_cellContextCoords->getAsPair();
            ImGui::Text("%s", std::format("Cell (Column: {}; Row: {})", col, row).c_str());
        }

        if (ImGui::Button("Close")) {
            ImGui::CloseCurrentPopup();
            closeCellContextPopup();
        }
        ImGui::EndPopup();
    }
}

void ScheduleGui::drawScheduleTable(Window& window, Input& input, GuiTextures& guiTextures) {
    ImGuiStyle& style = ImGui::GetStyle();
    ImGuiTableFlags tableFlags = ImGuiTableFlags_Reorderable | ImGuiTableFlags_ScrollY | ImGuiTableFlags_Borders |
        ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingFixedSame | ImGuiTableFlags_ScrollX;
    if (ImGui::BeginTable("ScheduleTable", m_scheduleCore.getColumnCount(), tableFlags, ImGui::GetContentRegionAvail())) {
        ImGui::GetCurrentTable()->DisableDefaultContextMenu = true;
        for (size_t column = 0; column < m_scheduleCore.getColumnCount(); column++) {
            ImGui::TableSetupColumn(m_scheduleCore.getColumn(column)->name.c_str());
        }

        // ROW 0: Filters
        ImGui::TableNextRow();
        for (size_t column = 0; column < m_scheduleCore.getColumnCount() && column < ImGui::TableGetColumnCount(); column++) {
            ImGui::TableSetColumnIndex(column);

            const ImVec2 label_size = ImGui::CalcTextSize("W", NULL, true);
            float addFilterButtonSize =
                ImGui::CalcItemSize(
                    ImVec2(0.0f, 0.0f), label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f)
                    .y;
            if (ImGui::Button(std::format("+##addFilterGroup{}", column).c_str(),
                              ImVec2(addFilterButtonSize, addFilterButtonSize)))
            {
                // display the FilterGroup editor to add a filter group to this Column
                if (auto filterEditor = getSubGui<FilterEditorSubGui>("FilterEditorSubGui")) {
                    filterEditor->createGroupAndEdit(column, ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax()));
                }
            }

            if (auto filterEditor = getSubGui<FilterEditorSubGui>("FilterEditorSubGui")) {
                if (filterEditor->getColumn() == column) {
                    filterEditor->draw(window, input, guiTextures);
                }
            }

            ImGui::SameLine();

            const Column* currentColumn = m_scheduleCore.getColumn(column);
            const auto& columnFilterGroups = currentColumn->getFilterGroupsConst();

            // DATA TO PASS TO FILTER EDITOR
            bool openFilterEditor = false;
            size_t editorGroupIndex = 0;
            ImRect itemAvoidRect;

            // LAMBDA: Draws buttons for every FilterGroup in the column. Sets data to pass to filterEditor if a button is clicked.
            auto drawFilterGroupButtons = [&](bool sameLine, float buttonWidth) {
                for (size_t i = 0; i < columnFilterGroups.size(); i++) {
                    const auto& filterGroup = currentColumn->getFilterGroupConst(i);
                    if (filterGroup.getIsEnabled() == false) {
                        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, gui_colors::inactiveAlpha);
                    }
                    // FilterGroup button with its name
                    if (ImGui::Button(std::format("{}##{};{}", filterGroup.getName(), column, i).c_str(),
                                      ImVec2(buttonWidth, 0)))
                    {
                        openFilterEditor = true;
                        editorGroupIndex = i;
                        itemAvoidRect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
                    }
                    if (filterGroup.getIsEnabled() == false) {
                        ImGui::PopStyleVar();
                    }

                    if (sameLine && i < currentColumn->getFilterGroupCount() - 1) {
                        ImGui::SameLine();
                    }
                }
            };

            const float filterButtonWidth =
                (ImGui::GetColumnWidth() - (ImGui::GetStyle().ItemSpacing.x * columnFilterGroups.size() - 1)) /
                columnFilterGroups.size();

            if (columnFilterGroups.size() <= 3) {
                drawFilterGroupButtons(true, filterButtonWidth);
            } else {
                if (ImGui::Button(
                        std::format("{} filter groups##OpenFilterGroupListButton{}", columnFilterGroups.size(), column)
                            .c_str()))
                {
                    m_filterGroupListColumn = column;
                    ImGui::OpenPopup("FilterGroupListPopup");
                }
                if (m_filterGroupListColumn == column && ImGui::BeginPopup("FilterGroupListPopup")) {
                    drawFilterGroupButtons(false, ImGui::CalcTextSize("M").x * filter_consts::FILTER_GROUP_NAME_MAX_LENGTH);
                    ImGui::EndPopup();
                }
            }
            // Open filter editor NOTE: All the data here is set in drawFilterGroupButtons() lambda!
            if (openFilterEditor) {
                if (auto filterEditor = getSubGui<FilterEditorSubGui>("FilterEditorSubGui")) {
                    filterEditor->openGroupEdit(column, editorGroupIndex, itemAvoidRect);
                }
            }
        }

        // ROW 1: Custom column header row
        ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
        for (size_t column = 0; column < m_scheduleCore.getColumnCount() && column < ImGui::TableGetColumnCount(); column++) {
            ImGui::TableSetColumnIndex(column);
            bool isColumnHeaderHovered =
                (ImGui::TableGetHoveredColumn() == column && ImGui::TableGetHoveredRow() == ImGui::TableGetRowIndex());
            ImGui::PushID(column);
            float headerCursorY = ImGui::GetCursorPosY();
            size_t pushedStyleVars = 0;
            // HIDE the sort button if the column header is not hovered and the column does not have a sort direction applied
            if (isColumnHeaderHovered == false && m_scheduleCore.getColumn(column)->sort == COLUMN_SORT_NONE) {
                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.0f);
                pushedStyleVars++;
            }
            // sort button!
            if (ImGui::ArrowButton(
                    std::format("##sortColumn{}", column).c_str(),
                    m_scheduleCore.getColumn(column)->sort == COLUMN_SORT_NONE
                        ? ImGuiDir_Right
                        : (m_scheduleCore.getColumn(column)->sort == COLUMN_SORT_DESCENDING ? ImGuiDir_Down : ImGuiDir_Up)))
            {
                setColumnSort.invoke(
                    column,
                    m_scheduleCore.getColumn(column)->sort == COLUMN_SORT_NONE
                        ? COLUMN_SORT_DESCENDING
                        : (m_scheduleCore.getColumn(column)->sort == COLUMN_SORT_DESCENDING ? COLUMN_SORT_ASCENDING
                                                                                            : COLUMN_SORT_NONE));
            }
            ImGui::PopStyleVar(pushedStyleVars);
            ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
            ImGui::TableHeader(ImGui::TableGetColumnName(column));
            ImGuiID tableHeaderID = ImGui::GetItemID();
            ImGuiTable* currentTable = ImGui::GetCurrentTable();
            // Show a close button on the right when hovered
            // permanent columns can't be removed so there's no need for a remove button
            if (isColumnHeaderHovered && m_scheduleCore.getColumn(column)->permanent == false) {
                // This is how the arrow button's size is calculated
                float headerButtonSize = ImGui::CalcTextSize("W").y;
                // SameLine() can't be used after a TableHeader so the position has to be calculated manually.
                ImGui::SetCursorScreenPos(
                    ImVec2(ImGui::TableGetCellBgRect(ImGui::GetCurrentTable(), column).Max.x - headerButtonSize - 8.0f,
                           ImGui::GetCursorScreenPos().y));
                ImGui::SetCursorPosY(headerCursorY);
                size_t pushedColorCount = 0;
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
                pushedColorCount++;
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 1.0f, 1.0f, 0.2f));
                pushedColorCount++;
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 1.0f, 1.0f, 0.4f));
                pushedColorCount++;
                if (gui_templates::ImageButtonStyleColored("##RemoveColumn",
                                                           guiTextures.getOrLoad("icon_remove").ImID,
                                                           ImVec2(headerButtonSize, headerButtonSize)))
                {
                    removeColumn.invoke(column);
                    ImGui::PopStyleColor(pushedColorCount);
                    ImGui::PopID();
                    ImGui::EndTable();
                    return;
                }
                ImGui::PopStyleColor(pushedColorCount);
            }
            // column header context menu
            if (isColumnHeaderHovered  // Hovering the column header
                && m_nextMouseReleaseOpenColumnContext  // The current mouse release can open the popup
                && ((ImGui::GetMouseDragDelta(0).x == 0 &&
                     ImGui::IsMouseReleased(ImGuiMouseButton_Left))  // Clicked LMB without dragging
                    || (ImGui::GetMouseDragDelta(1).x == 0 &&
                        ImGui::IsMouseReleased(ImGuiMouseButton_Right)))  // OR clicked RMB without dragging
                && (ImGui::IsAnyItemHovered() == false ||
                    ImGui::GetHoveredID() == tableHeaderID))  // AND hovering the table header
            {
                ImGui::TableOpenContextMenu(column);
            }
            bool popupOpenBefore = ImGui::GetCurrentTable()->IsContextPopupOpen;
            if (ImGui::GetCurrentTable()->ContextPopupColumn == column &&
                ImGui::TableBeginContextMenuPopup(ImGui::GetCurrentTable()))
            {
                drawColumnHeaderContextContent(column, currentTable, tableFlags);
                ImGui::EndPopup();
            }
            // The column context menu was closed this frame (probably through a mouse click)
            if (popupOpenBefore == true && ImGui::GetCurrentTable()->IsContextPopupOpen == false) {
                m_nextMouseReleaseOpenColumnContext = false;
            }
            ImGui::PopID();
        }
        // The first mouse release after closing the column context popup does nothing but allows the next release to open it again
        if (m_nextMouseReleaseOpenColumnContext == false &&
            (ImGui::IsMouseReleased(ImGuiMouseButton_Left) || ImGui::IsMouseReleased(ImGuiMouseButton_Right)))
        {
            m_nextMouseReleaseOpenColumnContext = true;
        }

        // DRAW SCHEDULE ELEMENTS
        std::vector<size_t> sortedRowIndices = m_scheduleCore.getSortedRowIndices();

        for (size_t unsortedRow = 0; unsortedRow < m_scheduleCore.getRowCount() && unsortedRow < sortedRowIndices.size();
             unsortedRow++)
        {
            size_t row = sortedRowIndices[unsortedRow];

            // CHECK FILTERS BEFORE DRAWING ROW
            for (size_t column = 0; column < m_scheduleCore.getColumnCount(); column++) {
                // Check if the row's Element passes every FilterGroup in this Column
                bool passesAllFilters = m_scheduleCore.getColumn(column)->checkElementPassesFilters(
                    row,
                    m_scheduleDateOverride  // Pass override date as current (Uses TimeWrapper::getCurrentTime() if it's empty)
                );
                // fails to pass, don't show this row
                if (passesAllFilters == false) {
                    goto do_not_draw_row;
                }
            }

            ImGui::TableNextRow();
            for (size_t column = 0; column < m_scheduleCore.getColumnCount() && column < ImGui::TableGetColumnCount(); column++)
            {
                ImGui::TableSetColumnIndex(column);
                if (drawTableCellContents(column, row, window, input, guiTextures) == false) {
                    // Failed to draw the entire row. Probably shouldn't draw the others, either.
                    ImGui::EndTable();
                    return;
                }
                if (ImGui::TableGetColumnFlags(column) & ImGuiTableColumnFlags_IsHovered &&
                    ImGui::TableGetHoveredRow() == ImGui::TableGetRowIndex() && ImGui::IsMouseReleased(1))
                {
                    openCellContextPopup(column, row);
                }

                if (m_cellContextCoords.has_value() && m_cellContextCoords->is(column, row)) {
                    drawCellContextContent();
                }
            }
            // Draw row context AFTER drawing cell content because the row might be removed before it is drawn
            if (m_rowContextRow.has_value() && m_rowContextRow.value() == row) {
                drawRowContextContent();
            }
        // END OF for (size_t unsortedRow = 0; unsortedRow < m_scheduleCore.getRowCount(); unsortedRow++)
        do_not_draw_row:
            bool b = false;  // stupid thing because fsr the label can't be at the end of the loop
        }
        ImGui::EndTable();
    }
}

bool ScheduleGui::drawTableCellContents(size_t column, size_t row, Window& window, Input& input, GuiTextures& guiTextures) {
    ImGuiStyle& style = ImGui::GetStyle();
    bool rowMenuButtonHovered = false;
    // Row button is displayed in the first column
    if (ImGui::GetCurrentTable()->Columns[column].DisplayOrder == 0) {
        size_t pushedStyleVars = 0;
        // HIDE the row button unless the row is hovered
        if (ImGui::TableGetHoveredRow() != ImGui::TableGetRowIndex()) {
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.0f);
            pushedStyleVars++;
        }
        const float labelSize = ImGui::CalcTextSize("X").y;
        const float rowMenuButtonSize = labelSize - (int)labelSize % 8;  //+ style.FramePadding.y * 2.0f;
        const bool showAltButton = (input.buttonStates.ctrlDown || input.buttonStates.shiftDown);
        if (gui_templates::ImageButtonStyleColored(
                std::format("##RowMenu{}", row).c_str(),
                (showAltButton ? guiTextures.getOrLoad("icon_remove") : guiTextures.getOrLoad("icon_row_menu")).ImID,
                ImVec2(rowMenuButtonSize, rowMenuButtonSize)))
        {
            if (showAltButton) {
                removeRow.invoke(row);
                // Return because this row can't be drawn anymore, it was removed.
                return false;
            } else {
                // TODO: Start row drag & drop
                openRowContextPopup(row);
            }
        }
        if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
            // printf("DRAGGING %zu\n", row);
            m_draggedRow = row;
        }

        ImGui::PopStyleVar(pushedStyleVars);
        rowMenuButtonHovered = ImGui::IsItemHovered();
        ImGui::SameLine();
    }

    bool columnEditDisabled = false;
    // If viewing a different date and the column has a reset option then show it disabled
    if (m_scheduleDateOverride.getIsEmpty() == false &&
        m_scheduleCore.getColumn(column)->resetOption != ColumnResetOption::Never)
    {
        columnEditDisabled = true;
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, gui_colors::disabledAlpha);
    }

    SCHEDULE_TYPE columnType = m_scheduleCore.getColumn(column)->type;
    ImGui::SetNextItemWidth(-FLT_MIN);

    bool isTableCellHighlighted = false;
    // Hightlight the table cell that is currently being edited in the element editor subgui
    if (auto elementEditor = getSubGui<ElementEditorSubGui>("ElementEditorSubGui")) {
        isTableCellHighlighted = isTableCellHighlighted ||
            (elementEditor->getOpenThisFrame() && elementEditor->getCoordinates().has_value() &&
             elementEditor->getCoordinates()->is(column, row));
    }
    // Hightlight table cells in the row that currently has its row context menu open
    isTableCellHighlighted = isTableCellHighlighted ||
        (m_rowContextRow.has_value() && m_rowContextRow.value() == row && ImGui::IsPopupOpen("ScheduleTableRowContextPopup"));
    // Hightlight the table cell that currently has its context menu open
    isTableCellHighlighted = isTableCellHighlighted ||
        (m_cellContextCoords.has_value() && m_cellContextCoords->is(column, row) &&
         ImGui::IsPopupOpen("ScheduleTableCellContextPopup"));
    if (isTableCellHighlighted) {
        ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg,
                               ImGui::GetColorU32(gui_color_calculations::getTableCellHighlightColor(
                                   style.Colors[ImGuiCol_WindowBg], style.Colors[ImGuiCol_Text])));
    }

    switch (columnType) {
        case (SCH_BOOL): {
            bool newValue = getElementValue<bool>(column, row, columnEditDisabled);
            if (ImGui::Checkbox(std::format("##{};{}", column, row).c_str(), &newValue)) {
                setElementValueBool.invoke(column, row, newValue);
            }
            // I will make an exception for this. To avoid the infamous Double Check™ this will have an additional check for whether any item (cough cough, remove row button) is being hovered. Happy now?
            if (isEditableElementClicked(columnEditDisabled) && ImGui::IsAnyItemHovered() == false) {
                setElementValueBool.invoke(column, row, !newValue);
            }
            break;
        }
        case (SCH_NUMBER): {
            int newValue = getElementValue<int>(column, row, columnEditDisabled);
            if (gui_templates::InputInt(std::format("##{};{}", column, row).c_str(), &newValue, false)) {
                setElementValueNumber.invoke(column, row, newValue);
            }
            // TEMP HACK Workaround to not lose focus instantly when clicking?
            if (columnEditDisabled == false && ImGui::IsMouseReleased(ImGuiMouseButton_Left) &&
                ImGui::TableGetHoveredColumn() == ImGui::TableGetColumnIndex() &&
                ImGui::TableGetHoveredRow() == ImGui::TableGetRowIndex() && ImGui::GetCurrentTable()->HoveredColumnBorder == -1)
            {
                ImGui::SetKeyboardFocusHere(-1);
            }
            break;
        }
        case (SCH_DECIMAL): {
            double newValue = getElementValue<double>(column, row, columnEditDisabled);
            if (gui_templates::InputDouble(std::format("##{};{}", column, row).c_str(), &newValue, "%.15g", false)) {
                setElementValueDecimal.invoke(column, row, newValue);
            }
            // TEMP HACK Workaround to not lose focus instantly when clicking?
            if (columnEditDisabled == false && ImGui::IsMouseReleased(ImGuiMouseButton_Left) &&
                ImGui::TableGetHoveredColumn() == ImGui::TableGetColumnIndex() &&
                ImGui::TableGetHoveredRow() == ImGui::TableGetRowIndex() && ImGui::GetCurrentTable()->HoveredColumnBorder == -1)
            {
                ImGui::SetKeyboardFocusHere(-1);
            }
            break;
        }
        case (SCH_TEXT): {
            std::string value = getElementValue<std::string>(column, row, columnEditDisabled);
            std::string displayedValue = value;

            if (auto elementEditor = getSubGui<ElementEditorSubGui>("ElementEditorSubGui")) {
                std::optional<ScheduleCoordinates> editorCoords = elementEditor->getCoordinates();
                if (elementEditor->getOpenThisFrame() && editorCoords.has_value() && editorCoords->is(column, row)) {
                    // if editing this text element, use this TextWrapped as a preview, the value will actually only be applied if the editor's input is applied
                    displayedValue = elementEditor->getEditorValue(displayedValue);
                }
            }
            // element to display the value as a wrapped, multiline text
            ImGui::TextWrapped("%s", displayedValue.c_str());
            // Open text editor if clicked while hovering the current column & row
            if (isEditableElementClicked(columnEditDisabled) && rowMenuButtonHovered == false) {
                if (auto elementEditor = getSubGui<ElementEditorSubGui>("ElementEditorSubGui")) {
                    elementEditor->setEditorValue(value);
                    elementEditor->setTextInputBoxSize(ImVec2(ImGui::GetColumnWidth(column), 0));
                    elementEditor->open(
                        column,
                        row,
                        SCH_TEXT,
                        ImRect(ImGui::GetItemRectMin(), ImGui::TableGetCellBgRect(ImGui::GetCurrentTable(), column).Max));
                }
            }
            if (auto elementEditor = getSubGui<ElementEditorSubGui>("ElementEditorSubGui")) {
                std::optional<ScheduleCoordinates> editorCoords = elementEditor->getCoordinates();
                if (editorCoords.has_value() && editorCoords->is(column, row)) {
                    elementEditor->draw(window, input, guiTextures);
                    // was editing this Element, made edits and just closed the editor. apply the edits
                    if (elementEditor->getOpenLastFrame() && elementEditor->getOpenThisFrame() == false &&
                        elementEditor->getMadeEdits())
                    {
                        setElementValueText.invoke(column, row, elementEditor->getEditorValue(value));
                    }
                }
            }
            break;
        }
        case (SCH_SELECT): {
            SingleSelectContainer value = getElementValue<SingleSelectContainer>(column, row, columnEditDisabled);
            auto selection = value.getSelection();
            const std::vector<SelectOption>& options = m_scheduleCore.getColumn(column)->selectOptions.getOptions();

            if (selection.has_value()) {
                if (gui_templates::SelectOptionButton(options[selection.value()],
                                                      std::format("##{};{}", column, row).c_str(),
                                                      ImVec2(0, 0),
                                                      ImGuiButtonFlags_MouseButtonMiddle))
                {
                    // Middle clicking erases the option - bonus feature
                    if (columnEditDisabled == false)  // && ImGui::IsMouseReleased(ImGuiButtonFlags_MouseButtonMiddle))
                    {
                        value.setSelected(selection.value(), false);
                        setElementValueSelect.invoke(column, row, value);
                    }
                }
            }
            if (isEditableElementClicked(columnEditDisabled) && rowMenuButtonHovered == false) {
                if (auto elementEditor = getSubGui<ElementEditorSubGui>("ElementEditorSubGui")) {
                    elementEditor->open(column, row, SCH_SELECT, ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax()));
                    elementEditor->setEditorValue(value);
                }
            }
            if (auto elementEditor = getSubGui<ElementEditorSubGui>("ElementEditorSubGui")) {
                std::optional<ScheduleCoordinates> editorCoords = elementEditor->getCoordinates();
                if (editorCoords.has_value() && editorCoords->is(column, row)) {
                    elementEditor->draw(window, input, guiTextures);
                    // was editing this Element, made edits and just closed the editor. apply the edits
                    if (elementEditor->getOpenLastFrame() && elementEditor->getOpenThisFrame() == false &&
                        elementEditor->getMadeEdits())
                    {
                        setElementValueSelect.invoke(column, row, elementEditor->getEditorValue(value));
                    }
                }
            }
            break;
        }
        case (SCH_MULTISELECT): {
            SelectContainer value = getElementValue<SelectContainer>(column, row, columnEditDisabled);
            auto selection = value.getSelection();
            const std::vector<SelectOption>& options = m_scheduleCore.getColumn(column)->selectOptions.getOptions();

            std::vector<int> selectionIndices = {};

            size_t selectedCount = selection.size();

            for (size_t s : selection) {
                selectionIndices.push_back(s);
            }

            // sort indices so that the same options are always displayed in the same order
            std::sort(std::begin(selectionIndices), std::end(selectionIndices));

            size_t currentRowWidth = 0;
            const float pixelsPerCharacter = ImGui::CalcTextSize("W").x;
            const float columnWidth = ImGui::GetColumnWidth(column);

            for (size_t i = 0; i < selectedCount; i++) {
                const float nextOptionAddedWidth = (currentRowWidth == 0 ? 0.0f : style.ItemSpacing.x) +
                    options[selectionIndices[i]].name.length() * pixelsPerCharacter + style.FramePadding.x * 2.0f;
                if (currentRowWidth + nextOptionAddedWidth < columnWidth) {
                    if (i > 0)  // Don't add padding to the first option
                    {
                        ImGui::SameLine();
                    }
                } else {
                    currentRowWidth = 0;
                }
                if (gui_templates::SelectOptionButton(options[selectionIndices[i]],
                                                      std::format("##{};{}", column, row).c_str(),
                                                      ImVec2(0, 0),
                                                      ImGuiButtonFlags_MouseButtonMiddle))
                {
                    printf("Clicked\n");
                    // Middle clicking erases the option - bonus feature
                    if (columnEditDisabled == false)  // && ImGui::IsMouseReleased(ImGuiButtonFlags_MouseButtonMiddle))
                    {
                        printf("Released\n");
                        value.setSelected(selectionIndices[i], false);
                        setElementValueSelect.invoke(column, row, value);
                    }
                }

                currentRowWidth = currentRowWidth == 0 ? ImGui::GetItemRectSize().x
                                                       : currentRowWidth + style.ItemSpacing.x + ImGui::GetItemRectSize().x;
            }
            if (isEditableElementClicked(columnEditDisabled) && rowMenuButtonHovered == false) {
                if (auto elementEditor = getSubGui<ElementEditorSubGui>("ElementEditorSubGui")) {
                    elementEditor->open(column, row, SCH_MULTISELECT, ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax()));
                    elementEditor->setEditorValue(value);
                }
            }
            if (auto elementEditor = getSubGui<ElementEditorSubGui>("ElementEditorSubGui")) {
                std::optional<ScheduleCoordinates> editorCoords = elementEditor->getCoordinates();
                if (editorCoords.has_value() && editorCoords->is(column, row)) {
                    elementEditor->draw(window, input, guiTextures);
                    // was editing this Element, made edits and just closed the editor. apply the edits
                    if (elementEditor->getOpenLastFrame() && elementEditor->getOpenThisFrame() == false &&
                        elementEditor->getMadeEdits())
                    {
                        setElementValueSelect.invoke(column, row, elementEditor->getEditorValue(value));
                    }
                }
            }
            break;
        }
        case (SCH_WEEKDAY): {
            WeekdayContainer value = getElementValue<WeekdayContainer>(column, row, columnEditDisabled);
            auto selection = value.getSelection();
            const std::vector<std::string>& optionNames = general_consts::weekdayNames;

            std::vector<int> selectionIndices = {};

            size_t selectedCount = selection.size();

            for (size_t s : selection) {
                selectionIndices.push_back(s);
            }

            // sort indices so that the same options are always displayed in the same order
            std::sort(std::begin(selectionIndices), std::end(selectionIndices));

            size_t currentRowWidth = 0;
            const float pixelsPerCharacter = ImGui::CalcTextSize("W").x;
            const float columnWidth = ImGui::GetColumnWidth(column);

            for (size_t i = 0; i < selectedCount; i++) {
                const float nextOptionAddedWidth = (currentRowWidth == 0 ? 0.0f : style.ItemSpacing.x) +
                    optionNames[selectionIndices[i]].length() * pixelsPerCharacter + style.FramePadding.x * 2.0f;
                if (currentRowWidth + nextOptionAddedWidth < columnWidth) {
                    if (i > 0)  // Don't add padding to the first option
                    {
                        ImGui::SameLine();
                    }
                } else {
                    currentRowWidth = 0;
                }
                if (gui_templates::SelectOptionButton(
                        SelectOption{optionNames[selectionIndices[i]], gui_colors::dayColors[selectionIndices[i]]},
                        std::format("##{};{}", column, row).c_str(),
                        ImVec2(),
                        ImGuiButtonFlags_MouseButtonMiddle))
                {
                    // Middle clicking erases the option - bonus feature
                    if (columnEditDisabled == false)  // && ImGui::IsMouseReleased(ImGuiButtonFlags_MouseButtonMiddle))
                    {
                        value.setSelected(selectionIndices[i], false);
                        setElementValueSelect.invoke(column, row, value);
                    }
                }

                currentRowWidth = currentRowWidth == 0 ? ImGui::GetItemRectSize().x
                                                       : currentRowWidth + style.ItemSpacing.x + ImGui::GetItemRectSize().x;
            }
            // left clicking anywhere in the cell opens the editor
            if (isEditableElementClicked(columnEditDisabled) && rowMenuButtonHovered == false) {
                if (auto elementEditor = getSubGui<ElementEditorSubGui>("ElementEditorSubGui")) {
                    elementEditor->open(column, row, SCH_WEEKDAY, ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax()));
                    elementEditor->setEditorValue(value);
                }
            }
            if (auto elementEditor = getSubGui<ElementEditorSubGui>("ElementEditorSubGui")) {
                std::optional<ScheduleCoordinates> editorCoords = elementEditor->getCoordinates();
                if (editorCoords.has_value() && editorCoords->is(column, row)) {
                    elementEditor->draw(window, input, guiTextures);
                    // was editing this Element, made edits and just closed the editor. apply the edits
                    if (elementEditor->getOpenLastFrame() && elementEditor->getOpenThisFrame() == false &&
                        elementEditor->getMadeEdits())
                    {
                        setElementValueWeekday.invoke(column, row, elementEditor->getEditorValue(value));
                    }
                }
            }
            break;
        }
        case (SCH_TIME): {
            TimeContainer value = getElementValue<TimeContainer>(column, row, columnEditDisabled);
            ImGui::Text("%s", value.getString().c_str());
            if (isEditableElementClicked(columnEditDisabled)) {
                if (auto elementEditor = getSubGui<ElementEditorSubGui>("ElementEditorSubGui")) {
                    elementEditor->setEditorValue(value);
                    elementEditor->open(column, row, SCH_TIME, ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax()));
                }
            }
            if (auto elementEditor = getSubGui<ElementEditorSubGui>("ElementEditorSubGui")) {
                std::optional<ScheduleCoordinates> editorCoords = elementEditor->getCoordinates();
                if (editorCoords.has_value() && editorCoords->is(column, row)) {
                    elementEditor->draw(window, input, guiTextures);
                    // was editing this Element, made edits and just closed the editor. apply the edits
                    if (elementEditor->getOpenLastFrame() && elementEditor->getOpenThisFrame() == false &&
                        elementEditor->getMadeEdits())
                    {
                        setElementValueTime.invoke(column, row, elementEditor->getEditorValue(value));
                    }
                }
            }
            break;
        }
        case (SCH_DATE): {
            auto value = getElementValue<DateContainer>(column, row, columnEditDisabled);
            ImGui::Text("%s", value.getString().c_str());  // Display the date of the current Date element
            if (isEditableElementClicked(columnEditDisabled)) {
                if (auto elementEditor = getSubGui<ElementEditorSubGui>("ElementEditorSubGui")) {
                    elementEditor->setEditorValue(value);
                    elementEditor->open(column, row, SCH_DATE, ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax()));
                }
            }
            if (auto elementEditor = getSubGui<ElementEditorSubGui>("ElementEditorSubGui")) {
                std::optional<ScheduleCoordinates> editorCoords = elementEditor->getCoordinates();
                if (editorCoords.has_value() && editorCoords->is(column, row)) {
                    elementEditor->draw(window, input, guiTextures);
                    // was editing this Element, made edits and just closed the editor. apply the edits
                    if (elementEditor->getOpenLastFrame() && elementEditor->getOpenThisFrame() == false &&
                        elementEditor->getMadeEdits())
                    {
                        setElementValueDate.invoke(column, row, elementEditor->getEditorValue(value));
                    }
                }
            }
            break;
        }
    }
    if (columnEditDisabled) {
        ImGui::PopItemFlag();
        ImGui::PopStyleVar();
    }

    if (columnEditDisabled == true && ImGui::TableGetHoveredColumn() == ImGui::TableGetColumnIndex() &&
        ImGui::TableGetHoveredRow() == ImGui::TableGetRowIndex())
    {
        ImGui::BeginTooltip();
        ImGui::Text(
            "This column cannot be edited when viewing a different date\nbecause it has a reset option different from 'Never'.");
        ImGui::EndTooltip();
    }
    return true;
}

void ScheduleGui::clearDateOverride() {
    m_scheduleDateOverride.clear();
}