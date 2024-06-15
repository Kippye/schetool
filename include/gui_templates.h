#pragma once

#include "imgui.h"
#include "imgui_internal.h"

#include "util.h"
#include "time_container.h"
#include "date_container.h"

namespace gui_templates
{
    // Displays a date editor, applies date edits to the provided DateContainer& and uses viewedYear and viewedMonth to store the *viewed* Date. Returns true if the DateContainer was modified.
    bool DateEditor(DateContainer& editorDate, unsigned int& viewedYear, unsigned int& viewedMonth, bool displayDate = true);
    // Displays a time editor, applies edits to the provided TimeContainer&. Returns true if the TimeContainer was modified.
    bool TimeEditor(TimeContainer& editorTime);
    void TextWithBackground(const char* fmt, ...);
    void TextWithBackground(const ImVec2& size, const char* fmt, ...);
}

namespace gui_callbacks
{
    int filterNumbers(ImGuiInputTextCallbackData* data);
}