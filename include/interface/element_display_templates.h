#pragma once

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
        T prevValue = value;
        bool modified = showElementInput(value, coords);
        if (allowEdit && modified) {
            return true;
        } else {
            // Editing is not allowed, revert to previous value
            if (modified) {
                value = prevValue;
            }
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
                        ImRect avoidRect,
                        bool allowEdit);
    // Single select
    bool ElementDisplay(SingleSelectContainer& value,
                        const ScheduleCore& scheduleCore,
                        ScheduleCoordinates coords,
                        std::shared_ptr<ElementEditorSubGui> elementEditor,
                        GuiPassReferences guiPass,
                        bool openEditor,
                        bool allowEdit);
    // Multiselect
    bool ElementDisplay(SelectContainer& value,
                        const ScheduleCore& scheduleCore,
                        ScheduleCoordinates coords,
                        std::shared_ptr<ElementEditorSubGui> elementEditor,
                        GuiPassReferences guiPass,
                        float availableWidth,
                        bool openEditor,
                        ImRect avoidRect,
                        bool allowEdit);
    // Weekday
    bool ElementDisplay(WeekdayContainer& value,
                        ScheduleCoordinates coords,
                        std::shared_ptr<ElementEditorSubGui> elementEditor,
                        GuiPassReferences guiPass,
                        float availableWidth,
                        bool openEditor,
                        ImRect avoidRect,
                        bool allowEdit);
    // Time
    bool ElementDisplay(TimeContainer& value,
                        ScheduleCoordinates coords,
                        std::shared_ptr<ElementEditorSubGui> elementEditor,
                        GuiPassReferences guiPass,
                        bool openEditor,
                        bool allowEdit);
    // Date
    bool ElementDisplay(DateContainer& value,
                        ScheduleCoordinates coords,
                        std::shared_ptr<ElementEditorSubGui> elementEditor,
                        GuiPassReferences guiPass,
                        bool openEditor,
                        bool allowEdit);
}  // namespace element_display_templates