#include "element_display_templates.h"

bool element_display_templates::ElementDisplay(std::string& value,
                                               ScheduleCoordinates coords,
                                               std::shared_ptr<ElementEditorSubGui> elementEditor,
                                               GuiDrawArgs& guiDrawArgs,
                                               bool openEditor,
                                               float editorWidth,
                                               ImRect avoidRect) {
    std::string prevValue = value;
    std::string displayedValue = value;

    if (elementEditor) {
        std::optional<ScheduleCoordinates> editorCoords = elementEditor->getCoordinates();
        if (elementEditor->getOpenThisFrame() && editorCoords.has_value() && editorCoords.value() == coords) {
            // if editing this text element, use this TextWrapped as a preview, the value will actually only be applied if the editor's input is applied
            displayedValue = elementEditor->getEditorValue(displayedValue);
        }
    }
    // Element to display the value as a wrapped, multiline text
    ImGui::TextWrapped("%s", displayedValue.c_str());
    // Open text editor if clicked while hovering the current column & row
    if (openEditor) {
        if (elementEditor) {
            elementEditor->setEditorValue(value);
            elementEditor->setTextInputBoxSize(ImVec2(editorWidth, 0));
            elementEditor->open(coords.column(),
                                coords.row(),
                                SCH_TEXT,
                                avoidRect.GetArea() > 0.0f
                                    ? avoidRect
                                    : (displayedValue.empty() ? ImRect(ImGui::GetMousePos(), ImGui::GetMousePos())
                                                              : ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax())));
        }
    }
    if (elementEditor) {
        std::optional<ScheduleCoordinates> editorCoords = elementEditor->getCoordinates();
        if (editorCoords.has_value() && editorCoords.value() == coords) {
            elementEditor->draw(guiDrawArgs);
            // was editing this Element, made edits and just closed the editor. apply the edits
            if (elementEditor->getOpenLastFrame() && elementEditor->getOpenThisFrame() == false &&
                elementEditor->getMadeEdits())
            {
                value = elementEditor->getEditorValue(value);
                return true;
            }
        }
    }
    return false;
}

void element_display_templates::ElementDisplay(std::string& value) {
    ImGui::TextWrapped("%s", value.c_str());
}

bool element_display_templates::ElementDisplay(SingleSelectContainer& value,
                                               const ScheduleCore& scheduleCore,
                                               ScheduleCoordinates coords,
                                               std::shared_ptr<ElementEditorSubGui> elementEditor,
                                               GuiDrawArgs& guiDrawArgs,
                                               bool openEditor) {
    auto selection = value.getSelection();
    const std::vector<SelectOption>& options = scheduleCore.getColumnConst(coords.column()).selectOptions.getOptions();

    if (selection.has_value()) {
        if (gui_templates::SelectOptionButton(options[selection.value()],
                                              std::format("##{}", coords.getString()).c_str(),
                                              ImVec2(0, 0),
                                              ImGuiButtonFlags_MouseButtonMiddle))
        {
            // Middle clicking erases the option - bonus feature
            value.setSelected(selection.value(), false);
            return true;
        }
    } else {
        addEmptyItem();
    }
    if (openEditor) {
        if (elementEditor) {
            elementEditor->open(coords.column(),
                                coords.row(),
                                SCH_SELECT,
                                selection.has_value() ? ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax())
                                                      : ImRect(ImGui::GetMousePos(), ImGui::GetMousePos()));
            elementEditor->setEditorValue(value);
        }
    }
    if (elementEditor) {
        std::optional<ScheduleCoordinates> editorCoords = elementEditor->getCoordinates();
        if (editorCoords.has_value() && editorCoords.value() == coords) {
            elementEditor->draw(guiDrawArgs);
            // Was editing this Element, made edits and just closed the editor. Apply the edits
            if (elementEditor->getOpenLastFrame() && elementEditor->getOpenThisFrame() == false &&
                elementEditor->getMadeEdits())
            {
                value = elementEditor->getEditorValue(value);
                return true;
            }
        }
    }
    return false;
}

void element_display_templates::ElementDisplay(SingleSelectContainer& value,
                                               const ScheduleCore& scheduleCore,
                                               ScheduleCoordinates coords) {
    auto selection = value.getSelection();
    const std::vector<SelectOption>& options = scheduleCore.getColumnConst(coords.column()).selectOptions.getOptions();

    if (selection.has_value()) {
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        gui_templates::SelectOptionButton(options[selection.value()], std::format("##{}", coords.getString()).c_str());
        ImGui::PopItemFlag();
    } else {
        addEmptyItem();
    }
}

