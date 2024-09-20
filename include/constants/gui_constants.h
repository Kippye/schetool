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
    // Select option colors in HSL (w is not really used but could be alpha).
    const std::map<SelectColor, ImVec4> selectOptionColors = 
    {
        {SelectColor_White,     {0.00f / 360, 0.00f, 0.92f, 255}},
        {SelectColor_Gray,      {0.00f / 360, 0.00f, 0.65f, 255}},
        {SelectColor_Brown,     {20.0f / 360, 0.22f, 0.47f, 255}},
        {SelectColor_Orange,    {19.0f / 360, 0.61f, 0.60f, 255}},
        {SelectColor_Yellow,    {47.0f / 360, 0.61f, 0.60f, 255}},
        {SelectColor_Green,     {123.f / 360, 0.48f, 0.64f, 255}},
        {SelectColor_Blue,      {197.f / 360, 0.50f, 0.63f, 255}},
        {SelectColor_Purple,    {270.f / 360, 0.50f, 0.63f, 255}},
        {SelectColor_Pink,      {325.f / 360, 0.43f, 0.72f, 255}},
        {SelectColor_Red,       {0.00f / 360, 0.58f, 0.65f, 255}},
    };
    // Colors of the days (equivalent select colors)
    const SelectColor dayColors[7] =
    {
        SelectColor_Blue,
        SelectColor_Orange,
        SelectColor_Green,
        SelectColor_Brown,
        SelectColor_Red,
        SelectColor_Purple,
        SelectColor_Yellow,
    };
    // Use for elements that can be interacted with but are inactive in some way.
    const float inactiveAlpha = 0.5f;
    // Use for elements that are completely disabled.
    const float disabledAlpha = 0.25f;
    const ImVec4 textColorBlack = {10.0f / 255, 10.0f / 255, 10.0f / 255, 1};
}

namespace gui_sizes
{
    const ImVec2 emptyLabelSize = ImVec2(64, 0);
    // The biggest width that a select option selectable will likely need. 
    // NOTE: Needs to be manually recalculated whenever schedule_consts::SELECT_OPTION_NAME_MAX_LENGTH is changed. Calculation: ImGui::CalcTextSize(std::string(schedule_consts::SELECT_OPTION_NAME_MAX_LENGTH, 'W').c_str()).x);
    const float selectOptionSelectableWidth = 142;
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