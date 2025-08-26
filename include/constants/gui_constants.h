#ifndef GUI_CONSTANTS
#define GUI_CONSTANTS

#include <string>
#include <map>
#include "imgui.h"
#include "constants/schedule_constants.h"

const std::string GUI_TEXTURE_DIR = "textures/gui/";

typedef int SelectColor;

enum SelectColor_ {
    SelectColor_White = 0,
    SelectColor_Gray = 1 << 0,
    SelectColor_Brown = 1 << 1,
    SelectColor_Orange = 1 << 2,
    SelectColor_Yellow = 1 << 3,
    SelectColor_Green = 1 << 4,
    SelectColor_Blue = 1 << 5,
    SelectColor_Purple = 1 << 6,
    SelectColor_Pink = 1 << 7,
    SelectColor_Red = 1 << 8,

    SelectColor_Last = 1 << 9,
};

const std::map<std::string, ImGuiStyleVar> styleVarNameToEnum = {
    {"Alpha", ImGuiStyleVar_Alpha},
    {"DisabledAlpha", ImGuiStyleVar_DisabledAlpha},
    {"WindowPadding", ImGuiStyleVar_WindowPadding},
    {"WindowRounding", ImGuiStyleVar_WindowRounding},
    {"WindowBorderSize", ImGuiStyleVar_WindowBorderSize},
    {"WindowMinSize", ImGuiStyleVar_WindowMinSize},
    {"WindowTitleAlign", ImGuiStyleVar_WindowTitleAlign},
    {"ChildRounding", ImGuiStyleVar_ChildRounding},
    {"ChildBorderSize", ImGuiStyleVar_ChildBorderSize},
    {"PopupRounding", ImGuiStyleVar_PopupRounding},
    {"PopupBorderSize", ImGuiStyleVar_PopupBorderSize},
    {"FramePadding", ImGuiStyleVar_FramePadding},
    {"FrameRounding", ImGuiStyleVar_FrameRounding},
    {"FrameBorderSize", ImGuiStyleVar_FrameBorderSize},
    {"ItemSpacing", ImGuiStyleVar_ItemSpacing},
    {"ItemInnerSpacing", ImGuiStyleVar_ItemInnerSpacing},
    {"IndentSpacing", ImGuiStyleVar_IndentSpacing},
    {"CellPadding", ImGuiStyleVar_CellPadding},
    {"ScrollbarSize", ImGuiStyleVar_ScrollbarSize},
    {"ScrollbarRounding", ImGuiStyleVar_ScrollbarRounding},
    {"GrabMinSize", ImGuiStyleVar_GrabMinSize},
    {"GrabRounding", ImGuiStyleVar_GrabRounding},
    {"ImageBorderSize", ImGuiStyleVar_ImageBorderSize},
    {"TabRounding", ImGuiStyleVar_TabRounding},
    {"TabBorderSize", ImGuiStyleVar_TabBorderSize},
    {"TabBarBorderSize", ImGuiStyleVar_TabBarBorderSize},
    {"TabBarOverlineSize", ImGuiStyleVar_TabBarOverlineSize},
    {"TableAngledHeadersAngle", ImGuiStyleVar_TableAngledHeadersAngle},
    {"TableAngledHeadersTextAlign", ImGuiStyleVar_TableAngledHeadersTextAlign},
    {"ButtonTextAlign", ImGuiStyleVar_ButtonTextAlign},
    {"SelectableTextAlign", ImGuiStyleVar_SelectableTextAlign},
    {"SeparatorTextBorderSize", ImGuiStyleVar_SeparatorTextBorderSize},
    {"SeparatorTextAlign", ImGuiStyleVar_SeparatorTextAlign},
    {"SeparatorTextPadding", ImGuiStyleVar_SeparatorTextPadding},
};

namespace gui_colors {
    // Select option colors in HSL (w is not really used but could be alpha).
    extern const std::map<SelectColor, ImVec4> selectOptionColors;
    // Background colors for schedule items based on their state
    extern const std::map<ScheduleItemState, ImVec4> scheduleItemStateColors;
    // Colors of the days (equivalent select colors)
    const SelectColor dayColors[7] = {
        SelectColor_Blue,
        SelectColor_Orange,
        SelectColor_Green,
        SelectColor_Brown,
        SelectColor_Red,
        SelectColor_Purple,
        SelectColor_Yellow,
    };
    // Use for elements that can be interacted with but are inactive in some way.
    extern const float inactiveAlpha;
    // Use for elements that are completely disabled.
    extern const float disabledAlpha;
    // RGB color for black text
    const ImVec4 textColorBlack = {10.0f / 255, 10.0f / 255, 10.0f / 255, 1};
    // RGB invisible color
    const ImVec4 colorInvisible = {0.0f, 0.0f, 0.0f, 0.0f};
}  // namespace gui_colors

namespace gui_sizes {
    extern const ImVec2 emptyLabelSize;
    namespace date_editor {
        extern const ImVec2 monthDayButtonSpacing;
        extern const float monthNameComboWidth;
        extern const float yearInputWidth;
    };  // namespace date_editor
    namespace filter_editor {
        extern const float ruleButtonWidthOffset;
        extern const float ruleComparisonComboWidth;
    }  // namespace filter_editor
}  // namespace gui_sizes

namespace gui_style_vars {
    // Window padding to use for the padding from the actual program window
    extern const ImVec2 windowEdgePadding;
}  // namespace gui_style_vars

enum class FontSize {
    // Font size of 12px
    Small,
    // Font size of 16px
    Normal,
    // Font size of 24px
    Big,
    // Font size of 32px
    Large,
    // Font size of 40px
    Huge,
};

namespace gui_fonts {
    const std::map<FontSize, const char*> fontSizeNames = {
        {FontSize::Small, "Small"},
        {FontSize::Normal, "Normal"},
        {FontSize::Big, "Big"},
        {FontSize::Large, "Large"},
    };
    const std::map<FontSize, size_t> fontSizePixelSizes = {
        {FontSize::Small, 12},
        {FontSize::Normal, 16},
        {FontSize::Big, 24},
        {FontSize::Large, 32},
        {FontSize::Huge, 40},
    };
}  // namespace gui_fonts

#endif