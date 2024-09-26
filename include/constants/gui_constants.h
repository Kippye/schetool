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
    
    SelectColor_Last    = 1 << 9,
};

// Full definition in interface_style.cpp
enum class GuiStyle;

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
    // RGB color for black text
    const ImVec4 textColorBlack = {10.0f / 255, 10.0f / 255, 10.0f / 255, 1};
    // RGB invisible color
    const ImVec4 colorInvisible = {0.0f, 0.0f, 0.0f, 0.0f};
}

namespace gui_sizes
{
    const ImVec2 emptyLabelSize = ImVec2(64, 0);
    namespace date_editor
    {
        const ImVec2 monthDayButtonSpacing = ImVec2(0.0f, 0.0f);
        const float monthNameComboWidth = 108.0f;
        const float yearInputWidth = 108.0f;
    };
    namespace filter_editor
    {
        const float ruleButtonWidthOffset = 30.0f;
        const float ruleComparisonComboWidth = 144.0f;
    }
}

namespace gui_style_vars
{
    const float labelButtonRounding = 4.0f;
}

enum class FontSize
{
    // Font size of 8px
    Small,
    // Font size of 16px
    Normal,
    // Font size of 24px
    Big,
    // Font size of 32px
    Large
};

namespace gui_fonts
{
    const std::map<FontSize, const char*> fontSizeNames =
    {
        {FontSize::Small, "Small"},
        {FontSize::Normal, "Normal"},
        {FontSize::Big, "Big"},
        {FontSize::Large, "Large"},
    };
    const std::map<FontSize, size_t> fontSizePixelSizes =
    {
        {FontSize::Small, 8},
        {FontSize::Normal, 16},
        {FontSize::Big, 24},
        {FontSize::Large, 32},
    };
}

#endif