bool element_display_templates::ElementDisplay(SelectContainer& value,
                                               const ScheduleCore& scheduleCore,
                                               ScheduleCoordinates coords,
                                               std::shared_ptr<ElementEditorSubGui> elementEditor,
                                               GuiDrawArgs& guiDrawArgs,
                                               float availableWidth,
                                               bool openEditor,
                                               ImRect avoidRect) {
    ImGuiStyle& style = ImGui::GetStyle();
    auto selection = value.getSelection();
    const std::vector<SelectOption>& options = scheduleCore.getColumnConst(coords.column()).selectOptions.getOptions();

    std::vector<int> selectionIndices = {};

    size_t selectedCount = selection.size();

    if (selectedCount == 0) {
        addEmptyItem();
        avoidRect = ImRect(ImGui::GetMousePos(), ImGui::GetMousePos());
    } else {
        for (size_t s : selection) {
            selectionIndices.push_back(s);
        }

        // sort indices so that the same options are always displayed in the same order
        std::sort(std::begin(selectionIndices), std::end(selectionIndices));

        size_t currentRowWidth = 0;
        const float pixelsPerCharacter = ImGui::CalcTextSize("W").x;
        ImVec2 rectMin, rectMax;

        for (size_t i = 0; i < selectedCount; i++) {
            const float nextOptionAddedWidth = (currentRowWidth == 0 ? 0.0f : style.ItemSpacing.x) +
                options[selectionIndices[i]].name.length() * pixelsPerCharacter + style.FramePadding.x * 2.0f;
            if (currentRowWidth + nextOptionAddedWidth < availableWidth) {
                if (i > 0)  // Don't add padding to the first option
                {
                    ImGui::SameLine();
                }
            } else {
                currentRowWidth = 0;
            }
            if (gui_templates::SelectOptionButton(options[selectionIndices[i]],
                                                  std::format("##{}", coords.getString()).c_str(),
                                                  ImVec2(0, 0),
                                                  ImGuiButtonFlags_MouseButtonMiddle))
            {
                // Middle clicking erases the option - bonus feature
                value.setSelected(selectionIndices[i], false);
                return true;
            }
            // Top left of the options
            if (i == 0) {
                rectMin = ImGui::GetItemRectMin();
            }
            // Bottom of the options (right is calculated above)
            if (i == selectedCount - 1) {
                rectMax.y = ImGui::GetItemRectMax().y;
            }
            currentRowWidth = currentRowWidth == 0 ? ImGui::GetItemRectSize().x
                                                   : currentRowWidth + style.ItemSpacing.x + ImGui::GetItemRectSize().x;
            // Make the rect of the option list as wide as the widest row
            rectMax.x = std::max(rectMax.x, rectMin.x + currentRowWidth);
        }
        if (avoidRect.GetArea() == 0.0f) {
            avoidRect = ImRect(rectMin, rectMax);
        }
    }
    if (openEditor) {
        if (elementEditor) {
            elementEditor->open(coords.column(), coords.row(), SCH_MULTISELECT, avoidRect);
            elementEditor->setEditorValue(value);
        }
    }
    if (elementEditor) {
        std::optional<ScheduleCoordinates> editorCoords = elementEditor->getCoordinates();
        if (editorCoords.has_value() && editorCoords.value() == coords) {
            elementEditor->draw(guiDrawArgs);
            // Was editing this Element, made edits and just closed the editor. Apply the edits
            if (elementEditor->getOpenLastFrame() && elementEditor->getOpenThisFrame() == false &&
                elementEditor->getMadeEdits())
            {
                value = elementEditor->getEditorValue(value);
                return true;
            }
        }
    }
    return false;
}

