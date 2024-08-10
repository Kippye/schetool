#include "imgui_stdlib.h"
#include "gui_templates.h"
#include "schedule_constants.h"

bool gui_templates::TextEditor(std::string& editorText, ImVec2 inputBoxSize, bool captureKeyboardFocus)
{
    if (captureKeyboardFocus)
    {
        ImGui::SetKeyboardFocusHere();
    }

    if (ImGui::InputTextMultiline("##editorTextInput", &editorText, inputBoxSize, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CtrlEnterForNewLine))
    {
        return true;
    }

    return false;
}

bool gui_templates::DateEditor(DateContainer& editorDate, unsigned int& viewedYear, unsigned int& viewedMonth, bool displayDate, bool displayClearButton)
{
    bool changedDate = false;
    if (displayDate)
    {
        // Display the date, with a minimum width for empty dates
        TextWithBackground(editorDate.getIsEmpty() ? gui_sizes::emptyLabelSize : ImVec2(0, 0), "%s##DateEditorDisplay%zu%zu", editorDate.getString().c_str(), viewedYear, viewedMonth);
    }
    if (displayClearButton && ImGui::Button("Clear"))
    {
        editorDate.clear();
        changedDate = true;
    }
    // DATE / CALENDAR
    if (ImGui::ArrowButton("##PreviousMonth", ImGuiDir_Left))
    {
        viewedMonth = viewedMonth == 1 ? 12 : viewedMonth - 1;
    }
    ImGui::SameLine();
    TimeWrapper formatTime;
    formatTime.setUtcMonth(viewedMonth);
    std::string monthName = formatTime.getUtcDynamicFmtString("{:%B}");
    ImGui::Button(monthName.append(std::string("##Month")).c_str(), ImVec2(96, 0));
    ImGui::SameLine();
    if (ImGui::ArrowButton("##NextMonth", ImGuiDir_Right))
    {
        viewedMonth = viewedMonth == 12 ? 1 : viewedMonth + 1;
    }
    int yearInput = viewedYear;
    if (ImGui::InputInt("##YearInput", &yearInput, 1, 1, ImGuiInputTextFlags_EnterReturnsTrue))
    {
        viewedYear = TimeWrapper::limitYearToValidRange(yearInput);
    }

    // CALENDAR
    size_t dayIndex = 0;
    unsigned int daysInMonth = mytime::get_month_day_count(viewedYear, viewedMonth);

    TimeWrapper firstOfTheMonth = TimeWrapper(viewedYear, viewedMonth, 1);
    // day of the week converted from Sun-Sat to Mon-Sun
    int dayOfTheWeekFirst = firstOfTheMonth.getUtcWeekDay(WEEK_START_MONDAY, ZERO_BASED);

    TimeWrapper lastOfTheMonth = TimeWrapper(viewedYear, viewedMonth, daysInMonth);
    // day of the week converted from Sun-Sat to Mon-Sun
    int dayOfTheWeekLast = lastOfTheMonth.getUtcWeekDay(WEEK_START_MONDAY, ZERO_BASED);

    unsigned int totalDisplayedDays = (dayOfTheWeekFirst) + (daysInMonth) + (6 - dayOfTheWeekLast);

    // TODO: Handle selecting a day in the previous / next month
    // TODO: Handle selecting a day in the previous / next year
    auto addCalendarDay = [&](int month, int dayDisplayNumber)
    {
        if (ImGui::Button(std::to_string(dayDisplayNumber).append("##").append(std::to_string(month)).c_str(), ImVec2(24, 24)) && month == viewedMonth)
        {
            editorDate.getTime().setUtcDate({viewedYear, viewedMonth, (unsigned int)dayDisplayNumber});
            changedDate = true;
        }
        if (dayDisplayNumber == editorDate.getTime().getUtcMonthDay())
        {
            // TODO: Highlight this day as selected in the calendar
        }
        // sameline when not the last day of the week and not the last day of the month (trailing sameline = bad)
        if ((dayIndex + 1) % 7 != 0 && dayIndex < totalDisplayedDays)
        {
            ImGui::SameLine();
        }
        dayIndex++;
    };

    // now take dayOfTheWeek days from the end of the previous month's daysInMonth
    // then daysInMonth days from this month
    // and take 6 - dayOfTheWeekLast days from the next month's beginning (in this case i can just count from 1)

    // days from prev month
    for (size_t i = dayOfTheWeekFirst; i > 0; i--)
    {
        int previousMonth = viewedMonth == 1 ? 12 : viewedMonth - 1;
        addCalendarDay(previousMonth, mytime::get_month_day_count(previousMonth < 12 ? viewedYear : viewedYear - 1, previousMonth) - (i - 1));
    }
    // days of the viewed month
    for (size_t i = 0; i < daysInMonth; i++)
    {
        addCalendarDay(viewedMonth, i + 1);
    }
    // days from next month
    for (size_t i = 0; i < 6 - dayOfTheWeekLast; i++)
    {
        int nextMonth = viewedMonth == 12 ? 1 : viewedMonth + 1;
        addCalendarDay(nextMonth, i + 1);
    }

    return changedDate;
}

