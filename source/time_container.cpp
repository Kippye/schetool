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
void Time::setClockTime(unsigned int hour, unsigned int minute)
{
    m_stored_time.tm_hour = std::min(hour, 23U);
    m_stored_time.tm_min = std::min(minute, 59U);
}
void Time::setMonthDay(unsigned int day)
{
    m_stored_time.tm_mday = day;
}
void Time::setMonth(unsigned int month)
{
    m_stored_time.tm_mon = month - 1;
}
void Time::setYear(unsigned int year)
{
    m_stored_time.tm_year = year - 1900;
}

// Set the weekday at index weekday (0-6) selection to selected
void Time::setWeekdaySelected(unsigned int weekday, bool selected)
{
    weekday = std::min(weekday, 6U);
    m_selectedWeekdays[weekday] = selected;
}