void element_display_templates::ElementDisplay(SelectContainer& value,
                                               const ScheduleCore& scheduleCore,
                                               ScheduleCoordinates coords,
                                               float availableWidth) {
    ImGuiStyle& style = ImGui::GetStyle();
    auto selection = value.getSelection();
    const std::vector<SelectOption>& options = scheduleCore.getColumnConst(coords.column()).selectOptions.getOptions();

    std::vector<int> selectionIndices = {};

    size_t selectedCount = selection.size();

    for (size_t s : selection) {
        selectionIndices.push_back(s);
    }

    // sort indices so that the same options are always displayed in the same order
    std::sort(std::begin(selectionIndices), std::end(selectionIndices));

    size_t currentRowWidth = 0;
    const float pixelsPerCharacter = ImGui::CalcTextSize("W").x;

    if (selectedCount == 0) {
        addEmptyItem();
    }
    for (size_t i = 0; i < selectedCount; i++) {
        const float nextOptionAddedWidth = (currentRowWidth == 0 ? 0.0f : style.ItemSpacing.x) +
            options[selectionIndices[i]].name.length() * pixelsPerCharacter + style.FramePadding.x * 2.0f;
        if (currentRowWidth + nextOptionAddedWidth < availableWidth) {
            if (i > 0)  // Don't add padding to the first option
            {
                ImGui::SameLine();
            }
        } else {
            currentRowWidth = 0;
        }
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        gui_templates::SelectOptionButton(options[selectionIndices[i]], std::format("##{}", coords.getString()).c_str());
        ImGui::PopItemFlag();

        currentRowWidth = currentRowWidth == 0 ? ImGui::GetItemRectSize().x
                                               : currentRowWidth + style.ItemSpacing.x + ImGui::GetItemRectSize().x;
    }
}

bool element_display_templates::ElementDisplay(WeekdayContainer& value,
                                               ScheduleCoordinates coords,
                                               std::shared_ptr<ElementEditorSubGui> elementEditor,
                                               GuiDrawArgs& guiDrawArgs,
                                               float availableWidth,
                                               bool openEditor,
                                               ImRect avoidRect) {
    ImGuiStyle& style = ImGui::GetStyle();
    auto selection = value.getSelection();
    const std::vector<std::string>& optionNames = general_consts::weekdayNames;

    std::vector<int> selectionIndices = {};

    size_t selectedCount = selection.size();

    if (selectedCount == 0) {
        addEmptyItem();
        avoidRect = ImRect(ImGui::GetMousePos(), ImGui::GetMousePos());
    } else {
        for (size_t s : selection) {
            selectionIndices.push_back(s);
        }

        // sort indices so that the same options are always displayed in the same order
        std::sort(std::begin(selectionIndices), std::end(selectionIndices));

        size_t currentRowWidth = 0;
        const float pixelsPerCharacter = ImGui::CalcTextSize("W").x;
        ImVec2 rectMin, rectMax;

        for (size_t i = 0; i < selectedCount; i++) {
            const float nextOptionAddedWidth = (currentRowWidth == 0 ? 0.0f : style.ItemSpacing.x) +
                optionNames[selectionIndices[i]].length() * pixelsPerCharacter + style.FramePadding.x * 2.0f;
            if (currentRowWidth + nextOptionAddedWidth < availableWidth) {
                if (i > 0)  // Don't add padding to the first option
                {
                    ImGui::SameLine();
                }
            } else {
                currentRowWidth = 0;
            }
            if (gui_templates::SelectOptionButton(
                    SelectOption{optionNames[selectionIndices[i]], gui_colors::dayColors[selectionIndices[i]]},
                    std::format("##{}", coords.getString()).c_str(),
                    ImVec2(),
                    ImGuiButtonFlags_MouseButtonMiddle))
            {
                // Middle clicking erases the option - bonus feature
                value.setSelected(selectionIndices[i], false);
                return true;
            }
            // Top left of the options
            if (i == 0) {
                rectMin = ImGui::GetItemRectMin();
            }
            // Bottom of the options (right is calculated above)
            if (i == selectedCount - 1) {
                rectMax.y = ImGui::GetItemRectMax().y;
            }

            currentRowWidth = currentRowWidth == 0 ? ImGui::GetItemRectSize().x
                                                   : currentRowWidth + style.ItemSpacing.x + ImGui::GetItemRectSize().x;
            // Make the rect of the option list as wide as the widest row
            rectMax.x = std::max(rectMax.x, rectMin.x + currentRowWidth);
        }
        if (avoidRect.GetArea() == 0.0f) {
            avoidRect = ImRect(rectMin, rectMax);
        }
    }
    if (openEditor) {
        if (elementEditor) {
            elementEditor->open(coords.column(), coords.row(), SCH_WEEKDAY, avoidRect);
            elementEditor->setEditorValue(value);
        }
    }
    if (elementEditor) {
        std::optional<ScheduleCoordinates> editorCoords = elementEditor->getCoordinates();
        if (editorCoords.has_value() && editorCoords.value() == coords) {
            elementEditor->draw(guiDrawArgs);
            // Was editing this Element, made edits and just closed the editor. Apply the edits
            if (elementEditor->getOpenLastFrame() && elementEditor->getOpenThisFrame() == false &&
                elementEditor->getMadeEdits())
            {
                value = elementEditor->getEditorValue(value);
                return true;
            }
        }
    }
    return false;
}

