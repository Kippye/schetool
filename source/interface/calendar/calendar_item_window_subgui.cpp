#include "calendar/calendar_item_window_subgui.h"
#include "schedule/element_editor_subgui.h"
#include "interface_style.h"
#include "schedule_constants.h"
#include "schedule_column.h"
#include "imgui_stdlib.h"
#include <string>

CalendarItemWindowSubGui::CalendarItemWindowSubGui(const char* ID,
                                                   const ScheduleCore& scheduleCore,
                                                   ScheduleEvents& scheduleEvents)
    : Gui(ID), m_scheduleCore(scheduleCore) {
    addSubGui(new ElementEditorSubGui("ElementEditorSubGui", m_scheduleCore, scheduleEvents));
    modifyColumnSelectOptions.addEvent(getSubGui<ElementEditorSubGui>("ElementEditorSubGui")->modifyColumnSelectOptions);
}

bool CalendarItemWindowSubGui::isEditablePropertyClicked(bool isEditingDisabled) const {
    return isEditingDisabled == false && ImGui::IsMouseClicked(ImGuiMouseButton_Left) &&
        ImGui::TableGetHoveredColumn() == ImGui::TableGetColumnIndex() &&
        ImGui::TableGetHoveredRow() == ImGui::TableGetRowIndex();
}