void gui_templates::TextWithBackground(const char *fmt, ...)
{
    // format to label
    const char* text, *text_end;
    va_list args;
    va_start(args, fmt);
    ImFormatStringToTempBufferV(&text, &text_end, fmt, args);
    va_end(args);

    ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::Button(text);
    ImGui::PopItemFlag();
}

void gui_templates::TextWithBackground(const ImVec2& size, const char *fmt, ...)
{
    // format to label
    const char* text, *text_end;
    va_list args;
    va_start(args, fmt);
    ImFormatStringToTempBufferV(&text, &text_end, fmt, args);
    va_end(args);

    ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::Button(text, size);
    ImGui::PopItemFlag();
}

bool gui_templates::TimeEditor(TimeContainer& editorTime)
{
    bool madeEdits = false;
    TimeWrapper hourFormatTime = TimeWrapper(ClockTimeWrapper(editorTime.getHours(), 0));
    // NOTE: Usually we would get local time for displaying but here we are only using the TimeWrapper as a formatting tool, the same time can be stored and formatted.
    std::string hourString = hourFormatTime.getUtcDynamicFmtString("{:%H}");
    auto hourBuf = hourString.data();
    ImGui::SetNextItemWidth(24);
    if (ImGui::InputText("##TimeEditorHours", hourBuf, sizeof(hourBuf), ImGuiInputTextFlags_CallbackCharFilter | ImGuiInputTextFlags_AutoSelectAll, gui_callbacks::filterNumbers))
    {
        int hourValue = 0;

        std::string hourStr = std::string(hourBuf);

        if (hourStr.empty() == false && hourStr.find_first_not_of("0123456789") == std::string::npos)
        {
            hourValue = std::stoi(hourBuf);
        }
        editorTime.setTime(hourValue, editorTime.getMinutes());
        madeEdits = true;
    }
    ImGui::SameLine();
    TimeWrapper minFormatTime = TimeWrapper(ClockTimeWrapper(0, editorTime.getMinutes()));
    // NOTE: Read above
    std::string minString = minFormatTime.getUtcDynamicFmtString("{:%M}");
    auto minBuf = minString.data();
    ImGui::SetNextItemWidth(24);
    if (ImGui::InputText("##TimeEditorMinutes", minBuf, sizeof(minBuf), ImGuiInputTextFlags_CallbackCharFilter | ImGuiInputTextFlags_AutoSelectAll, gui_callbacks::filterNumbers))
    {
        int minValue = 0;

        std::string minStr = std::string(minBuf);

        if (minStr.empty() == false && minStr.find_first_not_of("0123456789") == std::string::npos)
        {
            minValue = std::stoi(minBuf);
        }
        editorTime.setTime(editorTime.getHours(), minValue);
        madeEdits = true;
    }

    return madeEdits;
}

int gui_callbacks::filterNumbers(ImGuiInputTextCallbackData* data)
{
	if (data->EventChar > 47 && data->EventChar < 58)
		return 0;
	return 1;
}

int gui_callbacks::filterAlphanumerics(ImGuiInputTextCallbackData* data)
{
	if (
		(data->EventChar >= 97 && data->EventChar <= 122) // lowercase letters in english alphabet
		|| (data->EventChar >= 60 && data->EventChar <= 90 && data->EventChar != 63) // capital letters in english alphabet MINUS some other things such as "?"
		|| (data->EventChar >= 48 && data->EventChar <= 57) // numbers
		|| data->EventChar == 32) // space
	{
		return 0;
	}
	return 1;
}