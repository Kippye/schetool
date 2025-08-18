#include <string>
#include <cstdio>
#include <algorithm>
#include <format>
#include "schedule_constants.h"
#include "table/schedule_gui.h"
#include "schedule/element_editor_subgui.h"
#include "schedule/filter_editor_subgui.h"
#include "main_menu_bar/main_menu_bar_gui.h"
#include "view_tab_bar_gui.h"
#include "gui_templates.h"
#include "element_display_templates.h"
#include "gui_constants.h"
#include "schedule_coordinates.h"

ScheduleGui::ScheduleGui(const char* ID, const ScheduleCore& scheduleCore, ScheduleEvents& scheduleEvents)
    : m_scheduleCore(scheduleCore), Gui(ID) {
    addSubGui(new ElementEditorSubGui("ElementEditorSubGui", m_scheduleCore));
    modifyColumnSelectOptions.addEvent(getSubGui<ElementEditorSubGui>("ElementEditorSubGui")->modifyColumnSelectOptions);
    addSubGui(new FilterEditorSubGui("FilterEditorSubGui", m_scheduleCore, scheduleEvents));

    scheduleEvents.viewedDateChanged.addListener(viewedDateChangedListener);

    auto filterEditor = getSubGui<FilterEditorSubGui>("FilterEditorSubGui");
    // Link up event pipes
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

void ScheduleGui::draw(const WindowSize& windowSize, Input& input, GuiTextures& guiTextures) {
    const float offsetFromTop = MainMenuBarGui::getHeight() + ViewTabBarGui::getHeight();
    ImGuiStyle style = ImGui::GetStyle();
    ImGui::SetNextWindowSize(ImVec2((float)windowSize.getWidth(), (float)windowSize.getHeight() - offsetFromTop));
    ImGui::SetNextWindowContentSize(ImVec2((float)windowSize.getWidth(), (float)windowSize.getHeight() - offsetFromTop) -
                                    style.WindowPadding * 2.0f);
    ImGui::SetNextWindowPos(ImVec2(0.0f, offsetFromTop));

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::Begin(m_ID.c_str(),
                 NULL,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);
    ImGui::PopStyleVar();

    // Avoid imgui 0 column abort by not beginning the table at all if there are no columns in the schedule
    if (m_scheduleCore.getColumnCount() > 0) {
        const float SCHEDULE_TOP_MARGIN = offsetFromTop;
        const float CHILD_WINDOW_WIDTH = (float)windowSize.getWidth();
        const float CHILD_WINDOW_HEIGHT = (float)(windowSize.getHeight() - SCHEDULE_TOP_MARGIN);

        ImGui::SetNextWindowPos(ImVec2(0.0, SCHEDULE_TOP_MARGIN));
        ImGui::PushStyleColor(ImGuiCol_Border, gui_colors::colorInvisible);
        ImGui::PushStyleColor(ImGuiCol_BorderShadow, gui_colors::colorInvisible);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, gui_style_vars::windowEdgePadding);
        ImGui::BeginChild("SchedulePanel", ImVec2(CHILD_WINDOW_WIDTH, CHILD_WINDOW_HEIGHT), true);
        ImGui::PopStyleVar();
        ImGui::PopStyleColor(2);

        // DRAW the SCHEDULE TABLE
        drawScheduleTable(windowSize, input, guiTextures);

        ImGui::EndChild();
    }
    ImGui::End();
}

