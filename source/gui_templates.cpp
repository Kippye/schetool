#include "gui_templates.h"

bool gui_template_funcs::dateEditor(DateContainer& editorDate, unsigned int& viewedYear, unsigned int& viewedMonth)
{
    bool changedDate = false;
    ImGui::Text("%s", editorDate.getString().c_str());
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

    unsigned int daysInMonth = mytime::get_month_day_count(viewedYear, viewedMonth);

    for (unsigned int day = 1; day <= daysInMonth; day++)
    {
        if (ImGui::Button(std::to_string(day).c_str(), ImVec2(24, 24)))
        {
            editorDate.setYear(viewedYear, false);
            editorDate.setMonth(viewedMonth);
            editorDate.setMonthDay(day); 
            changedDate = true;
        }
        if (day == editorDate.getTime().tm_mday)
        {
            // TODO: Highlight this day as selected in the calendar
        }
        // sameline when not the last day of the week and not the last day of the month (trailing sameline = bad)
        if (day % 7 != 0 && day < daysInMonth)
        {
            ImGui::SameLine();
        }
    }
    return changedDate;
}