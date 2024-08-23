#pragma once

#include <string>
#include "imgui.h"
#include "imgui_internal.h"

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
    void TextWithBackground(const char* fmt, ...);
    void TextWithBackground(const ImVec2& size, const char* fmt, ...);
}