#pragma once

#include <memory>
#include "imgui.h"
#include "schedule_coordinates.h"
#include "schedule_core.h"
#include "gui_templates.h"
#include "schedule/element_editor_subgui.h"
#include "element.h"

struct GuiPassReferences {
        const WindowSize& windowSize;
        Input& input;
        GuiTextures& guiTextures;
};

namespace element_display_templates {
    namespace {
        template <typename T>
        bool showElementInput(T& value, ScheduleCoordinates coords) {
            static_assert(false, "No showElementInput specialisation for the provided type!");
            return false;
        }

        template <>
        bool showElementInput(bool& value, ScheduleCoordinates coords) {
            return ImGui::Checkbox(std::format("##{};{}", coords.column(), coords.row()).c_str(), &value);
        }

        template <>
        bool showElementInput(int& value, ScheduleCoordinates coords) {
            return gui_templates::InputInt(std::format("##{}", coords.getString()).c_str(), &value, false);
        }

        template <>
        bool showElementInput(double& value, ScheduleCoordinates coords) {
            return gui_templates::InputDouble(std::format("##{}", coords.getString()).c_str(), &value, "%.15g", false);
        }
    }  // namespace

    // Generic ElementDisplay for types:
    // - bool
    // - int
    // - double
    template <typename T>
    bool ElementDisplay(T& value, ScheduleCoordinates coords, bool allowEdit = false) {
        if (allowEdit) {
            return showElementInput(value, coords);
        } else {
            // HACK: We can't actually pass the correct SCHEDULE_TYPE here
            // But it currently doesn't matter if we just want to get the element as a string.

            // Also, a bool would just get displayed as "checked" or "unchecked".
            // But this isn't an issue either, because currently checkboxes always have allowEdit = true (or they are disabled externally)
            Element<T> element = Element<T>(SCH_NUMBER, value);
            ImGui::Text("%s", element.getString().c_str());
            return false;
        }
    }

    // Text
    bool ElementDisplay(std::string& value,
                        ScheduleCoordinates coords,
                        std::shared_ptr<ElementEditorSubGui> elementEditor,
                        GuiPassReferences guiPass,
                        bool openEditor,
                        float editorWidth,
                        ImRect avoidRect);
    void ElementDisplay(std::string& value);
    // Single select
    bool ElementDisplay(SingleSelectContainer& value,
                        const ScheduleCore& scheduleCore,
                        ScheduleCoordinates coords,
                        std::shared_ptr<ElementEditorSubGui> elementEditor,
                        GuiPassReferences guiPass,
                        bool openEditor);
    void ElementDisplay(SingleSelectContainer& value, const ScheduleCore& scheduleCore, ScheduleCoordinates coords);
    // Multiselect
    bool ElementDisplay(SelectContainer& value,
                        const ScheduleCore& scheduleCore,
                        ScheduleCoordinates coords,
                        std::shared_ptr<ElementEditorSubGui> elementEditor,
                        GuiPassReferences guiPass,
                        float availableWidth,
                        bool openEditor,
                        ImRect avoidRect);
    void ElementDisplay(SelectContainer& value,
                        const ScheduleCore& scheduleCore,
                        ScheduleCoordinates coords,
                        float availableWidth);
    // Weekday
    bool ElementDisplay(WeekdayContainer& value,
                        ScheduleCoordinates coords,
                        std::shared_ptr<ElementEditorSubGui> elementEditor,
                        GuiPassReferences guiPass,
                        float availableWidth,
                        bool openEditor,
                        ImRect avoidRect);
    void ElementDisplay(WeekdayContainer& value, ScheduleCoordinates coords, float availableWidth);
    // Time
    bool ElementDisplay(TimeContainer& value,
                        ScheduleCoordinates coords,
                        std::shared_ptr<ElementEditorSubGui> elementEditor,
                        GuiPassReferences guiPass,
                        bool openEditor);
    void ElementDisplay(TimeContainer& value);
    // Date
    bool ElementDisplay(DateContainer& value,
                        ScheduleCoordinates coords,
                        std::shared_ptr<ElementEditorSubGui> elementEditor,
                        GuiPassReferences guiPass,
                        bool openEditor);
    void ElementDisplay(DateContainer& value);
}  // namespace element_display_templates