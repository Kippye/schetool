#ifndef GUI_CONSTANTS
#define GUI_CONSTANTS

#include <string>
#include "imgui.h"
#include "imgui_internal.h"

const std::string GUI_TEXTURE_DIR = "textures/gui/";

namespace gui_colors
{
    const ImVec4 dayColors[7] =
    {
        ImVec4(0.0f / 255.0f, 62.0f / 255.0f, 186.0f / 255.0f, 1),
        ImVec4(198.0f / 255.0f, 138.0f / 255.0f, 0.0f / 255.0f, 1),
        ImVec4(0.0f / 255.0f, 160.0f / 255.0f, 16.0f / 255.0f, 1),
        ImVec4(86.0f / 255.0f, 47.0f / 255.0f, 0.0f / 255.0f, 1),
        ImVec4(181.0f / 255.0f, 43.0f / 255.0f, 43.0f / 255.0f, 1),
        ImVec4(94.0f / 255.0f, 60.0f / 255.0f, 188.0f / 255.0f, 1),
        ImVec4(216.0f / 255.0f, 188.0f / 255.0f, 47.0f / 255.0f, 1),
    };
    // Use for elements that can be interacted with but are inactive in some way.
    const float inactiveAlpha = 0.5f;
    // Use for elements that are completely disabled.
    const float disabledAlpha = 0.25f;
}

namespace gui_sizes
{
    const ImVec2 emptyLabelSize = ImVec2(64, 0);
    namespace date_editor
    {
        const ImVec2 monthDayButtonSize = ImVec2(24, 24);
        const ImVec2 monthDayButtonSpacing = ImVec2(0.0f, 0.0f);
        const float monthNameComboWidth = 108.0f;
        const float yearInputWidth = 108.0f;
        const ImVec2 yearIncrementButtonSize = ImVec2(22, 22);
    };
    namespace element_editor
    {
        const ImVec2 selectOptionEditButtonSize = ImVec2(18, 18);
    };
    namespace filter_editor
    {
        const float ruleButtonWidthOffset = 30.0f;
        const float ruleComparisonComboWidth = 144.0f;
    }
}

#endif