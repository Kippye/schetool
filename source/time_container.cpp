#include <time_container.h>
#include <iostream>

Time::Time()
{

}

Time::Time(const tm& time, bool _displayDate, bool _displayTime, bool _displayWeekday)
{
    m_stored_time = tm(time);
    displayDate = _displayDate;
    displayTime = _displayTime;
    displayWeekday = _displayWeekday;
}

const tm* Time::getTime()
{
    return &m_stored_time;
}
// Returns the string representation of the stored time. If full is true, returns the entire date and time, else [TODO] only the clock time.
std::string Time::getString()
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
            std::strftime(output, sizeof(output), "%A", &m_stored_time);
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

void Time::setTime(const tm& time)
{
    m_stored_time = tm(time);
}
void Time::setClockTime(unsigned int hour, unsigned int minute, unsigned int second)
{
    m_stored_time.tm_hour = hour;
    m_stored_time.tm_min = minute;
    m_stored_time.tm_sec = second;
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