void CalendarItemWindowSubGui::draw(const WindowSize& windowSize, Input& input, GuiTextures& guiTextures) {
    ImGuiStyle& style = ImGui::GetStyle();
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, style.WindowPadding * 4.0f);
    ImGui::SetNextWindowSize(ImVec2(windowSize.getWidth() * 0.5f, windowSize.getHeight() * 0.8f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, style.WindowRounding);

    if (ImGui::BeginPopupModal("CalendarItemWindowPopup", NULL, windowFlags)) {
        ImGui::PopStyleVar(2);
        // If there is no row for the viewed item, the popup closes itself.
        if (m_currentItemRow.has_value() == false) {
            m_editingItemName = false;
            ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
            return;
        }
        size_t row = m_currentItemRow.value();
        // Draw the name bigger than other properties
        const size_t nameColumnIndex = m_scheduleCore.getFlaggedColumnIndex(ScheduleColumnFlags_Name);
        ImFont* nameFontData = InterfaceStyleHandler::getFontData((FontSize)((int)InterfaceStyleHandler::getFontSize() + 1));
        bool nameColumnEditDisabled = m_scheduleDateOverride.getIsEmpty() == false &&
            m_scheduleCore.getColumnConst(nameColumnIndex).resetOption != ColumnResetOption::Never;
        std::string value = getElementValue<std::string>({nameColumnIndex, row}, nameColumnEditDisabled);
        ImGui::PushFont(nameFontData);
        float height = ImGui::CalcTextSize(value.c_str()).y + style.FramePadding.y * 2.0f;
        if (m_editingItemName == false) {
            ImGui::PushStyleColor(ImGuiCol_FrameBg, gui_colors::colorInvisible);
            ImGui::InputText("##ItemNameDummyInput", &value, ImGuiInputTextFlags_ReadOnly);
            ImGui::PopStyleColor();
            if (ImGui::IsItemClicked() || ImGui::IsItemActivated()) {
                m_editingItemName = true;
                m_editingItemNameJustStarted = true;
            }
        } else {
            gui_templates::TextEditor(value, ImVec2(ImGui::GetContentRegionAvail().x, height), m_editingItemNameJustStarted);
            if (ImGui::IsItemDeactivatedAfterEdit()) {
                setElementValueText.invoke(nameColumnIndex, row, value);
                m_editingItemName = false;
            }
            if (ImGui::IsItemDeactivated()) {
                m_editingItemName = false;
            }
            m_editingItemNameJustStarted = false;
        }
        // ImGui::PopStyleColor();
        ImGui::PopFont();
        ImGuiTableFlags propertyTableFlags = ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_NoSavedSettings;
        if (ImGui::BeginTable("PropertyColumnsTable", 2, propertyTableFlags)) {
            // Show [PROPERTY_NAME_VISIBLE_CHARS] letters of the property name before it cuts off off
            ImGui::TableSetupColumn(
                "PropertyName", ImGuiTableColumnFlags_WidthFixed, ImGui::CalcTextSize("M").x * PROPERTY_NAME_VISIBLE_CHARS);
            ImGui::TableSetupColumn("PropertyValue", ImGuiTableColumnFlags_WidthStretch);

            ImGui::TableNextRow();

            const float labelSize = ImGui::CalcTextSize("X").y;
            const float contextButtonSize = labelSize - (int)labelSize % 8;  //+ style.FramePadding.y * 2.0f;

            bool dragEnded = false;
            // Allow duplicate ID-s while reordering properties
            ImGui::PushItemFlag(ImGuiItemFlags_AllowDuplicateId, true);
            for (size_t scheduleCol = 0; scheduleCol < m_scheduleCore.getColumnCount(); scheduleCol++) {
                // The index of the actual column / property to display. HACK y stuff here
                size_t col = scheduleCol;
                ImGui::TableNextColumn();
                // Dragging a property
                if (m_draggedPropertyColumn.has_value() && m_draggedPropertySrcOrder.has_value() &&
                    m_draggedPropertyOrder.has_value())
                {
                    const size_t draggedPropertyCol = m_draggedPropertyColumn.value();
                    const size_t draggedPropertySrcOrder = m_draggedPropertySrcOrder.value();
                    const size_t draggedPropertyOrder = m_draggedPropertyOrder.value();
                    // Draw dragged property in between if needed
                    if (ImGui::TableGetRowIndex() == draggedPropertyOrder) {
                        col = draggedPropertyCol;
                    } else if (draggedPropertyOrder < draggedPropertySrcOrder) {  // Dragging up / to lower index
                        if (ImGui::TableGetRowIndex() > draggedPropertyOrder && col <= draggedPropertyCol) {
                            // Every column after the dragged one's display index and before or same as its column index should be decremented by 1
                            col--;
                        }
                    } else if (draggedPropertyOrder > draggedPropertySrcOrder) {  // Dragging down / to higher index
                        if (ImGui::TableGetRowIndex() < draggedPropertyOrder && col >= draggedPropertyCol) {
                            // Every column before the dragged one's display index and before or same as its column index should be decremented by 1
                            col++;
                        }
                    }
                }
                /// Property context menu button
                GuiTextureInfo contextButtonTexture;
                const bool isRemoveButton = (input.buttonStates.ctrlDown || input.buttonStates.shiftDown);

                // Property row not hovered - show the type's icon
                if (ImGui::TableGetHoveredRow() != ImGui::TableGetRowIndex()) {
                    const SCHEDULE_TYPE columnType = m_scheduleCore.getColumnConst(col).type;
                    guiTextures.exists(schedule_consts::scheduleTypeIconNames.contains(columnType)
                                           ? schedule_consts::scheduleTypeIconNames.at(columnType)
                                           : "MISSING_ICON",
                                       contextButtonTexture);
                    ImGui::PushStyleColor(ImGuiCol_Button, gui_colors::colorInvisible);
                } else {  // Property row is hovered - show kebab or remove button
                    if (!isRemoveButton) {
                        guiTextures.exists("icon_menu_kebab", contextButtonTexture);
                    } else {
                        guiTextures.exists("icon_remove", contextButtonTexture);
                    }
                }

                if (gui_templates::ImageButtonStyleColored(std::format("##propertyContextButton{}", col).c_str(),
                                                           contextButtonTexture.ImID,
                                                           ImVec2(contextButtonSize, contextButtonSize)))
                {
                    if (isRemoveButton) {
                        removeColumn.invoke(col);
                        break;
                    }
                }
                if (ImGui::TableGetHoveredRow() != ImGui::TableGetRowIndex()) {
                    ImGui::PopStyleColor();  // ImGuiCol_Button = invisible
                }
                if (!isRemoveButton) {
                    bool hoveringThisRow = ImGui::TableGetHoveredRow() == ImGui::TableGetRowIndex();
                    bool hoveringAnyRow =
                        ImGui::TableGetHoveredRow() >= 0 && ImGui::TableGetHoveredRow() < m_scheduleCore.getColumnCount();
                    int hoveredRowDirection = std::clamp(ImGui::TableGetHoveredRow() - ImGui::TableGetRowIndex(), -1, 1);
                    // Highlight the active row
                    if (ImGui::IsItemActive()) {
                        ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1,
                                               ImGui::GetColorU32(gui_color_calculations::getTableCellHighlightColor(
                                                   style.Colors[ImGuiCol_WindowBg], style.Colors[ImGuiCol_Text])));
                    }
                    // This button is active BUT this row is not hovered BUT some row *is* being hovered + the mouse has moved up or down.
                    if (ImGui::IsItemActive() && !hoveringThisRow && hoveringAnyRow && ImGui::GetMouseDragDelta(0).y != 0.0f) {
                        const int currentOrder = ImGui::TableGetRowIndex();
                        const int delta = ImGui::GetMouseDragDelta(0).y < 0.f ? -1 : 1;
                        const int nextOrder = currentOrder + delta;
                        // Next order is valid (also, the direction to the hovered row is the same as the mouse delta)
                        if (hoveredRowDirection == delta && nextOrder >= 0 && nextOrder < m_scheduleCore.getColumnCount()) {
                            if (m_draggedPropertyColumn.has_value() == false) {
                                m_draggedPropertySrcOrder = currentOrder;
                                m_draggedPropertyColumn = col;
                            }
                            m_draggedPropertyOrder = nextOrder;
                            ImGui::ResetMouseDragDelta();
                        }
                    } else if (ImGui::IsItemDeactivated()) {  // Drag just ended maybe?
                        dragEnded = true;
                    }
                }
                bool needToBreak = false;
                if (!isRemoveButton && ImGui::GetMouseDragDelta(ImGuiMouseButton_Left).y == 0.0f) {
                    drawPropertyContext(col, needToBreak);
                }
                // Quit early if the property context menu adds or removes a column
                if (needToBreak) {
                    break;
                }
                ImGui::SameLine();
                ImGui::AlignTextToFramePadding();
                ImGui::Text("%s", m_scheduleCore.getColumnConst(col).name.c_str());
                if (ImGui::BeginItemTooltip()) {  // Show the property's full name in a tooltip when it is hovered
                    ImGui::Text("%s", m_scheduleCore.getColumnConst(col).name.c_str());
                    ImGui::EndTooltip();
                }
                ImGui::TableNextColumn();
                // Draw the property value column
                drawItemProperty({windowSize, input, guiTextures}, {col, row});
            }
            ImGui::PopItemFlag();
            // Apply drag & drop + Reset drag & drop state
            if (dragEnded) {
                if (m_draggedPropertyColumn.has_value() && m_draggedPropertyOrder.has_value()) {
                    const size_t srcOrder = m_draggedPropertyColumn.value();
                    const size_t dstOrder = m_draggedPropertyOrder.value();
                    if (srcOrder != dstOrder) {
                        setColumnOrder.invoke(srcOrder, dstOrder);
                    }
                }
                m_draggedPropertyColumn.reset();
                m_draggedPropertySrcOrder.reset();
                m_draggedPropertyOrder.reset();
            }
            const float tableWidth = ImGui::GetCurrentTable()->OuterRect.GetWidth();
            ImGui::EndTable();
            // Add property / column button
            ImGui::Button("+", ImVec2(tableWidth, contextButtonSize + style.FramePadding.y * 2.0f));
            if (ImGui::BeginPopupContextItem("SelectAddedPropertyTypeContext", ImGuiPopupFlags_MouseButtonLeft)) {
                ImGui::Text("Add property");
                float propertyTypeButtonSize = 1.0f;
                for (int colType = 0; colType < SCH_LAST; colType++) {
                    float currentTextSize = gui_size_calculations::getTextButtonWidth(
                        schedule_consts::scheduleTypeNames.at((SCHEDULE_TYPE)colType));
                    propertyTypeButtonSize =
                        std::max(propertyTypeButtonSize, currentTextSize + ImGui::GetStyle().FramePadding.x * 2.0f);
                }
                for (int colType = 0; colType < SCH_LAST; colType++) {
                    if (ImGui::Button(std::format("{}##AddedPropertyTypeButton{}",
                                                  schedule_consts::scheduleTypeNames.at((SCHEDULE_TYPE)colType),
                                                  colType)
                                          .c_str(),
                                      ImVec2(propertyTypeButtonSize, 0.0f)))
                    {
                        addDefaultColumn.invoke(m_scheduleCore.getColumnCount(), (SCHEDULE_TYPE)colType);
                        if (!input.buttonStates.ctrlDown) {
                            ImGui::CloseCurrentPopup();
                        }
                    }
                }
                ImGui::EndPopup();
            }
        }

        // Clicking out of the modal closes it, unless closing a popup covering the modal
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) &&
            (ImGui::IsMouseHoveringRect(
                 ImGui::GetCurrentWindow()->OuterRectClipped.Min, ImGui::GetCurrentWindow()->OuterRectClipped.Max, false) ||
             ImGui::IsPopupOpen("", ImGuiPopupFlags_AnyPopupId)) == false)
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    } else {
        m_currentItemRow = std::nullopt;
        ImGui::PopStyleVar(2);
    }
}

