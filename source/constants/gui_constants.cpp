#include "constants/gui_constants.h"

namespace gui_colors {
    const std::map<SelectColor, ImVec4> selectOptionColors = {
        {SelectColor_White, {0.00f / 360, 0.00f, 0.92f, 255}},
        {SelectColor_Gray, {0.00f / 360, 0.00f, 0.65f, 255}},
        {SelectColor_Brown, {20.0f / 360, 0.22f, 0.47f, 255}},
        {SelectColor_Orange, {19.0f / 360, 0.61f, 0.60f, 255}},
        {SelectColor_Yellow, {47.0f / 360, 0.61f, 0.60f, 255}},
        {SelectColor_Green, {123.f / 360, 0.48f, 0.64f, 255}},
        {SelectColor_Blue, {197.f / 360, 0.50f, 0.63f, 255}},
        {SelectColor_Purple, {270.f / 360, 0.50f, 0.63f, 255}},
        {SelectColor_Pink, {325.f / 360, 0.43f, 0.72f, 255}},
        {SelectColor_Red, {0.00f / 360, 0.58f, 0.65f, 255}},
    };

    const std::map<ScheduleItemState, ImVec4> scheduleItemStateColors = {

        {ScheduleItemState::Unfinished, ImVec4(0.744f, 0.100f, 0.100f, 0.0f)},
        {ScheduleItemState::Finished, ImVec4(0.176f, 0.844f, 0.000f, 0.125f)},
        {ScheduleItemState::Current, ImVec4(0.050f, 0.000f, 0.844f, 0.125f)},
        {ScheduleItemState::Normal, {1.00f, 1.00f, 1.00f, 0.0f}},
    };

    const float inactiveAlpha = 0.5f;
    const float disabledAlpha = 0.25f;
}  // namespace gui_colors

namespace gui_sizes {
    const ImVec2 emptyLabelSize = ImVec2(64, 0);
    namespace date_editor {
        const ImVec2 monthDayButtonSpacing = ImVec2(0.0f, 0.0f);
        const float monthNameComboWidth = 108.0f;
        const float yearInputWidth = 108.0f;
    };  // namespace date_editor
    namespace filter_editor {
        const float ruleButtonWidthOffset = 30.0f;
        const float ruleComparisonComboWidth = 144.0f;
    }  // namespace filter_editor
}  // namespace gui_sizes

namespace gui_style_vars {
    const ImVec2 windowEdgePadding = ImVec2(16.0f, 16.0f);
}  // namespace gui_style_vars
