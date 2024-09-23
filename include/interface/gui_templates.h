#pragma once

#include <string>
#include "imgui.h"
#include "imgui_internal.h"

#include "select_options.h"
#include "time_container.h"
#include "date_container.h"

namespace gui_callbacks
{
    int filterNumbers(ImGuiInputTextCallbackData* data);
    int filterAlphanumerics(ImGuiInputTextCallbackData* data);
}

namespace gui_templates
{
    bool TextEditor(std::string& editorText, ImVec2 inputBoxSize = ImVec2(0, 0), bool captureKeyboardFocus = false);
    // Displays a date editor, applies date edits to the provided DateContainer& and uses viewedYear and viewedMonth to store the *viewed* Date. Returns true if the DateContainer was modified.
    bool DateEditor(DateContainer& editorDate, unsigned int& viewedYear, unsigned int& viewedMonth, bool displayDate = true, bool displayClearButton = true);
    // Displays a date editor, applies date edits to the provided TimeWrapper& and uses viewedYear and viewedMonth to store the *viewed* Date. Returns true if the TimeWrapper was modified.
    bool DateEditor(TimeWrapper& editorDate, unsigned int& viewedYear, unsigned int& viewedMonth, bool displayDate = true, bool displayClearButton = true);
    // Displays a time editor, applies edits to the provided TimeContainer&. Returns true if the TimeContainer was modified.
    bool TimeEditor(TimeContainer& editorTime);
    // Displays a button using a select option's name and color. The idLabel is appended to the option's name (i.e. "Select" + "##ID"). Returns true if the button was clicked.
    bool SelectOptionButton(const SelectOption& selectOption, const char* idLabel, ImVec2 size = ImVec2(0, 0), ImGuiButtonFlags flags = ImGuiButtonFlags_None);
    // Displays a selectable button using a select option's name and color. The idLabel is appended to the option's name (i.e. "Select" + "##ID"). Toggles selected when the button is clicked. Returns true if the button was clicked.
    bool SelectOptionSelectable(const SelectOption& selectOption, const char* idLabel, bool* selected, ImVec2 size = ImVec2(0, 0), ImGuiSelectableFlags flags = ImGuiButtonFlags_None);

    void TextWithBackground(const char* fmt, ...);
    void TextWithBackground(const ImVec2& size, const char* fmt, ...);
}

namespace gui_helpers
{
    // Push a style color that is automatically converted from HSL to RGB.
    void PushStyleColorHsl(ImGuiCol color, ImVec4 hslColor);
}

namespace gui_color_calculations
{
    // Color conversion functions from:
    // https://gist.github.com/ciembor/1494530

    /*
    * Converts an RGB color value to HSL. Conversion formula
    * adapted from http://en.wikipedia.org/wiki/HSL_color_space.
    * Assumes r, g, and b are contained in the range [0, 1] and
    * returns HSL in the range [0, 1].
    */
    void rgbToHsl(float r, float g, float b, float &out_h, float &out_s, float &out_v);
    ImVec4 rgbToHsl(ImVec4 rgb);
    /*
    * Converts an HSL color value to RGB. Conversion formula
    * adapted from http://en.wikipedia.org/wiki/HSL_color_space.
    * Assumes h, s, and l are contained in the range [0, 1] and
    * returns RGB in the range [0, 1].
    */
    void hslToRgb(float h, float s, float l, float &out_r, float &out_g, float &out_b);
    ImVec4 hslToRgb(ImVec4 hsl);
    ImVec4 getHoverColorFromBase(ImVec4 base);
    ImVec4 getActiveColorFromBase(ImVec4 base);
    ImVec4 getDisabledColorFromBase(ImVec4 base);
    // Calculate a likely suitable table cell highlight color (RGB) based on the text color (RGB) and window background color (RGB).
    // Essentially gets the average of those two colors.
    ImVec4 getTableCellHighlightColor(ImVec4 backgroundColor, ImVec4 fontColor);
}