void CalendarItemWindowSubGui::drawItemProperty(GuiPassReferences guiPass, ScheduleCoordinates coords) {
    ImGuiStyle& style = ImGui::GetStyle();
    size_t column = coords.column();
    size_t row = coords.row();

    bool columnEditDisabled = false;
    const Column& propertyColumn = m_scheduleCore.getColumnConst(column);
    // If viewing a different date and the column has a reset option then show it disabled
    if (m_scheduleDateOverride.getIsEmpty() == false && propertyColumn.resetOption != ColumnResetOption::Never) {
        columnEditDisabled = true;
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, gui_colors::disabledAlpha);
    }
    switch (propertyColumn.type) {
        case (SCH_BOOL): {
            bool newValue = getElementValue<bool>(coords, columnEditDisabled);
            if (element_display_templates::ElementDisplay(newValue, coords, true)) {
                setElementValueBool.invoke(column, row, newValue);
            }
            // I will make an exception for this. To avoid the infamous Double Check™ this will have an additional check for whether any item (cough cough, remove row button) is being hovered. Happy now?
            if (isEditablePropertyClicked(columnEditDisabled) && ImGui::IsAnyItemHovered() == false) {
                setElementValueBool.invoke(column, row, !newValue);
            }
            break;
        }
        case (SCH_NUMBER): {
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, style.FramePadding.y));
            int newValue = getElementValue<int>(coords, columnEditDisabled);
            if (element_display_templates::ElementDisplay(newValue, coords, true)) {
                setElementValueNumber.invoke(column, row, newValue);
            }
            ImGui::PopStyleVar();
            break;
        }
        case (SCH_DECIMAL): {
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, style.FramePadding.y));
            double newValue = getElementValue<double>(coords, columnEditDisabled);
            if (element_display_templates::ElementDisplay(newValue, coords, true)) {
                setElementValueDecimal.invoke(column, row, newValue);
            }
            ImGui::PopStyleVar();
            break;
        }
        case (SCH_TEXT): {
            std::string value = getElementValue<std::string>(coords, columnEditDisabled);
            if (element_display_templates::ElementDisplay(
                    value,
                    coords,
                    getSubGui<ElementEditorSubGui>("ElementEditorSubGui"),
                    guiPass,
                    isEditablePropertyClicked(columnEditDisabled),
                    ImGui::GetColumnWidth(coords.column())))  //ImRect(ImGui::TableGetCellBgRect(ImGui::GetCurrentTable(), 1))))
            {
                setElementValueText.invoke(coords.column(), row, value);
            }
            break;
        }
        case (SCH_SELECT): {
            SingleSelectContainer value = getElementValue<SingleSelectContainer>(coords, columnEditDisabled);

            if (element_display_templates::ElementDisplay(value,
                                                          m_scheduleCore,
                                                          coords,
                                                          getSubGui<ElementEditorSubGui>("ElementEditorSubGui"),
                                                          guiPass,
                                                          isEditablePropertyClicked(columnEditDisabled)))
            {
                setElementValueSelect.invoke(column, row, value);
            }
            break;
        }
        case (SCH_MULTISELECT): {
            SelectContainer value = getElementValue<SelectContainer>(coords, columnEditDisabled);

            if (element_display_templates::ElementDisplay(
                    value,
                    m_scheduleCore,
                    coords,
                    getSubGui<ElementEditorSubGui>("ElementEditorSubGui"),
                    guiPass,
                    ImGui::GetColumnWidth(column),
                    isEditablePropertyClicked(
                        columnEditDisabled)))  //ImRect(ImGui::TableGetCellBgRect(ImGui::GetCurrentTable(), 1))))
            {
                setElementValueMultiselect.invoke(column, row, value);
            }
            break;
        }
        case (SCH_WEEKDAY): {
            WeekdayContainer value = getElementValue<WeekdayContainer>(coords, columnEditDisabled);

            if (element_display_templates::ElementDisplay(
                    value,
                    coords,
                    getSubGui<ElementEditorSubGui>("ElementEditorSubGui"),
                    guiPass,
                    ImGui::GetColumnWidth(column),
                    isEditablePropertyClicked(
                        columnEditDisabled)))  //ImRect(ImGui::TableGetCellBgRect(ImGui::GetCurrentTable(), 1))))
            {
                setElementValueWeekday.invoke(column, row, value);
            }
            break;
        }
        case (SCH_TIME): {
            TimeContainer value = getElementValue<TimeContainer>(coords, columnEditDisabled);

            if (element_display_templates::ElementDisplay(value,
                                                          coords,
                                                          getSubGui<ElementEditorSubGui>("ElementEditorSubGui"),
                                                          guiPass,
                                                          isEditablePropertyClicked(columnEditDisabled)))
            {
                setElementValueTime.invoke(column, row, value);
            }
            break;
        }
        case (SCH_DATE): {
            DateContainer value = getElementValue<DateContainer>(coords, columnEditDisabled);

            if (element_display_templates::ElementDisplay(value,
                                                          coords,
                                                          getSubGui<ElementEditorSubGui>("ElementEditorSubGui"),
                                                          guiPass,
                                                          isEditablePropertyClicked(columnEditDisabled)))
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
}