// Must be run before BeginTable()!
void ScheduleGui::applyTableColumnOrder() {
    if (!m_scheduleTable) {
        return;
    }

    bool addReorderEdit = false;
    std::pair<size_t, size_t> reorderEditIndices;
    /// Update drag state
    // Column reordering just started
    if (m_scheduleTable->ReorderColumn != -1 && m_columnDragColumn.has_value() == false) {
        m_columnDragColumn = m_scheduleTable->ReorderColumn;
        m_columnDragBeginOrder = m_scheduleTable->Columns[m_scheduleTable->ReorderColumn].DisplayOrder;
    }
    // Column reordering just ended
    if (m_scheduleTable->ReorderColumn == -1 && m_columnDragColumn.has_value() && m_columnDragBeginOrder.has_value()) {
        size_t beginningOrder = m_columnDragBeginOrder.value();
        size_t finalOrder = m_scheduleTable->Columns[m_columnDragColumn.value()].DisplayOrder;
        // No point in adding an edit for moving the column to the same spot (it would happen otherwise)
        if (beginningOrder != finalOrder) {
            addReorderEdit = true;
            reorderEditIndices = {beginningOrder, finalOrder};
        }
        m_columnDragColumn.reset();
        m_columnDragBeginOrder.reset();
    }

    // Translate imgui column reorder into schedule column reorder.
    if (m_scheduleTable->ReorderColumn != -1 && m_scheduleTable->ReorderColumnDir != 0) {
        // We need to handle reordering across hidden columns.
        const int reorderDir = m_scheduleTable->ReorderColumnDir;
        ImGuiTableColumn* srcColumn = &m_scheduleTable->Columns[m_scheduleTable->ReorderColumn];
        ImGuiTableColumn* dstColumn =
            &m_scheduleTable->Columns[(reorderDir == -1) ? srcColumn->PrevEnabledColumn : srcColumn->NextEnabledColumn];
        // So we need to move the column's display order from srcColumn->DisplayOrder to dstColumn->DisplayOrder
        setColumnOrder.invoke(srcColumn->DisplayOrder, dstColumn->DisplayOrder);
        // Reset reorder dir to 0 cus we already handling this shit 8)
        m_scheduleTable->ReorderColumnDir = 0;
    }

    // Make the imgui column display order match schedule's column order
    for (int order = 0; order < m_scheduleTable->ColumnsCount; order++) {
        if (m_scheduleCore.existsColumnAtIndex(order, false) == false) {
            // No schedule column at the index - it's probably the "add column" column.
            continue;
        }
        if (m_scheduleCore.getInternalIndexFor(order).has_value() == false) {
            std::cout << std::format("ScheduleGui::drawScheduleTable(): No internal index for display order {}", order)
                      << std::endl;
            continue;
        }
        int column = m_scheduleCore.getInternalIndexFor(order).value();
        if (column < m_scheduleTable->ColumnsCount) {
            m_scheduleTable->Columns[column].DisplayOrder = order;
            m_scheduleTable->DisplayOrderToIndex[order] = (ImGuiTableColumnIdx)column;
        }
    }

    // Add a reorder edit from the index at the very beginning to the index at the very end
    // So that multiple 1-by-1 movements can be undone / redone in one command
    if (addReorderEdit) {
        createColumnReorderEdit.invoke(reorderEditIndices.first, reorderEditIndices.second);
    }
}

