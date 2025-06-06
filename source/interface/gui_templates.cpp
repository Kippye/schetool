#include "imgui_stdlib.h"
#include "gui_templates.h"
#include "schedule_constants.h"
#include "gui_constants.h"
#include "util.h"

bool gui_templates::TextEditor(std::string& editorText, ImVec2 inputBoxSize, bool captureKeyboardFocus) {
    if (captureKeyboardFocus) {
        ImGui::SetKeyboardFocusHere();
    }

    if (ImGui::InputTextMultiline("##editorTextInput",
                                  &editorText,
                                  inputBoxSize,
                                  ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CtrlEnterForNewLine))
    {
        return true;
    }

    return false;
}

bool gui_templates::DateEditor(
    DateContainer& editorDate, unsigned int& viewedYear, unsigned int& viewedMonth, bool displayDate, bool displayClearButton) {
    return DateEditor(editorDate.getTime(), viewedYear, viewedMonth, displayDate, displayClearButton);
}

bool gui_templates::DateEditor(
    TimeWrapper& editorDate, unsigned int& viewedYear, unsigned int& viewedMonth, bool displayDate, bool displayClearButton) {
    bool changedDate = false;
    if (displayDate) {
        // Display the date, with a minimum width for empty dates
        TextWithBackground(editorDate.getIsEmpty()
                               ? ImVec2(ImGui::CalcTextSize("01.01.1990").x + ImGui::GetStyle().FramePadding.x * 2.0f, 0)
                               : ImVec2(0, 0),
                           "%s##DateEditorDisplay%zu%zu",
                           editorDate.getStringUTC(TIME_FORMAT_DATE).c_str(),
                           viewedYear,
                           viewedMonth);
    }
    if (displayClearButton) {
        ImGui::SameLine();
        if (ImGui::Button("Clear")) {
            editorDate.clear();
            changedDate = true;
        }
    }

    const float inputLabelWidth = ImGui::CalcTextSize("September").x * 1.5f;
    // MONTH SELECTION
    if (ImGui::ArrowButton("##PreviousMonth", ImGuiDir_Left)) {
        viewedMonth = viewedMonth == 1 ? 12 : viewedMonth - 1;
    }
    ImGui::SameLine();
    TimeWrapper formatTime;
    formatTime.setMonthUTC(viewedMonth);
    std::string monthName = formatTime.getDynamicFmtStringUTC("{:%B}");
    ImGui::SetNextItemWidth(inputLabelWidth);
    if (ImGui::BeginCombo("##DateEditorMonth", monthName.c_str(), ImGuiComboFlags_NoArrowButton)) {
        for (size_t i = 1; i <= 12; i++) {
            bool isSelected = i == viewedMonth;

            formatTime.setMonthUTC(i);
            monthName = formatTime.getDynamicFmtStringUTC("{:%B}");
            if (ImGui::Selectable(monthName.c_str(), isSelected)) {
                viewedMonth = i;
            }

            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }

        ImGui::EndCombo();
    }
    ImGui::SameLine();
    if (ImGui::ArrowButton("##NextMonth", ImGuiDir_Right)) {
        viewedMonth = viewedMonth == 12 ? 1 : viewedMonth + 1;
    }
    const ImVec2 incrementButtonSize = ImGui::GetItemRectSize();

    // YEAR INPUT
    int yearInput = viewedYear;
    if (ImGui::Button("-", incrementButtonSize)) {
        viewedYear = TimeWrapper::limitYearToValidRange(viewedYear - 1);
    }
    ImGui::SameLine();
    ImGui::SetNextItemWidth(inputLabelWidth);
    if (ImGui::InputInt("##YearInput", &yearInput, 0, 0, ImGuiInputTextFlags_EnterReturnsTrue)) {
        viewedYear = TimeWrapper::limitYearToValidRange(yearInput);
    }
    ImGui::SameLine();
    if (ImGui::Button("+", incrementButtonSize)) {
        viewedYear = TimeWrapper::limitYearToValidRange(viewedYear + 1);
    }
    ImGui::SameLine(0.0f, 0.0f);
    const float dayButtonsAreaWidth = ImGui::GetCursorPosX() - ImGui::GetWindowContentRegionMin().x;
    ImGui::NewLine();

    // MONTH DAYS
    size_t dayIndex = 0;
    unsigned int daysInMonth = mytime::get_month_day_count(viewedYear, viewedMonth);

    TimeWrapper firstOfTheMonth = TimeWrapper(viewedYear, viewedMonth, 1);
    // day of the week converted from Sun-Sat to Mon-Sun
    int dayOfTheWeekFirst = firstOfTheMonth.getWeekdayUTC(WEEK_START_MONDAY, ZERO_BASED);

    TimeWrapper lastOfTheMonth = TimeWrapper(viewedYear, viewedMonth, daysInMonth);
    // day of the week converted from Sun-Sat to Mon-Sun
    int dayOfTheWeekLast = lastOfTheMonth.getWeekdayUTC(WEEK_START_MONDAY, ZERO_BASED);

    unsigned int totalDisplayedDays = (dayOfTheWeekFirst) + (daysInMonth) + (6 - dayOfTheWeekLast);
    const float monthDayButtonSize =
        /*ImGui::GetContentRegionAvail().x*/ dayButtonsAreaWidth / 7.0f - (6 * gui_sizes::date_editor::monthDayButtonSpacing.x);

    auto addCalendarDay = [&](int month, int dayDisplayNumber) {
        unsigned int pushedColorCount = 0;
        unsigned int pushedVarCount = 0;
        // Highlight the selected day in its correct month
        if (editorDate.getIsEmpty() == false && (DateWrapper(viewedYear, month, dayDisplayNumber) == editorDate.getDateUTC())) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered));
            pushedColorCount++;
        }
        // Display days from other months as slightly darker, even if selected
        if (month != viewedMonth) {
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.65f);
            pushedVarCount++;
        }
        if (ImGui::Button(std::to_string(dayDisplayNumber).append("##").append(std::to_string(month)).c_str(),
                          ImVec2(monthDayButtonSize, monthDayButtonSize)))
        {
            unsigned int selectedDayYear = viewedYear;
            if (viewedMonth == 1 && month == 12) {
                selectedDayYear--;
            }  // Viewing january and clicked a day from previous year's december
            else if (viewedMonth == 12 && month == 1)
            {
                selectedDayYear++;
            }  // Viewing december and clicked a day from next month's january
            editorDate.setDateUTC({selectedDayYear, (unsigned int)month, (unsigned int)dayDisplayNumber});
            changedDate = true;
        }
        ImGui::PopStyleColor(pushedColorCount);
        ImGui::PopStyleVar(pushedVarCount);

        // sameline when not the last day of the week and not the last day of the month (trailing sameline = bad)
        if ((dayIndex + 1) % 7 != 0 && dayIndex < totalDisplayedDays) {
            ImGui::SameLine(0, gui_sizes::date_editor::monthDayButtonSpacing.x);
        }
        dayIndex++;
    };

    // now take dayOfTheWeek days from the end of the previous month's daysInMonth
    // then daysInMonth days from this month
    // and take 6 - dayOfTheWeekLast days from the next month's beginning (in this case i can just count from 1)

    // days from prev month
    for (size_t i = dayOfTheWeekFirst; i > 0; i--) {
        int previousMonth = viewedMonth == 1 ? 12 : viewedMonth - 1;
        addCalendarDay(previousMonth,
                       mytime::get_month_day_count(previousMonth < 12 ? viewedYear : viewedYear - 1, previousMonth) - (i - 1));
    }
    // days of the viewed month
    for (size_t i = 0; i < daysInMonth; i++) {
        addCalendarDay(viewedMonth, i + 1);
    }
    // days from next month
    for (size_t i = 0; i < 6 - dayOfTheWeekLast; i++) {
        int nextMonth = viewedMonth == 12 ? 1 : viewedMonth + 1;
        addCalendarDay(nextMonth, i + 1);
    }

    return changedDate;
}

