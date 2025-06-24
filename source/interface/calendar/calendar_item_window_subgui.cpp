#include "calendar/calendar_item_window_subgui.h"
#include "schedule/element_editor_subgui.h"
#include "table/schedule_gui.h"
#include "interface_style.h"
#include "schedule_column.h"
#include "imgui_stdlib.h"
#include <string>

CalendarItemWindowSubGui::CalendarItemWindowSubGui(const char* ID, const ScheduleCore& scheduleCore)
    : Gui(ID), m_scheduleCore(scheduleCore) {
    addSubGui(new ElementEditorSubGui("ElementEditorSubGui", m_scheduleCore));
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
    // TODO: I would really like this modal to have rounded corners, but it seems quite difficult to accomplish that.
    if (ImGui::BeginPopupModal("CalendarItemWindowPopup", NULL, windowFlags)) {
        ImGui::PopStyleVar();
        // If there is no row for the viewed item, the popup closes itself.
        if (m_currentItemRow.has_value() == false) {
            m_editingItemName = false;
            ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
            return;
        }
        size_t row = m_currentItemRow.value();
        std::vector<size_t> orderedColumnIndices = std::vector<size_t>(m_scheduleCore.getColumnCount());
        // Display item properties according to the schedule table's column order, if the table exists.
        if (auto scheduleTable = ScheduleGui::getScheduleTable()) {
            size_t columnIndex = 0;
            for (const auto& column : scheduleTable->Columns) {
                orderedColumnIndices.at(column.DisplayOrder) = columnIndex;
                columnIndex++;
            }
        } else  // No scheduleTable for whatever reason. Just display columns in the order they are in ScheduleCore.
        {
            for (size_t i = 0; i < orderedColumnIndices.size(); i++) {
                orderedColumnIndices[i] = i;
            }
        }
        // Draw the name bigger than other properties
        const size_t nameColumnIndex = m_scheduleCore.getFlaggedColumnIndex(ScheduleColumnFlags_Name);
        ImFont* nameFontData = InterfaceStyleHandler::getFontData((FontSize)((int)InterfaceStyleHandler::getFontSize() + 1));
        bool nameColumnEditDisabled = m_scheduleDateOverride.getIsEmpty() == false &&
            m_scheduleCore.getColumn(nameColumnIndex)->resetOption != ColumnResetOption::Never;
        std::string value = getElementValue<std::string>({nameColumnIndex, row}, nameColumnEditDisabled);
        ImGui::PushFont(nameFontData);
        float height = ImGui::CalcTextSize(value.c_str()).y + style.FramePadding.y * 2.0f;
        if (m_editingItemName == false) {
            ImGui::PushStyleColor(ImGuiCol_FrameBg, gui_colors::colorInvisible);
            ImGui::InputText("##ItemNameDummyInput", &value, ImGuiInputTextFlags_ReadOnly);
            ImGui::PopStyleColor();
            if (ImGui::IsItemClicked()) {
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
        ImGuiTableFlags propertyTableFlags = ImGuiTableFlags_NoBordersInBody;
        if (ImGui::BeginTable("PropertyColumnsTable", 2, propertyTableFlags)) {
            // Show [PROPERTY_NAME_VISIBLE_CHARS] letters of the property name before it cuts off off
            ImGui::TableSetupColumn(
                "PropertyName", ImGuiTableColumnFlags_WidthFixed, ImGui::CalcTextSize("M").x * PROPERTY_NAME_VISIBLE_CHARS);
            ImGui::TableSetupColumn("PropertyValue", ImGuiTableColumnFlags_WidthStretch);

            ImGui::TableNextRow();
            for (size_t unorderedCol = 0; unorderedCol < m_scheduleCore.getColumnCount(); unorderedCol++) {
                size_t col = orderedColumnIndices.at(unorderedCol);
                // The name has already been shown
                if (col == nameColumnIndex) {
                    continue;
                }
                ImGui::TableNextColumn();
                ImGui::AlignTextToFramePadding();
                ImGui::Text("%s", m_scheduleCore.getColumn(col)->name.c_str());
                if (ImGui::BeginItemTooltip()) {
                    ImGui::Text("%s", m_scheduleCore.getColumn(col)->name.c_str());
                    ImGui::EndTooltip();
                }
                ImGui::TableNextColumn();
                ScheduleColumnFlags columnFlags = m_scheduleCore.getColumn(col)->flags;
                drawItemProperty({windowSize, input, guiTextures}, {col, row});
            }
            ImGui::EndTable();
        }

        // Clicking out of the modal closes it
        bool elementEditorIsOpen = false;
        if (auto elementEditorSubGui = getSubGui<ElementEditorSubGui>("ElementEditorSubGui")) {
            elementEditorIsOpen = elementEditorSubGui->getOpenThisFrame();
        }
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) &&
            (ImGui::IsMouseHoveringRect(
                 ImGui::GetCurrentWindow()->OuterRectClipped.Min, ImGui::GetCurrentWindow()->OuterRectClipped.Max, false) ||
             elementEditorIsOpen) == false)
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    } else {
        m_currentItemRow = std::nullopt;
        ImGui::PopStyleVar();
    }
}

void CalendarItemWindowSubGui::drawItemProperty(GuiPassReferences guiPass, ScheduleCoordinates coords) {
    ImGuiStyle& style = ImGui::GetStyle();
    size_t column = coords.column();
    size_t row = coords.row();

    bool columnEditDisabled = false;
    const Column* propertyColumn = m_scheduleCore.getColumn(column);
    // If viewing a different date and the column has a reset option then show it disabled
    if (m_scheduleDateOverride.getIsEmpty() == false && propertyColumn->resetOption != ColumnResetOption::Never) {
        columnEditDisabled = true;
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, gui_colors::disabledAlpha);
    }
    switch (propertyColumn->type) {
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
                    ImGui::GetColumnWidth(coords.column()),
                    ImRect()))  //ImRect(ImGui::TableGetCellBgRect(ImGui::GetCurrentTable(), 1))))
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
                    isEditablePropertyClicked(columnEditDisabled),
                    ImRect()))  //ImRect(ImGui::TableGetCellBgRect(ImGui::GetCurrentTable(), 1))))
            {
                setElementValueSelect.invoke(column, row, value);
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
                    isEditablePropertyClicked(columnEditDisabled),
                    ImRect()))  //ImRect(ImGui::TableGetCellBgRect(ImGui::GetCurrentTable(), 1))))
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