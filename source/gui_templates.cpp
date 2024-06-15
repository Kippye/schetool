#include "gui_templates.h"

bool gui_templates::DateEditor(DateContainer& editorDate, unsigned int& viewedYear, unsigned int& viewedMonth, bool displayDate)
{
    bool changedDate = false;
    if (displayDate)
    {
        TextWithBackground("%s", editorDate.getString().c_str());
    }
    // DATE / CALENDAR
    if (ImGui::ArrowButton("##PreviousMonth", ImGuiDir_Left))
    {
        viewedMonth = viewedMonth == 0 ? 11 : viewedMonth - 1;
    }
    ImGui::SameLine();
    char monthName[16];
    tm formatTime;
    formatTime.tm_mon = viewedMonth;
    std::strftime(monthName, sizeof(monthName), "%B", &formatTime);
    ImGui::Button(std::string(monthName).append(std::string("##Month")).c_str(), ImVec2(96, 0));
    ImGui::SameLine();
    if (ImGui::ArrowButton("##NextMonth", ImGuiDir_Right))
    {
        viewedMonth = viewedMonth == 11 ? 0 : viewedMonth + 1;
    }
    int yearInput = viewedYear + 1900;
    if (ImGui::InputInt("##YearInput", &yearInput, 1, 1, ImGuiInputTextFlags_EnterReturnsTrue))
    {
        viewedYear = DateContainer::convertToValidYear(yearInput);
    }

    // CALENDAR
    size_t dayIndex = 0;
    unsigned int daysInMonth = mytime::get_month_day_count(viewedYear, viewedMonth);

    tm timeIn = tm{0, 0, 0, 1, (int)viewedMonth, (int)viewedYear};
    time_t timeTemp = std::mktime(&timeIn);
    tm firstOfTheMonth;
    localtime_s(&firstOfTheMonth, &timeTemp);

    // day of the week converted from Sun-Sat to Mon-Sun
    int dayOfTheWeekFirst = firstOfTheMonth.tm_wday == 0 ? 6 : firstOfTheMonth.tm_wday - 1;

    timeIn = tm{0, 0, 0, (int)daysInMonth, (int)viewedMonth, (int)viewedYear};
    timeTemp = std::mktime(&timeIn);
    tm lastOfTheMonth;
    localtime_s(&lastOfTheMonth, &timeTemp);

    // day of the week converted from Sun-Sat to Mon-Sun
    int dayOfTheWeekLast = lastOfTheMonth.tm_wday == 0 ? 6 : lastOfTheMonth.tm_wday - 1;

    unsigned int totalDisplayedDays = (dayOfTheWeekFirst) + (daysInMonth) + (6 - dayOfTheWeekLast);

    // TODO: Handle selecting a day in the previous / next month
    // TODO: Handle selecting a day in the previous / next year
    auto addCalendarDay = [&](int month, int dayDisplayNumber)
    {
        if (ImGui::Button(std::to_string(dayDisplayNumber).c_str(), ImVec2(24, 24)) && month == viewedMonth)
        {
            editorDate.setYear(viewedYear, false);
            editorDate.setMonth(viewedMonth);
            editorDate.setMonthDay(dayDisplayNumber); 
            changedDate = true;
        }
        if (dayDisplayNumber == editorDate.getTime().tm_mday)
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
        int previousMonth = viewedMonth == 0 ? 11 : viewedMonth - 1;
        addCalendarDay(previousMonth, mytime::get_month_day_count(previousMonth < 11 ? viewedYear : viewedYear - 1, previousMonth) - (i - 1));
    }
    // days of the viewed month
    for (size_t i = 0; i < daysInMonth; i++)
    {
        addCalendarDay(viewedMonth, i + 1);
    }
    // days from next month
    for (size_t i = 0; i < 6 - dayOfTheWeekLast; i++)
    {
        int nextMonth = viewedMonth == 11 ? 0 : viewedMonth + 1;
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