bool gui_templates::TimeEditor(TimeContainer& editorTime) {
    bool madeEdits = false;
    TimeWrapper hourFormatTime = TimeWrapper(ClockTimeWrapper(editorTime.getHours(), 0));
    // NOTE: Usually we would get local time for displaying but here we are only using the TimeWrapper as a formatting tool, the same time can be stored and formatted.
    std::string hourString = hourFormatTime.getDynamicFmtStringUTC("{:%H}");
    auto hourBuf = hourString.data();
    ImGui::SetNextItemWidth(24);
    if (ImGui::InputText("##TimeEditorHours",
                         hourBuf,
                         sizeof(hourBuf),
                         ImGuiInputTextFlags_CallbackCharFilter | ImGuiInputTextFlags_AutoSelectAll,
                         gui_callbacks::filterNumbers))
    {
        int hourValue = 0;

        std::string hourStr = std::string(hourBuf);

        if (hourStr.empty() == false && hourStr.find_first_not_of("0123456789") == std::string::npos) {
            hourValue = std::stoi(hourBuf);
        }
        editorTime.setTime(hourValue, editorTime.getMinutes());
        madeEdits = true;
    }
    ImGui::SameLine();
    TimeWrapper minFormatTime = TimeWrapper(ClockTimeWrapper(0, editorTime.getMinutes()));
    // NOTE: Read above
    std::string minString = minFormatTime.getDynamicFmtStringUTC("{:%M}");
    auto minBuf = minString.data();
    ImGui::SetNextItemWidth(24);
    if (ImGui::InputText("##TimeEditorMinutes",
                         minBuf,
                         sizeof(minBuf),
                         ImGuiInputTextFlags_CallbackCharFilter | ImGuiInputTextFlags_AutoSelectAll,
                         gui_callbacks::filterNumbers))
    {
        int minValue = 0;

        std::string minStr = std::string(minBuf);

        if (minStr.empty() == false && minStr.find_first_not_of("0123456789") == std::string::npos) {
            minValue = std::stoi(minBuf);
        }
        editorTime.setTime(editorTime.getHours(), minValue);
        madeEdits = true;
    }

    return madeEdits;
}

