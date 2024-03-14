#include <algorithm>
#include <cctype>
#include <ctime>
#include <iterator>
#include <string.h>
#include <time_container.h>
#include <iostream>

Time::Time()
{

}

Time::Time(const tm& time)
{
    m_stored_time = tm(time);
}

const tm* Time::getTime()
{
    return &m_stored_time;
}
// Returns the string representation of the stored time, depending on the displayDate, displayTime and displayWeekday values.
std::string Time::getString(bool displayDate, bool displayTime, bool displayWeekday)
{
    try
    {
        char output[1024];

        if (displayDate == false && displayTime == false && displayWeekday == false)
        {
            displayDate = true;
        }

        if (displayWeekday)
        {
            unsigned int currentIndex = 0;
            tm weekdayDisplay;

            // display all selected weekdays
            for (int i = 0; i < sizeof(m_selectedWeekdays); i++)
            {
                if (m_selectedWeekdays[i] == true)
                {
                    weekdayDisplay.tm_wday = i == 0 ? 1 : (i == 6 ? 0 : i + 1);
                    char currentOutput[32];
                    std::strftime(currentOutput, sizeof(currentOutput), "%A ", &weekdayDisplay);
                    // copy to total output
                    for (int j = 0; j < sizeof(currentOutput); j++)
                    {
                        if (std::isalpha((unsigned char)currentOutput[j]) != 0 || std::isblank((unsigned char)currentOutput[j]))
                        {
                            output[currentIndex] = currentOutput[j];
                            currentIndex++;
                        }
                        else
                        {
                            break;
                        }
                    }
                }
            }
        }
        else
        {
            if (displayDate && displayTime)
            {
                std::strftime(output, sizeof(output), "%d/%m/%y %H:%M", &m_stored_time);
            }
            else
            {
                if (displayDate)
                {
                    std::strftime(output, sizeof(output), "%d/%m/%y", &m_stored_time);
                }
                else
                {
                    std::strftime(output, sizeof(output), "%H:%M", &m_stored_time);
                }
            }
        }

        return std::string(output);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << "\n";
        std::cout << "std::string Time::getString(): Failed to convert current time to a string. Invalid time?" << std::endl;
        return std::string("");
    }
}  

const bool* Time::getSelectedWeekdays()
{
    return m_selectedWeekdays;
}

void Time::setTime(const tm& time)
{
    m_stored_time = tm(time);
}
// NOTE: Values are automatically clamped below 23 for hours and below 59 for minutes
void Time::setClockTime(unsigned int hour, unsigned int minute)
{
    m_stored_time.tm_hour = std::min(hour, 23U);
    m_stored_time.tm_min = std::min(minute, 59U);
}
void Time::setMonthDay(unsigned int day)
{
    m_stored_time.tm_mday = day;
}
// NOTE: If the given month is < 0, it will be set to 11. If it's > 11, it will be set to 0
void Time::setMonth(int month)
{
    m_stored_time.tm_mon = month < 0 ? 11 : (month > 11 ? 0 : month);
}
// NOTE: hasBeenSubtracted is used to determine which range the year should be limited to (if 1900 has already been subtracted from the year)
// subtractTmBaseYear - if this is true, then 1900 will be subtracted from the year before doing other validation
void Time::setYear(int year, bool convert)
{
    year = convertToValidYear(year, !convert, convert);
    m_stored_time.tm_year = year;
}

// Set the weekday at index weekday (0-6) selection to selected
void Time::setWeekdaySelected(unsigned int weekday, bool selected)
{
    weekday = std::min(weekday, 6U);
    m_selectedWeekdays[weekday] = selected;
}

// Helper function that converts any unsigned integer to a year usable in the tm struct
int Time::convertToValidYear(int year, bool hasBeenSubtracted, bool subtractTmBaseYear)
{
    // if subtractTmBaseYear = false just clamp, return
    // if subtactTmBaseYear = true, subtract, then clamp to diff range

    if (subtractTmBaseYear)
    {
        year -= 1900;
    }

    if (hasBeenSubtracted)
    {
        return std::max(std::min(year, 8000), -1900);
    }
    else
    {
        return std::max(std::min(year, 8000 + 1900), 0);
    }
}