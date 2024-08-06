#include <date_container.h>
#include <cmath>
#include "util.h"

DateContainer::DateContainer() 
{
}

DateContainer::DateContainer(const TimeWrapper& time)
{
    m_time = time;
}

std::string DateContainer::getString(bool asRelative) const
{
    if (m_time.getIsEmpty() == true)
    {
        return std::string("");
    }

    char output[1024];

    if (asRelative == false)
    {
        tm timeAsTm = m_time.getTm();
        std::strftime(output, sizeof(output), "%d/%m/%y", &timeAsTm); 
    }
    else
    {
        int offsetFromToday = getDayDifference(getCurrentSystemDate());

        if (offsetFromToday > 1)
        {
            sprintf(output, "in %d days", offsetFromToday);
        }
        else
        {        
            sprintf(output, offsetFromToday == 
            0 ? "Today"
            : "Tomorrow");
        }
    }

    return std::string(output);
}

bool DateContainer::getIsEmpty() const
{
    return m_time.getIsEmpty();
}

void DateContainer::clear()
{
    m_time.clear();
}

TimeWrapper& DateContainer::getTime()
{
    return m_time;
}

const TimeWrapper& DateContainer::getTimeConst() const
{
    return m_time;
}

void DateContainer::setTime(const TimeWrapper& time)
{
    m_time = time;
}

// Add 1 to the month day. If it reaches the amount of days in the month, the month day will be set to 1 and month will be incremented, too.
void DateContainer::incrementMonthDay()
{
    m_time.incrementMonthDay();
}

void DateContainer::decrementMonthDay()
{
    m_time.decrementMonthDay();
}

// Add 1 to the month. If it reaches 12 (0-indexed), the month will be set to 0 and year will be incremented, too.
void DateContainer::incrementMonth()
{
    m_time.incrementMonth();
}

void DateContainer::decrementMonth()
{
    m_time.decrementMonth();
}

void DateContainer::incrementYear()
{
    m_time.incrementYear();
}

void DateContainer::decrementYear()
{
    m_time.decrementYear();
}

int DateContainer::getDayDifference(const DateContainer& other) const
{
    int dayDifference = 0;
    tm thisTm = m_time.getTm();
    tm otherTm = other.getTimeConst().getTm();
    std::time_t thisTime = std::mktime(&thisTm);
    std::time_t otherTime = std::mktime(&otherTm);

    if (thisTime != (std::time_t)(-1) && otherTime != (std::time_t)(-1))
    {
        double difference = std::difftime(thisTime, otherTime) / (60 * 60 * 24);
        dayDifference = (int)std::round(difference);
    }

    return dayDifference;
}

// STATIC
DateContainer DateContainer::getCurrentSystemDate()
{ 
    return DateContainer(TimeWrapper::getCurrentSystemTime());
}