void CalendarItemWindowSubGui::drawPropertyContext(size_t col, bool& needToBreak) {
    if (ImGui::BeginPopupContextItem(NULL, ImGuiPopupFlags_MouseButtonLeft)) {
        // We need to access the column only through this.
        // Because duplicating a column will invalidate the reference.
        auto getContextColumn = [&]() -> const Column& { return m_scheduleCore.getColumnConst(col); };

        // Renaming
        std::string name = getContextColumn().name.c_str();
        name.reserve(COLUMN_NAME_MAX_LENGTH);
        char* buf = name.data();

        if (ImGui::InputText(
                std::format("##columnName{}", col).c_str(), buf, name.capacity(), ImGuiInputTextFlags_EnterReturnsTrue))
        {
            setColumnName.invoke(col, buf);
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
            setColumnType.invoke(col, newColumnType.value());
        }
        if (getContextColumn().permanent) {
            ImGui::PopItemFlag();
        }

        ImGui::Separator();

        if (ImGui::MenuItem("Remove", NULL, false, !getContextColumn().permanent)) {
            removeColumn.invoke(col);
            needToBreak = true;
            ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
            return;  // Can't continue
        }

        if (ImGui::MenuItem("Duplicate", NULL, false, !getContextColumn().permanent)) {
            duplicateColumn.invoke(col);
            needToBreak = true;
        }

        ImGui::Separator();

        // Reset values
        if (ImGui::MenuItem("Reset default values", NULL, false)) {
            resetColumn.invoke(col, true);
        }

        // Reset setting dropdown
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Reset:");
        ImGui::SameLine();
        if (std::optional<ColumnResetOption> newColumnResetOption = gui_templates::Dropdown(
                "##ColumnResetSetting", getContextColumn().resetOption, schedule_consts::columnResetOptionStrings))
        {
            setColumnResetOption.invoke(col, newColumnResetOption.value());
        }

        ImGui::Separator();

        if (ImGui::Button("Close"))
            ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }
}

std::optional<size_t> CalendarItemWindowSubGui::getCurrentItemRow() const {
    return m_currentItemRow;
}

void CalendarItemWindowSubGui::passScheduleDateOverride(const TimeWrapper& dateOverride) {
    m_scheduleDateOverride = dateOverride;
}

void CalendarItemWindowSubGui::open(size_t itemRow) {
    m_currentItemRow = itemRow;
    ImGui::OpenPopup("CalendarItemWindowPopup");
}

void CalendarItemWindowSubGui::updateItemRow(size_t newItemRow) {
    // Only valid if an item is already being viewed
    if (m_currentItemRow.has_value()) {
        m_currentItemRow = newItemRow;
    }
}

void CalendarItemWindowSubGui::close() {
    m_currentItemRow.reset();
}