#pragma once

#include <utility>
#include <memory>
#include "imgui.h"
#include "schedule_coordinates.h"
#include "schedule_core.h"
#include "gui_templates.h"
#include "schedule/element_editor_subgui.h"

struct GuiPassReferences {
        const WindowSize& windowSize;
        Input& input;
        GuiTextures& guiTextures;
};

template <typename T>
inline bool showElementInput(T& value, ScheduleCoordinates coords) {
    static_assert(false, "No showElementInput specialisation for the provided type!");
}

template <>
inline bool showElementInput(bool& value, ScheduleCoordinates coords) {
    return ImGui::Checkbox(std::format("##{};{}", coords.column(), coords.row()).c_str(), &value);
}

template <>
inline bool showElementInput(int& value, ScheduleCoordinates coords) {
    return gui_templates::InputInt(std::format("##{}", coords.getString()).c_str(), &value, false);
}

template <>
inline bool showElementInput(double& value, ScheduleCoordinates coords) {
    return gui_templates::InputDouble(std::format("##{}", coords.getString()).c_str(), &value, "%.15g", false);
}

// Generic ElementDisplay for types:
// - bool
// - int
// - double
template <typename T>
std::pair<bool, T> ElementDisplay(T& value,
                                  const ScheduleCore&,
                                  std::shared_ptr<ElementEditorSubGui>,
                                  ScheduleCoordinates coords,
                                  float availableWidth,
                                  bool allowEdit = false) {
    T prevValue = value;
    bool modified = showElementInput(value, coords);
    if (allowEdit && modified) {
        return {true, value};
    } else {
        // Editing is not allowed, revert to previous value
        if (modified) {
            value = prevValue;
        }
        return {false, value};
    }
}

inline std::pair<bool, std::string> ElementDisplay(std::string& value,
                                                   ScheduleCoordinates coords,
                                                   std::shared_ptr<ElementEditorSubGui> elementEditor,
                                                   GuiPassReferences guiPass,
                                                   bool openEditor,
                                                   float editorWidth,
                                                   ImRect avoidRect,
                                                   bool allowEdit) {
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
            elementEditor->open(coords.column(), coords.row(), SCH_TEXT, avoidRect);
        }
    }
    if (elementEditor) {
        std::optional<ScheduleCoordinates> editorCoords = elementEditor->getCoordinates();
        if (editorCoords.has_value() && editorCoords.value() == coords) {
            elementEditor->draw(guiPass.windowSize, guiPass.input, guiPass.guiTextures);
            // was editing this Element, made edits and just closed the editor. apply the edits
            if (elementEditor->getOpenLastFrame() && elementEditor->getOpenThisFrame() == false &&
                elementEditor->getMadeEdits())
            {
                return {true, elementEditor->getEditorValue(value)};
            }
        }
    }
    return {false, value};
}

inline std::pair<bool, SingleSelectContainer> ElementDisplay(SingleSelectContainer& value,
                                                             const ScheduleCore& scheduleCore,
                                                             ScheduleCoordinates coords,
                                                             std::shared_ptr<ElementEditorSubGui> elementEditor,
                                                             GuiPassReferences guiPass,
                                                             bool openEditor,
                                                             float editorWidth,
                                                             ImRect avoidRect,
                                                             bool allowEdit) {
    auto selection = value.getSelection();
    const std::vector<SelectOption>& options = scheduleCore.getColumn(coords.column())->selectOptions.getOptions();

    if (selection.has_value()) {
        if (gui_templates::SelectOptionButton(options[selection.value()],
                                              std::format("##{}", coords.getString()).c_str(),
                                              ImVec2(0, 0),
                                              ImGuiButtonFlags_MouseButtonMiddle))
        {
            // Middle clicking erases the option - bonus feature
            if (allowEdit) {
                value.setSelected(selection.value(), false);
                return {true, value};
            }
        }
    }
    if (openEditor) {
        if (elementEditor) {
            elementEditor->open(
                coords.column(), coords.row(), SCH_SELECT, ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax()));
            elementEditor->setEditorValue(value);
        }
    }
    if (elementEditor) {
        std::optional<ScheduleCoordinates> editorCoords = elementEditor->getCoordinates();
        if (editorCoords.has_value() && editorCoords.value() == coords) {
            elementEditor->draw(guiPass.windowSize, guiPass.input, guiPass.guiTextures);
            // Was editing this Element, made edits and just closed the editor. Apply the edits
            if (elementEditor->getOpenLastFrame() && elementEditor->getOpenThisFrame() == false &&
                elementEditor->getMadeEdits())
            {
                return {true, elementEditor->getEditorValue(value)};
            }
        }
    }
    return {false, value};
}