void ScheduleGui::drawScheduleTable(const WindowSize& windowSize, Input& input, GuiTextures& guiTextures) {
    ImGuiStyle& style = ImGui::GetStyle();
    ImGuiTableFlags tableFlags = ImGuiTableFlags_Reorderable | ImGuiTableFlags_ScrollY | ImGuiTableFlags_Borders |
        ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingFixedSame | ImGuiTableFlags_ScrollX | ImGuiTableFlags_NoSavedOrder |
        ImGuiTableFlags_HighlightHoveredColumn;
    // Correctly order the table columns
    applyTableColumnOrder();

    if (ImGui::BeginTable("ScheduleTable", m_scheduleCore.getColumnCount() + 1, tableFlags, ImGui::GetContentRegionAvail())) {
        ImGuiTable* currentTable = ImGui::GetCurrentTable();
        m_scheduleTable = currentTable;
        currentTable->DisableDefaultContextMenu = true;
        for (size_t column = 0; column < m_scheduleCore.getColumnCount(); column++) {
            ImGui::TableSetupColumn(m_scheduleCore.getColumnConst(column).name.c_str());
        }
        ImGuiTableColumnFlags addColumnFlags = ImGuiTableColumnFlags_NoResize | ImGuiTableColumnFlags_NoReorder;
        ImGui::TableSetupColumn("+ Add column", addColumnFlags);

        ImGui::TableSetupScrollFreeze(0, 2);

        // ROW 0: Filters
        ImGui::TableNextRow();
        for (size_t column = 0; column < m_scheduleCore.getColumnCount() && column < ImGui::TableGetColumnCount(); column++) {
            ImGui::TableSetColumnIndex(m_scheduleCore.getInternalIndexFor(column).value());

            const ImVec2 label_size = ImGui::CalcTextSize("+");
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
                    filterEditor->draw(windowSize, input, guiTextures);
                }
            }

            ImGui::SameLine();

            const Column& currentColumn = m_scheduleCore.getColumnConst(column);
            const auto& columnFilterGroups = currentColumn.getFilterGroupsConst();

            // DATA TO PASS TO FILTER EDITOR
            bool openFilterEditor = false;
            size_t editorGroupIndex = 0;
            ImRect itemAvoidRect;

            // LAMBDA: Draws buttons for every FilterGroup in the column. Sets data to pass to filterEditor if a button is clicked.
            auto drawFilterGroupButtons = [&](bool sameLine, float buttonWidth) {
                for (size_t i = 0; i < columnFilterGroups.size(); i++) {
                    const auto& filterGroup = currentColumn.getFilterGroupConst(i);
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
                    if (ImGui::BeginItemTooltip()) {
                        ImGui::Text("%s", filterGroup.getName().c_str());
                        ImGui::EndTooltip();
                    }

                    if (sameLine && i < currentColumn.getFilterGroupCount() - 1) {
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
        }  // Filters row

        // ROW 1: Custom column header row
        ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
        for (size_t column = 0; column < m_scheduleCore.getColumnCount() && column < ImGui::TableGetColumnCount(); column++) {
            ImGui::TableSetColumnIndex(m_scheduleCore.getInternalIndexFor(column).value());
            bool isColumnHeaderHovered = (ImGui::TableGetHoveredColumn() == ImGui::TableGetColumnIndex() &&
                                          ImGui::TableGetHoveredRow() == ImGui::TableGetRowIndex());
            ImGui::PushID(ImGui::TableGetColumnIndex());
            float headerCursorY = ImGui::GetCursorPosY();
            size_t pushedStyleColors = 0;
            GuiTextureInfo sortButtonTexture;
            const SCHEDULE_TYPE columnType = m_scheduleCore.getColumnConst(column).type;
            const COLUMN_SORT columnSort = m_scheduleCore.getColumnConst(column).sort;

            guiTextures.exists(schedule_consts::scheduleTypeIconNames.contains(columnType)
                                   ? schedule_consts::scheduleTypeIconNames.at(columnType)
                                   : "MISSING_ICON",
                               sortButtonTexture);
            // Hide the button background if the column header is not hovered and the column does not have a sort direction applied
            if (isColumnHeaderHovered == false && m_scheduleCore.getColumnConst(column).sort == COLUMN_SORT_NONE) {
                ImGui::PushStyleColor(ImGuiCol_Button, gui_colors::colorInvisible);
                pushedStyleColors++;
            } else {  // If the column header is hovered OR the column has a sort direction, display the correct sort icon
                guiTextures.exists(columnSort == COLUMN_SORT_NONE
                                       ? "icon_sort_none"
                                       : (columnSort == COLUMN_SORT_ASCENDING ? "icon_sort_ascending" : "icon_sort_descending"),
                                   sortButtonTexture);
            }
            // Sort button!
            const float sortButtonSize = ImGui::GetFrameHeight() - style.FramePadding.y * 2.0f;
            if (gui_templates::ImageButtonStyleColored(std::format("##sortColumnOrTypeIcon{}", column).c_str(),
                                                       sortButtonTexture.ImID,
                                                       ImVec2(sortButtonSize, sortButtonSize)))
            {
                setColumnSort.invoke(column,
                                     columnSort == COLUMN_SORT_NONE
                                         ? COLUMN_SORT_DESCENDING
                                         : (columnSort == COLUMN_SORT_DESCENDING ? COLUMN_SORT_ASCENDING : COLUMN_SORT_NONE));
            }
            ImGui::PopStyleColor(pushedStyleColors);
            ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
            // TODO: Align this header text to the sort button better
            ImGui::TableHeader(m_scheduleCore.getColumnConst(column).name.c_str());
            ImGuiID tableHeaderID = ImGui::GetItemID();
            // Show a remove button on the right when hovered
            // permanent columns can't be removed so there's no need for a remove button
            if (isColumnHeaderHovered && m_scheduleCore.getColumnConst(column).permanent == false) {
                // This is how the arrow button's size is calculated
                float headerButtonSize = ImGui::CalcTextSize("W").y;
                // SameLine() can't be used after a TableHeader so the position has to be calculated manually.
                ImGui::SetCursorScreenPos(
                    ImVec2(ImGui::TableGetCellBgRect(ImGui::GetCurrentTable(), ImGui::TableGetColumnIndex()).Max.x -
                               headerButtonSize - 8.0f,
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
                ImGui::TableOpenContextMenu(ImGui::TableGetColumnIndex());
            }
            bool popupOpenBefore = ImGui::GetCurrentTable()->IsContextPopupOpen;
            if (ImGui::GetCurrentTable()->ContextPopupColumn == ImGui::TableGetColumnIndex() &&
                ImGui::TableBeginContextMenuPopup(ImGui::GetCurrentTable()))
            {
                drawColumnHeaderContext(column, currentTable, tableFlags);
                ImGui::EndPopup();
            }
            // The column context menu was closed this frame (probably through a mouse click)
            if (popupOpenBefore == true && ImGui::GetCurrentTable()->IsContextPopupOpen == false) {
                m_nextMouseReleaseOpenColumnContext = false;
            }
            ImGui::PopID();
        }  // Custom header row
        // "Add column" header
        {
            ImGui::TableSetColumnIndex(ImGui::TableGetColumnCount() - 1);
            bool isColumnHeaderHovered = (ImGui::TableGetHoveredColumn() == ImGui::TableGetColumnIndex() &&
                                          ImGui::TableGetHoveredRow() == ImGui::TableGetRowIndex());
            ImGui::TableHeader("+ Add column");
            if (isColumnHeaderHovered  // Hovering the column header
                && m_nextMouseReleaseOpenColumnContext  // The current mouse release can open the popup
                && ((ImGui::GetMouseDragDelta(0).x == 0 &&
                     ImGui::IsMouseReleased(ImGuiMouseButton_Left))  // Clicked LMB without dragging
                    || (ImGui::GetMouseDragDelta(1).x == 0 &&
                        ImGui::IsMouseReleased(ImGuiMouseButton_Right)))  // OR clicked RMB without dragging
            )
            {
                ImGui::TableOpenContextMenu(ImGui::TableGetColumnIndex());
            }
            bool popupOpenBefore = ImGui::GetCurrentTable()->IsContextPopupOpen;
            if (ImGui::GetCurrentTable()->ContextPopupColumn == ImGui::TableGetColumnIndex() &&
                ImGui::TableBeginContextMenuPopup(ImGui::GetCurrentTable()))
            {
                drawAddColumnHeaderContext(input);
                ImGui::EndPopup();
            }
            // The column context menu was closed this frame (probably through a mouse click)
            if (popupOpenBefore == true && ImGui::GetCurrentTable()->IsContextPopupOpen == false) {
                m_nextMouseReleaseOpenColumnContext = false;
            }
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
            if (!m_scheduleCore.checkPassesAllFilters(row, m_scheduleDateOverride)) {
                goto do_not_draw_row;
            }

            ImGui::TableNextRow();
            for (size_t column = 0; column < m_scheduleCore.getColumnCount() && column < ImGui::TableGetColumnCount(); column++)
            {
                ImGui::TableSetColumnIndex(m_scheduleCore.getInternalIndexFor(column).value());
                if (drawTableCellContents(column, row, windowSize, input, guiTextures) == false) {
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
                    drawCellContext();
                }
            }
            // Draw row context AFTER drawing cell content because the row might be removed before it is drawn
            if (m_rowContextRow.has_value() && m_rowContextRow.value() == row) {
                drawRowContext();
            }
        // END OF for (size_t unsortedRow = 0; unsortedRow < m_scheduleCore.getRowCount(); unsortedRow++)
        do_not_draw_row:
            bool b = false;  // stupid thing because fsr the label can't be at the end of the loop
        }
        // "Add row" row
        ImGui::TableNextRow();
        // for (size_t column = 0; column < m_scheduleCore.getColumnCount() && column < ImGui::TableGetColumnCount(); column++) {
        ImGui::TableSetColumnIndex(m_scheduleCore.getInternalIndexFor(0).value());
        ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
        if (ImGui::Selectable("+ Add row", false, ImGuiSelectableFlags_SpanAllColumns, ImVec2(0.0f, ImGui::GetFrameHeight()))) {
            addRow.invoke(m_scheduleCore.getRowCount());
        }
        ImGui::PopStyleVar();
        ImGui::EndTable();
        ImGuiTableColumn& lastColumn =
            m_scheduleTable->Columns[m_scheduleTable->DisplayOrderToIndex[m_scheduleTable->RightMostEnabledColumn]];
        ImVec2 topLeft = ImVec2(m_scheduleTable->InnerRect.GetTL());
        ImVec2 bottomRight = ImVec2(lastColumn.ClipRect.GetBR().x, m_scheduleTable->RowPosY2);
        m_tableContentRect = ImRect(topLeft, bottomRight);
    }
}

bool ScheduleGui::drawTableCellContents(
    size_t column, size_t row, const WindowSize& windowSize, Input& input, GuiTextures& guiTextures) {
    ImGuiStyle& style = ImGui::GetStyle();
    bool rowMenuButtonHovered = false;
    // Row button is displayed in the first column
    // Column and imgui display order should match
    if (column == 0) {
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
                (showAltButton ? guiTextures.getOrLoad("icon_remove") : guiTextures.getOrLoad("icon_menu_kebab")).ImID,
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
        m_scheduleCore.getColumnConst(column).resetOption != ColumnResetOption::Never)
    {
        columnEditDisabled = true;
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, gui_colors::disabledAlpha);
    }

    SCHEDULE_TYPE columnType = m_scheduleCore.getColumnConst(column).type;
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

    ScheduleCoordinates coords = ScheduleCoordinates(column, row);
    GuiPassReferences guiPass = GuiPassReferences(windowSize, input, guiTextures);

    switch (columnType) {
        case (SCH_BOOL): {
            bool newValue = getElementValue<bool>(column, row, columnEditDisabled);
            if (element_display_templates::ElementDisplay(newValue, coords, true)) {
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
            if (element_display_templates::ElementDisplay(newValue, coords, true)) {
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
            if (element_display_templates::ElementDisplay(newValue, coords, true)) {
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
            if (element_display_templates::ElementDisplay(
                    value,
                    coords,
                    getSubGui<ElementEditorSubGui>("ElementEditorSubGui"),
                    guiPass,
                    (isEditableElementClicked(columnEditDisabled) && rowMenuButtonHovered == false),
                    ImGui::GetColumnWidth(column),
                    ImRect(ImGui::TableGetCellBgRect(ImGui::GetCurrentTable(), column))))
            {
                setElementValueText.invoke(column, row, value);
            }
            break;
        }
        case (SCH_SELECT): {
            SingleSelectContainer value = getElementValue<SingleSelectContainer>(column, row, columnEditDisabled);

            if (element_display_templates::ElementDisplay(
                    value,
                    m_scheduleCore,
                    coords,
                    getSubGui<ElementEditorSubGui>("ElementEditorSubGui"),
                    guiPass,
                    (isEditableElementClicked(columnEditDisabled) && rowMenuButtonHovered == false)))
            {
                setElementValueSelect.invoke(column, row, value);
            }
            break;
        }
        case (SCH_MULTISELECT): {
            SelectContainer value = getElementValue<SelectContainer>(column, row, columnEditDisabled);

            if (element_display_templates::ElementDisplay(
                    value,
                    m_scheduleCore,
                    coords,
                    getSubGui<ElementEditorSubGui>("ElementEditorSubGui"),
                    guiPass,
                    ImGui::GetColumnWidth(column),
                    (isEditableElementClicked(columnEditDisabled) && rowMenuButtonHovered == false),
                    ImRect(ImGui::TableGetCellBgRect(ImGui::GetCurrentTable(), column))))
            {
                setElementValueMultiselect.invoke(column, row, value);
            }
            break;
        }
        case (SCH_WEEKDAY): {
            WeekdayContainer value = getElementValue<WeekdayContainer>(column, row, columnEditDisabled);

            if (element_display_templates::ElementDisplay(
                    value,
                    coords,
                    getSubGui<ElementEditorSubGui>("ElementEditorSubGui"),
                    guiPass,
                    ImGui::GetColumnWidth(column),
                    (isEditableElementClicked(columnEditDisabled) && rowMenuButtonHovered == false),
                    ImRect(ImGui::TableGetCellBgRect(ImGui::GetCurrentTable(), column))))
            {
                setElementValueWeekday.invoke(column, row, value);
            }
            break;
        }
        case (SCH_TIME): {
            TimeContainer value = getElementValue<TimeContainer>(column, row, columnEditDisabled);

            if (element_display_templates::ElementDisplay(
                    value,
                    coords,
                    getSubGui<ElementEditorSubGui>("ElementEditorSubGui"),
                    guiPass,
                    (isEditableElementClicked(columnEditDisabled) && rowMenuButtonHovered == false)))
            {
                setElementValueTime.invoke(column, row, value);
            }
            break;
        }
        case (SCH_DATE): {
            DateContainer value = getElementValue<DateContainer>(column, row, columnEditDisabled);

            if (element_display_templates::ElementDisplay(
                    value,
                    coords,
                    getSubGui<ElementEditorSubGui>("ElementEditorSubGui"),
                    guiPass,
                    (isEditableElementClicked(columnEditDisabled) && rowMenuButtonHovered == false)))
            {
                setElementValueDate.invoke(column, row, value);
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

void ScheduleGui::drawColumnHeaderContext(size_t columnIndex, ImGuiTable* table, ImGuiTableFlags tableFlags) {
    // We need to access the column only through this.
    // Because duplicating a column will invalidate the reference.
    auto getContextColumn = [&]() -> const Column& { return m_scheduleCore.getColumnConst(columnIndex); };

    // Renaming
    std::string name = getContextColumn().name.c_str();
    name.reserve(COLUMN_NAME_MAX_LENGTH);
    char* buf = name.data();

    if (ImGui::InputText(
            std::format("##columnName{}", columnIndex).c_str(), buf, name.capacity(), ImGuiInputTextFlags_EnterReturnsTrue))
    {
        setColumnName.invoke(columnIndex, buf);
    }

    // Select type (for non-permanent columns)
    ImGuiComboFlags typeDropdownFlags = ImGuiComboFlags_None;
    ImGui::Separator();
    if (getContextColumn().permanent) {
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        typeDropdownFlags |= ImGuiComboFlags_NoArrowButton;
    }
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Type:");
    ImGui::SameLine();
    if (std::optional<SCHEDULE_TYPE> newColumnType = gui_templates::Dropdown(
            "##ColumnType", getContextColumn().type, schedule_consts::scheduleTypeNames, typeDropdownFlags))
    {
        setColumnType.invoke(columnIndex, newColumnType.value());
    }
    if (getContextColumn().permanent) {
        ImGui::PopItemFlag();
    }

    ImGui::Separator();

    if (ImGui::MenuItem("Remove", NULL, false, !getContextColumn().permanent)) {
        removeColumn.invoke(columnIndex);
        ImGui::CloseCurrentPopup();
        return;  // Can't continue
    }

    if (ImGui::MenuItem("Duplicate", NULL, false, !getContextColumn().permanent)) {
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
    if (std::optional<ColumnResetOption> newColumnResetOption = gui_templates::Dropdown(
            "##ColumnResetSetting", getContextColumn().resetOption, schedule_consts::columnResetOptionStrings))
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
            if (getContextColumn().permanent) {
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

void ScheduleGui::drawAddColumnHeaderContext(const Input& input) {
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
}

void ScheduleGui::openRowContextPopup(size_t row) {
    m_rowContextRow = row;
    ImGui::OpenPopup("ScheduleTableRowContextPopup", ImGuiPopupFlags_NoOpenOverExistingPopup);
}

void ScheduleGui::drawRowContext() {
    if (m_rowContextRow.has_value() == false) {
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
            m_rowContextRow.reset();
        }
        ImGui::EndPopup();
    }
}

void ScheduleGui::openCellContextPopup(size_t column, size_t row) {
    m_cellContextCoords = {column, row};
    ImGui::OpenPopup("ScheduleTableCellContextPopup", ImGuiPopupFlags_NoOpenOverExistingPopup);
}

void ScheduleGui::drawCellContext() {
    if (m_cellContextCoords.has_value() == false) {
        return;
    }
    if (ImGui::BeginPopup("ScheduleTableCellContextPopup", ImGuiWindowFlags_NoMove)) {
        if (m_cellContextCoords->column() < m_scheduleCore.getColumnCount()) {
            auto [col, row] = m_cellContextCoords->getAsPair();
            ImGui::Text("%s", std::format("Cell (Column: {}; Row: {})", col, row).c_str());
        }

        if (ImGui::Button("Close")) {
            ImGui::CloseCurrentPopup();
            m_cellContextCoords.reset();
        }
        ImGui::EndPopup();
    }
}