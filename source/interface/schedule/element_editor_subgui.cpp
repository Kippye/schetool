#include <regex>
#include <iostream>
#include "schedule/element_editor_subgui.h"
#include "gui_templates.h"
#include "schedule_constants.h"
#include "gui_constants.h"

ElementEditorSubGui::ElementEditorSubGui(const char* ID, const ScheduleCore& scheduleCore, ScheduleEvents& scheduleEvents)
    : m_scheduleCore(scheduleCore), Gui(ID) {
    scheduleEvents.editRedone.addListener(columnReorderedListener);
    scheduleEvents.editUndone.addListener(columnReorderedListener);
    scheduleEvents.editRedone.addListener(selectOptionsEditListener);
    scheduleEvents.editUndone.addListener(selectOptionsEditListener);
    scheduleEvents.selectOptionsChanged.addListener(selectOptionsChangedListener);
}

void ElementEditorSubGui::draw(GuiDrawArgs& args) {
    // give old current open state to the last frame's state
    m_openLastFrame = m_openThisFrame;
    m_madeEditsThisFrame = false;

    // Something has gone wrong!
    if (m_currentElementCoords.has_value() == false) {
        std::cout << "ElementEditorSubGui::draw(): Can't draw because current element coords is empty!" << std::endl;
        return;
    }

    if (ImGui::BeginPopupEx(ImGui::GetID("Editor"),
                            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration |
                                ImGuiWindowFlags_AlwaysAutoResize))
    {
        // TODO clean up / make the positioning more precise!
        ImGuiDir dir = ImGuiDir_Down;
        ImGuiWindow* popup = ImGui::GetCurrentWindow();
        ImRect r_outer = ImGui::GetPopupAllowedExtentRect(popup);
        ImVec2 autoFitSize = ImGui::CalcWindowNextAutoFitSize(popup);
        ImGui::SetWindowPos(ImGui::FindBestWindowPosForPopupEx(
            popup->Pos, autoFitSize, &popup->AutoPosLastDirection, r_outer, m_avoidRect, ImGuiPopupPositionPolicy_Default));
        //return FindBestWindowPosForPopupEx(window->Pos, window->Size, &window->AutoPosLastDirection, r_outer, ImRect(window->Pos, window->Pos), ImGuiPopupPositionPolicy_Default); // Ideally we'd disable r_avoid here

        switch (m_editedType) {
            case (SCH_TEXT): {
                if (gui_templates::TextEditor(m_editorText, m_textInputBoxSize, m_openLastFrame == false)) {
                    m_madeEditsThisFrame = m_madeEdits = true;
                    ImGui::CloseCurrentPopup();
                }
                break;
            }
            case (SCH_TIME): {
                if (gui_templates::TimeEditor(m_editorTime, m_editorBufferTime)) {
                    m_madeEditsThisFrame = m_madeEdits = true;
                }
                break;
            }
            case (SCH_DATE): {
                if (gui_templates::DateEditor(m_editorDate, m_viewedYear, m_viewedMonth)) {
                    m_madeEditsThisFrame = m_madeEdits = true;
                }
                break;
            }
            case (SCH_SELECT): {
                auto selection = m_editorSingleSelect.getSelection();
                const std::vector<SelectOption>& options =
                    m_scheduleCore.getColumnSelectOptions(m_currentElementCoords->column()).getOptions();

                // Selection index out of range is a no-no!
                if (selection.has_value()) {
                    if (selection.value() >= options.size()) {
                        m_editorSingleSelect.replaceSelection(std::nullopt);
                        selection.reset();
                    }
                }

                if (selection.has_value()) {
                    if (gui_templates::SelectOptionButton(options[selection.value()],
                                                          "EditorSelectedOption",
                                                          ImVec2(0, 0),
                                                          ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight))
                    {
                        m_editorSingleSelect.setSelected(selection.value(), false);
                        m_madeEditsThisFrame = m_madeEdits = true;
                    }
                    ImGui::SameLine();
                }

                // Add new options
                if (m_scheduleCore.getColumnSelectOptions(m_currentElementCoords->column()).getIsMutable()) {
                    const SelectOptions& selectOptions =
                        m_scheduleCore.getColumnSelectOptions(m_currentElementCoords->column());

                    std::string str;
                    str.reserve(schedule_consts::SELECT_OPTION_NAME_MAX_LENGTH);
                    char* buf = str.data();
                    ImGui::SetNextItemWidth(gui_size_calculations::getSelectOptionSelectableWidth());
                    // Give the option name input keyboard focus if the editor was just opened
                    if (m_openLastFrame == false) {
                        ImGui::SetKeyboardFocusHere();
                    }
                    if (ImGui::InputText("##EditorOptionInput",
                                         buf,
                                         schedule_consts::SELECT_OPTION_NAME_MAX_LENGTH,
                                         ImGuiInputTextFlags_EnterReturnsTrue))
                    {
                        if (std::string(buf).empty() == false) {
                            SelectColor addedOptionColor = SelectColor_White;
                            // If there is a previous select option, get the "next" color from its color.
                            if (selectOptions.getOptionCount() > 0) {
                                SelectColor lastOptionColor = selectOptions.getOptions().back().color;
                                // Add 1 to the last color if it's 0, otherwise multiply by 2 or loop around to 0 if needed.
                                addedOptionColor = lastOptionColor == 0
                                    ? 1
                                    : (lastOptionColor * 2 < SelectColor_Last ? lastOptionColor * 2 : 0);
                            }
                            SelectOptionsModification modificationToApply =
                                SelectOptionsModification(OPTION_MODIFICATION_ADD)
                                    .options({SelectOption(std::string(buf), addedOptionColor)});

                            modifyColumnSelectOptions.invoke(m_currentElementCoords->column(), modificationToApply);

                            // TODO: Check if the modification was actually applied
                            m_madeEditsThisFrame = m_madeEdits = true;
                            // NOTE: break here because otherwise the start and end of the function kind of go out of sync
                            break;
                        }
                    }
                }

                std::optional<size_t> hoveredOptionIndex = std::nullopt;
                std::optional<size_t> activeOptionIndex = std::nullopt;
                std::optional<std::string> activeOptionID = std::nullopt;

                // Display existing options
                for (size_t i = 0; i < options.size(); i++) {
                    bool selected = selection.has_value() && selection.value() == i;

                    std::string optionButtonID = std::string("EditorOption").append(options[i].name);

                    // Draw a name input in place of the usual selectable
                    if (m_selectEditState.editingOptionName && m_selectEditState.editedOptionIndex == i) {
                        std::string name = options[i].name;
                        name.reserve(schedule_consts::SELECT_OPTION_NAME_MAX_LENGTH);
                        char* buf = name.data();

                        if (m_selectEditState.focusOptionNameInput) {
                            ImGui::SetKeyboardFocusHere(0);
                            m_selectEditState.focusOptionNameInput = false;
                        }

                        ImGui::SetNextItemWidth(gui_size_calculations::getSelectOptionSelectableWidth());
                        if (ImGui::InputText("##SelectOptionNameInput",
                                             buf,
                                             schedule_consts::SELECT_OPTION_NAME_MAX_LENGTH,
                                             ImGuiInputTextFlags_EnterReturnsTrue,
                                             gui_callbacks::filterAlphanumerics))
                        {
                            modifyColumnSelectOptions.invoke(m_currentElementCoords->column(),
                                                             SelectOptionsModification(OPTION_MODIFICATION_RENAME)
                                                                 .firstIndex(m_selectEditState.editedOptionIndex)
                                                                 .name(buf));
                            m_selectEditState.editingOptionName = false;
                        }

                        if (ImGui::IsItemDeactivated()) {
                            m_selectEditState.editingOptionName = false;
                        }
                    }
                    // Draw the selectable for this select option
                    else {
                        bool prevSelected = selected;
                        ImGui::SetNextItemAllowOverlap();
                        if (gui_templates::SelectOptionSelectable(
                                options[i],
                                optionButtonID.c_str(),
                                &selected,
                                ImVec2(gui_size_calculations::getSelectOptionSelectableWidth(), 0)))
                        {
                            // Don't change option selection when drag is ended
                            if (m_selectEditState.draggedOptionID == optionButtonID && m_selectEditState.hasOptionBeenDragged) {
                                selected = prevSelected;
                            } else {
                                m_editorSingleSelect.setSelected(i, selected);
                                m_madeEditsThisFrame = m_madeEdits = true;
                            }
                        }
                        if (ImGui::IsItemActive()) {
                            activeOptionIndex = i;
                            activeOptionID = optionButtonID;
                        }
                        if (ImGui::IsMouseHoveringRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax())) {
                            hoveredOptionIndex = i;
                        }
                        // bool isItemHovered = ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
                        const float optionButtonHeight = ImGui::GetItemRectSize().y;
                        const float optionButtonRectMaxX = ImGui::GetItemRectMax().x;

                        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
                            // Since i can't make double clicking do nothing (without some complicated stuffery), the option just gets its selection set again on the second click.
                            selected = !selected;
                            m_editorSingleSelect.setSelected(i, selected);
                            // Start editing this select option's name
                            m_selectEditState.editingOptionName = true;
                            m_selectEditState.focusOptionNameInput = true;
                            m_selectEditState.editedOptionIndex = i;
                        }
                        // Show a remove button on the right when hovered, only if the options are mutable
                        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenOverlapped |
                                                 ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) &&
                            m_scheduleCore.getColumnSelectOptions(m_currentElementCoords->column()).getIsMutable())
                        {
                            float removeButtonSize = ImGui::CalcTextSize("X").y;
                            ImGui::SameLine();
                            ImGui::SetCursorScreenPos(
                                ImVec2(optionButtonRectMaxX - removeButtonSize - ImGui::GetStyle().FramePadding.x * 2.0f,
                                       ImGui::GetCursorScreenPos().y));
                            size_t pushedColorCount = 0;
                            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
                            pushedColorCount++;
                            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 1.0f, 1.0f, 0.2f));
                            pushedColorCount++;
                            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 1.0f, 1.0f, 0.4f));
                            pushedColorCount++;
                            if (gui_templates::ImageButtonStyleColored(std::format("##RemoveSelectOption{}", i).c_str(),
                                                                       args.guiTextures.getOrLoad("icon_remove").ImID,
                                                                       ImVec2(removeButtonSize, removeButtonSize)))
                            {
                                SelectOptionsModification modificationToApply =
                                    SelectOptionsModification(OPTION_MODIFICATION_REMOVE).firstIndex(i);
                                modifyColumnSelectOptions.invoke(m_currentElementCoords->column(), modificationToApply);
                                m_madeEditsThisFrame = m_madeEdits = true;
                                ImGui::PopStyleColor(pushedColorCount);
                                // break because the whole thing must be restarted now
                                goto break_select_case;
                            }
                            ImGui::PopStyleColor(pushedColorCount);
                        }
                    }

                    ImGui::SameLine();
                    // Edit color button
                    if (ImGui::ColorButton(
                            std::format("##EditSelectOptionColor{}", i).c_str(),
                            gui_color_calculations::hslToRgb(gui_colors::selectOptionColors.at(options[i].color)),
                            ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoDragDrop))
                    {
                        m_selectEditState.colorChooserOptionIndex = i;
                        ImGui::OpenPopup("SelectOptionColorChooserPopup");
                    }
                    ImRect colorChooserButtonAvoidRect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
                    // Color chooser popup, shows a list of available select option colors.
                    if (m_selectEditState.colorChooserOptionIndex == i &&
                        ImGui::BeginPopup("SelectOptionColorChooserPopup", ImGuiWindowFlags_NoMove))
                    {
                        ImGuiWindow* popup = ImGui::GetCurrentWindow();
                        ImRect r_outer = ImGui::GetPopupAllowedExtentRect(popup);
                        ImVec2 autoFitSize = ImGui::CalcWindowNextAutoFitSize(popup);
                        ImVec2 popupPos =
                            ImVec2(popup->Pos.x, colorChooserButtonAvoidRect.Min.y - ImGui::GetStyle().WindowPadding.y);
                        ImGui::SetWindowPos(ImGui::FindBestWindowPosForPopupEx(popupPos,
                                                                               autoFitSize,
                                                                               &popup->AutoPosLastDirection,
                                                                               r_outer,
                                                                               colorChooserButtonAvoidRect,
                                                                               ImGuiPopupPositionPolicy_Default));

                        // Draw buttons for all possible select option colors
                        for (auto [colorEnum, colorHsl] : gui_colors::selectOptionColors) {
                            if (ImGui::ColorButton(std::format("##ColorChooserPopupColor{}", colorEnum).c_str(),
                                                   gui_color_calculations::hslToRgb(colorHsl),
                                                   ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoDragDrop))
                            {
                                modifyColumnSelectOptions.invoke(
                                    m_currentElementCoords->column(),
                                    SelectOptionsModification(OPTION_MODIFICATION_RECOLOR).firstIndex(i).color(colorEnum));
                                ImGui::CloseCurrentPopup();
                            }
                        }
                        ImGui::EndPopup();
                    }
                }
                // Drag to reorder options
                if (m_selectEditState.editingOptionName == false &&
                    m_scheduleCore.getColumnSelectOptions(m_currentElementCoords->column()).getIsMutable())
                {
                    // ImGui::IsItemToggledSelection()
                    // Some option is active
                    if (activeOptionIndex.has_value()) {
                        if (activeOptionID.has_value() && activeOptionID != m_selectEditState.draggedOptionID) {
                            m_selectEditState.hasOptionBeenDragged = false;
                            m_selectEditState.draggedOptionID = activeOptionID.value();
                        }

                        // Hovering a different option (NOT whitespace!)
                        if (hoveredOptionIndex.has_value() && hoveredOptionIndex != activeOptionIndex) {
                            float dragDeltaY = ImGui::GetMouseDragDelta().y;
                            int indexDelta = 0;
                            if (dragDeltaY < 0.0f && activeOptionIndex > 0) {
                                indexDelta = -1;
                            } else if (dragDeltaY > 0.0f && activeOptionIndex < options.size() - 1) {
                                indexDelta = 1;
                            }

                            if (indexDelta != 0) {
                                m_selectEditState.hasOptionBeenDragged = true;
                                SelectOptionsModification modificationToApply =
                                    SelectOptionsModification(OPTION_MODIFICATION_MOVE)
                                        .firstIndex(activeOptionIndex.value())
                                        .secondIndex(activeOptionIndex.value() + indexDelta);
                                modifyColumnSelectOptions.invoke(m_currentElementCoords->column(), modificationToApply);
                                m_madeEditsThisFrame = m_madeEdits = true;
                                ImGui::ResetMouseDragDelta();
                            }
                        }
                    }
                    // Drag ended
                    else {
                        m_selectEditState.draggedOptionID = "";
                        m_selectEditState.hasOptionBeenDragged = false;
                    }
                }

                break;
            }
            case (SCH_MULTISELECT): {
                auto selection = m_editorSelect.getSelection();
                const std::vector<SelectOption>& options =
                    m_scheduleCore.getColumnSelectOptions(m_currentElementCoords->column()).getOptions();

                std::vector<size_t> selectionIndices = {};

                for (size_t s : selection) {
                    // Remove out-of-range selection indices
                    if (s >= options.size()) {
                        m_editorSelect.setSelected(s, false);
                        continue;
                    }
                    selectionIndices.push_back(s);
                }

                // sort indices so that the same options are always displayed in the same order
                std::sort(std::begin(selectionIndices), std::end(selectionIndices));

                for (size_t i = 0; i < selectionIndices.size(); i++) {
                    if (gui_templates::SelectOptionButton(options[selectionIndices[i]],
                                                          std::format("EditorSelectedOption{}", i).c_str(),
                                                          ImVec2(0, 0),
                                                          ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight))
                    {
                        m_editorSelect.setSelected(selectionIndices[i], false);
                        m_madeEditsThisFrame = m_madeEdits = true;
                    }
                    ImGui::SameLine();
                }

                // add new options
                if (m_scheduleCore.getColumnSelectOptions(m_currentElementCoords->column()).getIsMutable()) {
                    const SelectOptions& selectOptions =
                        m_scheduleCore.getColumnSelectOptions(m_currentElementCoords->column());

                    std::string str;
                    str.reserve(schedule_consts::SELECT_OPTION_NAME_MAX_LENGTH);
                    char* buf = str.data();
                    ImGui::SetNextItemWidth(gui_size_calculations::getSelectOptionSelectableWidth());
                    if (m_openLastFrame == false)  // Give the option name input keyboard focus if the editor was just opened
                    {
                        ImGui::SetKeyboardFocusHere();
                    }
                    if (ImGui::InputText("##EditorOptionInput",
                                         buf,
                                         schedule_consts::SELECT_OPTION_NAME_MAX_LENGTH,
                                         ImGuiInputTextFlags_EnterReturnsTrue))
                    {
                        if (std::string(buf).empty() == false) {
                            SelectColor addedOptionColor = SelectColor_White;
                            // If there is a previous select option, get the "next" color from its color.
                            if (selectOptions.getOptionCount() > 0) {
                                SelectColor lastOptionColor = selectOptions.getOptions().back().color;
                                // Add 1 to the last color if it's 0, otherwise multiply by 2 or loop around to 0 if needed.
                                addedOptionColor = lastOptionColor == 0
                                    ? 1
                                    : (lastOptionColor * 2 < SelectColor_Last ? lastOptionColor * 2 : 0);
                            }
                            SelectOptionsModification modificationToApply =
                                SelectOptionsModification(OPTION_MODIFICATION_ADD)
                                    .options({SelectOption(std::string(buf), addedOptionColor)});

                            modifyColumnSelectOptions.invoke(m_currentElementCoords->column(), modificationToApply);

                            // TODO: Check if the modification was actually applied
                            m_madeEditsThisFrame = m_madeEdits = true;
                            // NOTE: break here because otherwise the start and end of the function kind of go out of sync
                            break;
                        }
                    }
                }

                std::optional<size_t> hoveredOptionIndex = std::nullopt;
                std::optional<size_t> activeOptionIndex = std::nullopt;
                std::optional<std::string> activeOptionID = std::nullopt;

                // display existing options
                for (size_t i = 0; i < options.size(); i++) {
                    bool selected = selection.find(i) != selection.end();

                    std::string optionButtonID = std::string("EditorOption").append(options[i].name);

                    // Draw a name input in place of the usual selectable
                    if (m_selectEditState.editingOptionName && m_selectEditState.editedOptionIndex == i) {
                        std::string name = options[i].name;
                        name.reserve(schedule_consts::SELECT_OPTION_NAME_MAX_LENGTH);
                        char* buf = name.data();

                        if (m_selectEditState.focusOptionNameInput) {
                            ImGui::SetKeyboardFocusHere(0);
                            m_selectEditState.focusOptionNameInput = false;
                        }

                        ImGui::SetNextItemWidth(gui_size_calculations::getSelectOptionSelectableWidth());
                        if (ImGui::InputText("##SelectOptionNameInput",
                                             buf,
                                             schedule_consts::SELECT_OPTION_NAME_MAX_LENGTH,
                                             ImGuiInputTextFlags_EnterReturnsTrue,
                                             gui_callbacks::filterAlphanumerics))
                        {
                            modifyColumnSelectOptions.invoke(m_currentElementCoords->column(),
                                                             SelectOptionsModification(OPTION_MODIFICATION_RENAME)
                                                                 .firstIndex(m_selectEditState.editedOptionIndex)
                                                                 .name(buf));
                            m_selectEditState.editingOptionName = false;
                        }

                        if (ImGui::IsItemDeactivated()) {
                            m_selectEditState.editingOptionName = false;
                        }
                    }
                    // Draw the selectable for this select option
                    else {
                        bool prevSelected = selected;
                        ImGui::SetNextItemAllowOverlap();
                        if (gui_templates::SelectOptionSelectable(
                                options[i],
                                optionButtonID.c_str(),
                                &selected,
                                ImVec2(gui_size_calculations::getSelectOptionSelectableWidth(), 0)))
                        {
                            // Don't change option selection when drag is ended
                            if (m_selectEditState.draggedOptionID == optionButtonID && m_selectEditState.hasOptionBeenDragged) {
                                selected = prevSelected;
                            } else {
                                m_editorSelect.setSelected(i, selected);
                                m_madeEditsThisFrame = m_madeEdits = true;
                            }
                        }
                        if (ImGui::IsItemActive()) {
                            activeOptionIndex = i;
                            activeOptionID = optionButtonID;
                        }
                        if (ImGui::IsMouseHoveringRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax())) {
                            hoveredOptionIndex = i;
                        }
                        // bool isItemHovered = ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
                        const float optionButtonHeight = ImGui::GetItemRectSize().y;
                        const float optionButtonRectMaxX = ImGui::GetItemRectMax().x;

                        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
                            // Since i can't make double clicking do nothing (without some complicated stuffery), the option just gets its selection set again on the second click.
                            selected = !selected;
                            m_editorSelect.setSelected(i, selected);
                            // Start editing this select option's name
                            m_selectEditState.editingOptionName = true;
                            m_selectEditState.focusOptionNameInput = true;
                            m_selectEditState.editedOptionIndex = i;
                        }
                        // Show a remove button on the right when hovered, only if the options are mutable
                        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenOverlapped |
                                                 ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) &&
                            m_scheduleCore.getColumnSelectOptions(m_currentElementCoords->column()).getIsMutable())
                        {
                            float removeButtonSize = ImGui::CalcTextSize("X").y;
                            ImGui::SameLine();
                            ImGui::SetCursorScreenPos(
                                ImVec2(optionButtonRectMaxX - removeButtonSize - ImGui::GetStyle().FramePadding.x * 2.0f,
                                       ImGui::GetCursorScreenPos().y));
                            size_t pushedColorCount = 0;
                            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
                            pushedColorCount++;
                            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 1.0f, 1.0f, 0.2f));
                            pushedColorCount++;
                            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 1.0f, 1.0f, 0.4f));
                            pushedColorCount++;
                            if (gui_templates::ImageButtonStyleColored(std::format("##RemoveSelectOption{}", i).c_str(),
                                                                       args.guiTextures.getOrLoad("icon_remove").ImID,
                                                                       ImVec2(removeButtonSize, removeButtonSize)))
                            {
                                SelectOptionsModification modificationToApply =
                                    SelectOptionsModification(OPTION_MODIFICATION_REMOVE).firstIndex(i);
                                modifyColumnSelectOptions.invoke(m_currentElementCoords->column(), modificationToApply);
                                m_madeEditsThisFrame = m_madeEdits = true;
                                ImGui::PopStyleColor(pushedColorCount);
                                // break because the whole thing must be restarted now
                                goto break_select_case;
                            }
                            ImGui::PopStyleColor(pushedColorCount);
                        }
                    }

                    ImGui::SameLine();
                    // Edit color button
                    if (ImGui::ColorButton(
                            std::format("##EditSelectOptionColor{}", i).c_str(),
                            gui_color_calculations::hslToRgb(gui_colors::selectOptionColors.at(options[i].color)),
                            ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoDragDrop))
                    {
                        m_selectEditState.colorChooserOptionIndex = i;
                        ImGui::OpenPopup("SelectOptionColorChooserPopup");
                    }
                    ImRect colorChooserButtonAvoidRect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
                    // Color chooser popup, shows a list of available select option colors.
                    if (m_selectEditState.colorChooserOptionIndex == i && ImGui::BeginPopup("SelectOptionColorChooserPopup")) {
                        ImGuiWindow* popup = ImGui::GetCurrentWindow();
                        ImRect r_outer = ImGui::GetPopupAllowedExtentRect(popup);
                        ImVec2 autoFitSize = ImGui::CalcWindowNextAutoFitSize(popup);
                        ImVec2 popupPos =
                            ImVec2(popup->Pos.x, colorChooserButtonAvoidRect.Min.y - ImGui::GetStyle().WindowPadding.y);
                        ImGui::SetWindowPos(ImGui::FindBestWindowPosForPopupEx(popupPos,
                                                                               autoFitSize,
                                                                               &popup->AutoPosLastDirection,
                                                                               r_outer,
                                                                               colorChooserButtonAvoidRect,
                                                                               ImGuiPopupPositionPolicy_Default));

                        // Draw buttons for all possible select option colors
                        for (auto [colorEnum, colorHsl] : gui_colors::selectOptionColors) {
                            if (ImGui::ColorButton(std::format("##ColorChooserPopupColor{}", colorEnum).c_str(),
                                                   gui_color_calculations::hslToRgb(colorHsl),
                                                   ImGuiColorEditFlags_NoTooltip))
                            {
                                modifyColumnSelectOptions.invoke(
                                    m_currentElementCoords->column(),
                                    SelectOptionsModification(OPTION_MODIFICATION_RECOLOR).firstIndex(i).color(colorEnum));
                                ImGui::CloseCurrentPopup();
                            }
                        }
                        ImGui::EndPopup();
                    }
                }
                // Drag to reorder options
                if (m_selectEditState.editingOptionName == false &&
                    m_scheduleCore.getColumnSelectOptions(m_currentElementCoords->column()).getIsMutable())
                {
                    // Some option is active
                    if (activeOptionIndex.has_value()) {
                        if (activeOptionID.has_value() && activeOptionID != m_selectEditState.draggedOptionID) {
                            m_selectEditState.hasOptionBeenDragged = false;
                            m_selectEditState.draggedOptionID = activeOptionID.value();
                        }

                        // Hovering a different option (NOT whitespace!)
                        if (hoveredOptionIndex.has_value() && hoveredOptionIndex != activeOptionIndex) {
                            float dragDeltaY = ImGui::GetMouseDragDelta().y;
                            int indexDelta = 0;
                            if (dragDeltaY < 0.0f && activeOptionIndex > 0) {
                                indexDelta = -1;
                            } else if (dragDeltaY > 0.0f && activeOptionIndex < options.size() - 1) {
                                indexDelta = 1;
                            }

                            if (indexDelta != 0) {
                                m_selectEditState.hasOptionBeenDragged = true;
                                SelectOptionsModification modificationToApply =
                                    SelectOptionsModification(OPTION_MODIFICATION_MOVE)
                                        .firstIndex(activeOptionIndex.value())
                                        .secondIndex(activeOptionIndex.value() + indexDelta);
                                modifyColumnSelectOptions.invoke(m_currentElementCoords->column(), modificationToApply);
                                m_madeEditsThisFrame = m_madeEdits = true;
                                ImGui::ResetMouseDragDelta();
                            }
                        }
                    }
                    // Drag ended
                    else {
                        m_selectEditState.draggedOptionID = "";
                        m_selectEditState.hasOptionBeenDragged = false;
                    }
                }

                break;
            }
            case (SCH_WEEKDAY): {
                auto selection = m_editorWeekday.getSelection();
                size_t selectedCount = selection.size();
                const std::vector<std::string>& optionNames = general_consts::weekdayNames;

                std::vector<size_t> selectionIndices = {};

                for (size_t s : selection) {
                    selectionIndices.push_back(s);
                }

                // sort indices so that the same options are always displayed in the same order
                std::sort(std::begin(selectionIndices), std::end(selectionIndices));

                for (size_t i = 0; i < selectedCount; i++) {
                    if (gui_templates::SelectOptionButton(
                            SelectOption{optionNames[selectionIndices[i]], gui_colors::dayColors[selectionIndices[i]]},
                            std::format("EditorSelectedOption{}", i).c_str(),
                            ImVec2(0, 0),
                            ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight))
                    {
                        m_editorWeekday.setSelected(selectionIndices[i], false);
                        m_madeEditsThisFrame = m_madeEdits = true;
                    }
                    // no sameline for the last selected option
                    if (i != selectedCount - 1) {
                        ImGui::SameLine();
                    }
                }

                // display existing options
                for (size_t i = 0; i < optionNames.size(); i++) {
                    bool selected = selection.find(i) != selection.end();

                    if (gui_templates::SelectOptionSelectable(
                            SelectOption{optionNames[i], gui_colors::dayColors[i]},
                            std::format("##EditorOption{}", i).c_str(),
                            &selected,
                            ImVec2(gui_size_calculations::getSelectOptionSelectableWidth(), 0),
                            ImGuiSelectableFlags_DontClosePopups))
                    {
                        m_editorWeekday.setSelected(i, selected);
                        m_madeEditsThisFrame = m_madeEdits = true;
                    }
                }

                break;
            }
            default: {
                ImGui::EndPopup();
                return;
            }
        }
    break_select_case:

        m_openThisFrame = true;
        ImGui::EndPopup();
    } else {
        m_openThisFrame = false;
    }
}