inline std::pair<bool, SelectContainer> ElementDisplay(SelectContainer& value,
                                                       const ScheduleCore& scheduleCore,
                                                       ScheduleCoordinates coords,
                                                       std::shared_ptr<ElementEditorSubGui> elementEditor,
                                                       GuiPassReferences guiPass,
                                                       float availableWidth,
                                                       bool openEditor,
                                                       float editorWidth,
                                                       ImRect avoidRect,
                                                       bool allowEdit) {
    ImGuiStyle& style = ImGui::GetStyle();
    auto selection = value.getSelection();
    const std::vector<SelectOption>& options = scheduleCore.getColumn(coords.column())->selectOptions.getOptions();

    std::vector<int> selectionIndices = {};

    size_t selectedCount = selection.size();

    for (size_t s : selection) {
        selectionIndices.push_back(s);
    }

    // sort indices so that the same options are always displayed in the same order
    std::sort(std::begin(selectionIndices), std::end(selectionIndices));

    size_t currentRowWidth = 0;
    const float pixelsPerCharacter = ImGui::CalcTextSize("W").x;

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
            if (allowEdit) {
                value.setSelected(selectionIndices[i], false);
                return {true, value};
            }
        }

        currentRowWidth = currentRowWidth == 0 ? ImGui::GetItemRectSize().x
                                               : currentRowWidth + style.ItemSpacing.x + ImGui::GetItemRectSize().x;
    }
    if (openEditor) {
        if (elementEditor) {
            elementEditor->open(
                coords.column(), coords.row(), SCH_SELECT, ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax()));
            elementEditor->setEditorValue(value);
        }
    }
    if (elementEditor) {
        std::optional<ScheduleCoordinates> editorCoords = elementEditor->getCoordinates();
        if (editorCoords.has_value() && editorCoords.value() == coords) {
            elementEditor->draw(guiPass.windowSize, guiPass.input, guiPass.guiTextures);
            // Was editing this Element, made edits and just closed the editor. Apply the edits
            if (elementEditor->getOpenLastFrame() && elementEditor->getOpenThisFrame() == false &&
                elementEditor->getMadeEdits())
            {
                return {true, elementEditor->getEditorValue(value)};
            }
        }
    }
    return {false, value};
}

inline std::pair<bool, WeekdayContainer> ElementDisplay(WeekdayContainer& value,
                                                        const ScheduleCore& scheduleCore,
                                                        ScheduleCoordinates coords,
                                                        std::shared_ptr<ElementEditorSubGui> elementEditor,
                                                        GuiPassReferences guiPass,
                                                        float availableWidth,
                                                        bool openEditor,
                                                        float editorWidth,
                                                        ImRect avoidRect,
                                                        bool allowEdit) {
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
            if (allowEdit) {
                value.setSelected(selectionIndices[i], false);
                return {true, value};
            }
        }

        currentRowWidth = currentRowWidth == 0 ? ImGui::GetItemRectSize().x
                                               : currentRowWidth + style.ItemSpacing.x + ImGui::GetItemRectSize().x;
    }
    if (openEditor) {
        if (elementEditor) {
            elementEditor->open(
                coords.column(), coords.row(), SCH_WEEKDAY, ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax()));
            elementEditor->setEditorValue(value);
        }
    }
    if (elementEditor) {
        std::optional<ScheduleCoordinates> editorCoords = elementEditor->getCoordinates();
        if (editorCoords.has_value() && editorCoords.value() == coords) {
            elementEditor->draw(guiPass.windowSize, guiPass.input, guiPass.guiTextures);
            // Was editing this Element, made edits and just closed the editor. Apply the edits
            if (elementEditor->getOpenLastFrame() && elementEditor->getOpenThisFrame() == false &&
                elementEditor->getMadeEdits())
            {
                return {true, elementEditor->getEditorValue(value)};
            }
        }
    }
    return {false, value};
}

inline std::pair<bool, TimeContainer> ElementDisplay(TimeContainer& value,
                                                     const ScheduleCore& scheduleCore,
                                                     ScheduleCoordinates coords,
                                                     std::shared_ptr<ElementEditorSubGui> elementEditor,
                                                     GuiPassReferences guiPass,
                                                     float availableWidth,
                                                     bool openEditor,
                                                     float editorWidth,
                                                     ImRect avoidRect,
                                                     bool allowEdit) {
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
            elementEditor->draw(guiPass.windowSize, guiPass.input, guiPass.guiTextures);
            // was editing this Element, made edits and just closed the editor. apply the edits
            if (elementEditor->getOpenLastFrame() && elementEditor->getOpenThisFrame() == false &&
                elementEditor->getMadeEdits())
            {
                return {true, elementEditor->getEditorValue(value)};
            }
        }
    }
    return {false, value};
}

inline std::pair<bool, DateContainer> ElementDisplay(DateContainer& value,
                                                     const ScheduleCore& scheduleCore,
                                                     ScheduleCoordinates coords,
                                                     std::shared_ptr<ElementEditorSubGui> elementEditor,
                                                     GuiPassReferences guiPass,
                                                     float availableWidth,
                                                     bool openEditor,
                                                     float editorWidth,
                                                     ImRect avoidRect,
                                                     bool allowEdit) {
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
            elementEditor->draw(guiPass.windowSize, guiPass.input, guiPass.guiTextures);
            // was editing this Element, made edits and just closed the editor. apply the edits
            if (elementEditor->getOpenLastFrame() && elementEditor->getOpenThisFrame() == false &&
                elementEditor->getMadeEdits())
            {
                return {true, elementEditor->getEditorValue(value)};
            }
        }
    }
    return {false, value};
}