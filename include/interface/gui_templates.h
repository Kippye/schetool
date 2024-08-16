#pragma once

#include <string>
#include "imgui.h"
#include "imgui_internal.h"

#include "util.h"
#include "time_container.h"
#include "date_container.h"

const std::string GUI_TEXTURE_DIR = "textures/gui/";

namespace gui_templates
{
    bool TextEditor(std::string& editorText, ImVec2 inputBoxSize = ImVec2(0, 0), bool captureKeyboardFocus = false);
    // Displays a date editor, applies date edits to the provided DateContainer& and uses viewedYear and viewedMonth to store the *viewed* Date. Returns true if the DateContainer was modified.
    bool DateEditor(DateContainer& editorDate, unsigned int& viewedYear, unsigned int& viewedMonth, bool displayDate = true, bool displayClearButton = true);
    // Displays a date editor, applies date edits to the provided TimeWrapper& and uses viewedYear and viewedMonth to store the *viewed* Date. Returns true if the TimeWrapper was modified.
    bool DateEditor(TimeWrapper& editorDate, unsigned int& viewedYear, unsigned int& viewedMonth, bool displayDate = true, bool displayClearButton = true);
    // Displays a time editor, applies edits to the provided TimeContainer&. Returns true if the TimeContainer was modified.
    bool TimeEditor(TimeContainer& editorTime);
    void TextWithBackground(const char* fmt, ...);
    void TextWithBackground(const ImVec2& size, const char* fmt, ...);
}

namespace gui_callbacks
{
    int filterNumbers(ImGuiInputTextCallbackData* data);
    int filterAlphanumerics(ImGuiInputTextCallbackData* data);
}

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
}

namespace gui_sizes
{
    const ImVec2 emptyLabelSize = ImVec2(64, 0);
    const float filterRuleButtonWidthOffset = 30.0f;
}