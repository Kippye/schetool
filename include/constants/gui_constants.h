#ifndef GUI_CONSTANTS
#define GUI_CONSTANTS

#include <string>
#include <map>
#include "imgui.h"
#include "imgui_internal.h"

const std::string GUI_TEXTURE_DIR = "textures/gui/";

typedef int SelectColor;

enum SelectColor_
{
    SelectColor_White   = 0,
    SelectColor_Gray    = 1 << 0,
    SelectColor_Brown   = 1 << 1,
    SelectColor_Orange  = 1 << 2,
    SelectColor_Yellow  = 1 << 3,
    SelectColor_Green   = 1 << 4,
    SelectColor_Blue    = 1 << 5,
    SelectColor_Purple  = 1 << 6,
    SelectColor_Pink    = 1 << 7,
    SelectColor_Red     = 1 << 8,
};

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
    const std::map<SelectColor, ImColor> selectOptionColors = 
    {
        {SelectColor_White,     {216, 216, 216, 255}},
        {SelectColor_Gray,      {155, 155, 155, 255}},
        {SelectColor_Brown,     {135, 99, 78, 255}},
        {SelectColor_Orange,    {255, 172, 117, 255}},
        {SelectColor_Yellow,    {255, 233, 124, 255}},
        {SelectColor_Green,     {75, 130, 55, 255}},
        {SelectColor_Blue,      {99, 167, 216, 255}},
        {SelectColor_Purple,    {151, 80, 183, 255}},
        {SelectColor_Pink,      {255, 132, 228, 255}},
        {SelectColor_Red,       {229, 91, 91, 255}},
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