void ElementEditorSubGui::open(size_t column, size_t row, SCHEDULE_TYPE type, const ImRect& avoidRect) {
    m_currentElementCoords = ScheduleCoordinates(column, row);
    m_editedType = type;
    m_avoidRect = avoidRect;

    m_madeEditsThisFrame = m_madeEdits = false;
    m_selectEditState.editingOptionName = false;

    ImGui::OpenPopup("Editor");
}

void ElementEditorSubGui::closeAndReset() {
    m_currentElementCoords.reset();
    m_madeEditsThisFrame = m_madeEdits = false;
    m_selectEditState.editingOptionName = false;
    m_openThisFrame = false;
}

void ElementEditorSubGui::setTextInputBoxSize(ImVec2 size) {
    m_textInputBoxSize = size;
}

bool ElementEditorSubGui::getOpenThisFrame() const {
    return m_openThisFrame;
}

bool ElementEditorSubGui::getOpenLastFrame() const {
    return m_openLastFrame;
}

bool ElementEditorSubGui::getMadeEdits() const {
    return m_madeEdits;
}

bool ElementEditorSubGui::getMadeEditsThisFrame() const {
    return m_madeEditsThisFrame;
}

std::optional<ScheduleCoordinates> ElementEditorSubGui::getCoordinates() const {
    return m_currentElementCoords;
}