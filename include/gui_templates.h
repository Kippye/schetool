#pragma once

#include "imgui.h"
#include "util.h"
#include "date_container.h"

namespace gui_template_funcs
{
    // Displays a date editor, applies date edits to the provided DateContainer& and uses viewedYear and viewedMonth to store the *viewed* Date. Returns true if the DateContainer was modified.
    bool dateEditor(DateContainer& editorDate, unsigned int& viewedYear, unsigned int& viewedMonth);
}