void gui_templates::TextWithBackground(const char* fmt, ...) {
    // format to label
    const char *text, *text_end;
    va_list args;
    va_start(args, fmt);
    ImFormatStringToTempBufferV(&text, &text_end, fmt, args);
    va_end(args);

    ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
    ImGui::Button(text);
    ImGui::PopItemFlag();
}

void gui_templates::TextWithBackground(const ImVec2& size, const char* fmt, ...) {
    // format to label
    const char *text, *text_end;
    va_list args;
    va_start(args, fmt);
    ImFormatStringToTempBufferV(&text, &text_end, fmt, args);
    va_end(args);

    ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
    ImGui::Button(text, size);
    ImGui::PopItemFlag();
}

bool gui_templates::ImageButtonStyleColored(const char* idLabel,
                                            ImTextureID textureID,
                                            ImVec2 size,
                                            ImVec2 uv0,
                                            ImVec2 uv1,
                                            ImVec4 bgColor,
                                            ImGuiButtonFlags buttonFlags) {
    return ImGui::ImageButtonEx(
        ImGui::GetID(idLabel), textureID, size, uv0, uv1, bgColor, ImGui::GetStyleColorVec4(ImGuiCol_Text), buttonFlags);
}


bool gui_templates::SelectOptionButton(const SelectOption& selectOption,
                                       const char* idLabel,
                                       ImVec2 size,
                                       ImGuiButtonFlags flags) {
    bool buttonPressed = false;
    size_t pushedColorCount = 0;
    ImVec4 baseColor = gui_colors::selectOptionColors.at(selectOption.color);

    gui_helpers::PushStyleColorHsl(ImGuiCol_Button, baseColor);
    pushedColorCount++;
    gui_helpers::PushStyleColorHsl(ImGuiCol_ButtonHovered, gui_color_calculations::getHoverColorFromBase(baseColor));
    pushedColorCount++;
    gui_helpers::PushStyleColorHsl(ImGuiCol_ButtonActive, gui_color_calculations::getActiveColorFromBase(baseColor));
    pushedColorCount++;
    ImGui::PushStyleColor(ImGuiCol_Text, gui_colors::textColorBlack);
    pushedColorCount++;
    size_t pushedStyleVarCount = 0;
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, gui_style_vars::labelButtonRounding);
    pushedStyleVarCount++;
    if (ImGui::ButtonEx(std::string(selectOption.name).append(idLabel).c_str(), ImVec2(0, 0), flags)) {
        buttonPressed = true;
    }
    ImGui::PopStyleColor(pushedColorCount);
    ImGui::PopStyleVar(pushedStyleVarCount);

    return buttonPressed;
}