void element_display_templates::ElementDisplay(WeekdayContainer& value, ScheduleCoordinates coords, float availableWidth) {
    ImGuiStyle& style = ImGui::GetStyle();
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

    if (selectedCount == 0) {
        addEmptyItem();
    }
    for (size_t i = 0; i < selectedCount; i++) {
        const float nextOptionAddedWidth = (currentRowWidth == 0 ? 0.0f : style.ItemSpacing.x) +
            optionNames[selectionIndices[i]].length() * pixelsPerCharacter + style.FramePadding.x * 2.0f;
        if (currentRowWidth + nextOptionAddedWidth < availableWidth) {
            if (i > 0)  // Don't add padding to the first option
            {
                ImGui::SameLine();
            }
        } else {
            currentRowWidth = 0;
        }
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        gui_templates::SelectOptionButton(
            SelectOption{optionNames[selectionIndices[i]], gui_colors::dayColors[selectionIndices[i]]},
            std::format("##{}", coords.getString()).c_str());
        ImGui::PopItemFlag();

        currentRowWidth = currentRowWidth == 0 ? ImGui::GetItemRectSize().x
                                               : currentRowWidth + style.ItemSpacing.x + ImGui::GetItemRectSize().x;
    }
}

bool element_display_templates::ElementDisplay(TimeContainer& value,
                                               ScheduleCoordinates coords,
                                               std::shared_ptr<ElementEditorSubGui> elementEditor,
                                               GuiDrawArgs& guiDrawArgs,
                                               bool openEditor) {
    ImGui::Text("%s", value.getString().c_str());
    if (openEditor) {
        if (elementEditor) {
            elementEditor->setEditorValue(value);
            elementEditor->open(
                coords.column(), coords.row(), SCH_TIME, ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax()));
        }
    }
    if (elementEditor) {
        std::optional<ScheduleCoordinates> editorCoords = elementEditor->getCoordinates();
        if (editorCoords.has_value() && editorCoords.value() == coords) {
            elementEditor->draw(guiDrawArgs);
            // was editing this Element, made edits and just closed the editor. apply the edits
            if (elementEditor->getOpenThisFrame() && elementEditor->getMadeEditsThisFrame()) {
                value = elementEditor->getEditorValue(value);
                return true;
            }
        }
    }
    return false;
}

void element_display_templates::ElementDisplay(TimeContainer& value) {
    ImGui::Text("%s", value.getString().c_str());
}

bool element_display_templates::ElementDisplay(DateContainer& value,
                                               ScheduleCoordinates coords,
                                               std::shared_ptr<ElementEditorSubGui> elementEditor,
                                               GuiDrawArgs& guiDrawArgs,
                                               bool openEditor) {
    ImGui::Text("%s", value.getString().c_str());  // Display the date of the current Date element
    if (openEditor) {
        if (elementEditor) {
            elementEditor->setEditorValue(value);
            elementEditor->open(
                coords.column(), coords.row(), SCH_DATE, ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax()));
        }
    }
    if (elementEditor) {
        std::optional<ScheduleCoordinates> editorCoords = elementEditor->getCoordinates();
        if (editorCoords.has_value() && editorCoords.value() == coords) {
            elementEditor->draw(guiDrawArgs);
            // was editing this Element, made edits and just closed the editor. apply the edits
            if (elementEditor->getOpenThisFrame() && elementEditor->getMadeEditsThisFrame()) {
                value = elementEditor->getEditorValue(value);
                return true;
            }
        }
    }
    return false;
}

void element_display_templates::ElementDisplay(DateContainer& value) {
    ImGui::Text("%s", value.getString().c_str());  // Display the date of the current Date element
}