bool gui_templates::SelectOptionSelectable(
    const SelectOption& selectOption, const char* idLabel, bool* selected, ImVec2 size, ImGuiButtonFlags flags) {
    bool selectablePressed = false;
    size_t pushedColorCount = 0;
    ImVec4 baseColor = gui_colors::selectOptionColors.at(selectOption.color);

    // Use base color if selected or disabled color if not selected
    gui_helpers::PushStyleColorHsl(ImGuiCol_Button,
                                   *selected ? baseColor : gui_color_calculations::getDisabledColorFromBase(baseColor));
    pushedColorCount++;
    gui_helpers::PushStyleColorHsl(ImGuiCol_ButtonHovered, gui_color_calculations::getHoverColorFromBase(baseColor));
    pushedColorCount++;
    gui_helpers::PushStyleColorHsl(ImGuiCol_ButtonActive, gui_color_calculations::getActiveColorFromBase(baseColor));
    pushedColorCount++;
    ImGui::PushStyleColor(ImGuiCol_Text, gui_colors::textColorBlack);
    pushedColorCount++;
    size_t pushedStyleVarCount = 0;
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, gui_style_vars::labelButtonRounding);
    pushedStyleVarCount++;
    if (ImGui::ButtonEx(std::string(selectOption.name).append(idLabel).c_str(), size, flags)) {
        *selected = !*selected;
        selectablePressed = true;
    }
    ImGui::PopStyleColor(pushedColorCount);
    ImGui::PopStyleVar(pushedStyleVarCount);

    return selectablePressed;
}

int gui_callbacks::filterNumbers(ImGuiInputTextCallbackData* data) {
    if (data->EventChar > 47 && data->EventChar < 58)
        return 0;
    return 1;
}

int gui_callbacks::filterAlphanumerics(ImGuiInputTextCallbackData* data) {
    if ((data->EventChar >= 97 && data->EventChar <= 122)  // lowercase letters in english alphabet
        || (data->EventChar >= 60 && data->EventChar <= 90 &&
            data->EventChar != 63)  // capital letters in english alphabet MINUS some other things such as "?"
        || (data->EventChar >= 48 && data->EventChar <= 57)  // numbers
        || data->EventChar == 32)  // space
    {
        return 0;
    }
    return 1;
}

void gui_helpers::PushStyleColorHsl(ImGuiCol color, ImVec4 hslColor) {
    ImGui::PushStyleColor(color, gui_color_calculations::hslToRgb(hslColor));
}

float gui_size_calculations::getTextButtonWidth(const char* label) {
    return ImGui::CalcTextSize(label, NULL, true).x;
}

float gui_size_calculations::getSelectOptionSelectableWidth() {
    float labelWidth = getTextButtonWidth(std::string(schedule_consts::SELECT_OPTION_NAME_MAX_LENGTH, 'W').c_str());
    return labelWidth + ImGui::GetStyle().FramePadding.x * 2.0f;
}

void gui_color_calculations::rgbToHsl(float r, float g, float b, float& out_h, float& out_s, float& out_l) {
    float max = std::max(std::max(r, g), b);
    float min = std::min(std::min(r, g), b);

    out_h = out_s = out_l = (max + min) / 2;

    if (max == min) {
        out_h = out_s = 0;  // achromatic
    } else {
        float d = max - min;
        out_s = (out_l > 0.5) ? d / (2 - max - min) : d / (max + min);

        if (max == r) {
            out_h = (g - b) / d + (g < b ? 6 : 0);
        } else if (max == g) {
            out_h = (b - r) / d + 2;
        } else if (max == b) {
            out_h = (r - g) / d + 4;
        }

        out_h /= 6;
    }
}

ImVec4 gui_color_calculations::rgbToHsl(ImVec4 rgb) {
    float h, s, l;
    rgbToHsl(rgb.x, rgb.y, rgb.z, h, s, l);
    return ImVec4(h, s, l, rgb.w);
}

void gui_color_calculations::hslToRgb(float h, float s, float l, float& out_r, float& out_g, float& out_b) {
    /*
    * Converts a HUE to r, g or b.
    * returns float in the range [0, 1].
    */
    auto hue2rgb = [](float p, float q, float t) {
        if (t < 0)
            t += 1;
        if (t > 1)
            t -= 1;
        if (t < 1. / 6)
            return p + (q - p) * 6 * t;
        if (t < 1. / 2)
            return q;
        if (t < 2. / 3)
            return p + (q - p) * (2.f / 3 - t) * 6;
        return p;
    };

    if (0 == s) {
        out_r = out_g = out_b = l;  // achromatic
    } else {
        float q = l < 0.5 ? l * (1 + s) : l + s - l * s;
        float p = 2 * l - q;
        out_r = hue2rgb(p, q, h + 1. / 3);
        out_g = hue2rgb(p, q, h);
        out_b = hue2rgb(p, q, h - 1. / 3);
    }
}

ImVec4 gui_color_calculations::hslToRgb(ImVec4 hsl) {
    float r, g, b;
    hslToRgb(hsl.x, hsl.y, hsl.z, r, g, b);
    return ImVec4(r, g, b, hsl.w);
}

ImVec4 gui_color_calculations::getHoverColorFromBase(ImVec4 base) {
    int light = base.z * 100;

    if (light <= 30)  // Make dark color brighter
    {
        light += 5;
    } else  // Darken bright colors
    {
        light -= 5;
    }
    return ImVec4(base.x, base.y, light / 100.0f, base.w);
}
ImVec4 gui_color_calculations::getActiveColorFromBase(ImVec4 base) {
    int light = base.z * 100;

    if (light <= 30)  // Make dark colors a lot brighter
    {
        light += 20;
    } else  // Darken bright colors a lot
    {
        light -= 20;
    }
    return ImVec4(base.x, base.y, light / 100.0f, base.w);
}
ImVec4 gui_color_calculations::getDisabledColorFromBase(ImVec4 base) {
    int light = base.z * 100;

    int light_sub = 40;  // For colors whose L is in the range 60-100, subtract 40 from it

    if (light < 60)  // For colors whose L is below 60, gradually reduce the subtracted amount down to a minimum of 10
    {
        light_sub = std::max(40 - ((60 - light_sub) / 20) * 10, 10);
    }

    light = std::max(light - light_sub, 0);
    return ImVec4(base.x, base.y, light / 100.0f, base.w);
}

ImVec4 gui_color_calculations::getTableCellHighlightColor(ImVec4 backgroundColor, ImVec4 fontColor) {
    ImVec4 sum = ImVec4(
        backgroundColor.x + fontColor.x, backgroundColor.y + fontColor.y, backgroundColor.z + fontColor.z, backgroundColor.w);
    return ImVec4(sum.x / 2.0f, sum.y / 2.0f, sum.z / 2.0f, sum.w);
}