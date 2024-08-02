#include <date_container.h>
#include <cmath>
#include "util.h"

DateContainer::DateContainer() 
{
    m_empty = true;
}
DateContainer::DateContainer(const tm& t)
{
    time = t;
    m_empty = false;
}

std::string DateContainer::getString(bool asRelative) const
{
    if (m_empty == true)
    {
        return std::string("");
    }

    char output[1024];

    if (asRelative == false)
    {
        std::strftime(output, sizeof(output), "%d/%m/%y", &time); 
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

tm DateContainer::getTime() const
{
    return time;
}

bool DateContainer::getIsEmpty() const
{
    return m_empty;
}

void DateContainer::clear()
{
    m_empty = true;
}

void DateContainer::setTime(const tm& time)
{
    this->time = tm(time);
    m_empty = false;
}

unsigned int DateContainer::getMonthDay() const
{
    return time.tm_mday;
}

void DateContainer::setMonthDay(unsigned int day)
{
    time.tm_mday = std::min(day, mytime::get_month_day_count(time));
    m_empty = false;
}

// Add 1 to the month day. If it reaches the amount of days in the month, the month day will be set to 1 and month will be incremented, too.
void DateContainer::incrementMonthDay()
{
    time.tm_mday++;
    // reset to 1 if past current month
    if (time.tm_mday > mytime::get_month_day_count(time))
    {
        time.tm_mday = 1;
        incrementMonth();
    }
}

void DateContainer::decrementMonthDay()
{
    time.tm_mday--;
    // reset to month day count if less than 1
    if (time.tm_mday < 1)
    {
        decrementMonth();
        time.tm_mday = mytime::get_month_day_count(time);
    }
}

unsigned int DateContainer::getMonth() const
{
    return time.tm_mon;
}

void DateContainer::setMonth(int month)
{
    time.tm_mon = month < 0 ? 11 : (month > 11 ? 0 : month);
    setMonthDay(time.tm_mday);
    m_empty = false;
}

// Add 1 to the month. If it reaches 12 (0-indexed), the month will be set to 0 and year will be incremented, too.
void DateContainer::incrementMonth()
{
    time.tm_mon++;

    if (time.tm_mon > 11)
    {
        time.tm_mon = 0;
        incrementYear();
    }
}

void DateContainer::decrementMonth()
{
    time.tm_mon--;

    if (time.tm_mon < 0)
    {
        time.tm_mon = 11;
        decrementYear();
    }
}

unsigned int DateContainer::getYear(bool asFull) const
{
    return asFull ? time.tm_year + 1900 : time.tm_year;
}

// Sets the year of this date (NOTE: Makes it not empty).
// convert - pass true if the input is a normal year, false if the input is a tm_year
void DateContainer::setYear(int year, bool convert)
{
    year = convertToValidYear(year, !convert, convert);
    time.tm_year = year;
    m_empty = false;
}

void DateContainer::incrementYear()
{
    time.tm_year = convertToValidYear(time.tm_year + 1);
}

void DateContainer::decrementYear()
{
    time.tm_year = convertToValidYear(time.tm_year - 1);
}

int DateContainer::getDayDifference(const DateContainer& other) const
{
    int dayDifference = 0;
    tm thisTm = time;
    tm otherTm = other.getTime();
    std::time_t thisTime = std::mktime(&thisTm);
    std::time_t otherTime = std::mktime(&otherTm);

    if (thisTime != (std::time_t)(-1) && otherTime != (std::time_t)(-1))
    {
        double difference = std::difftime(thisTime, otherTime) / (60 * 60 * 24);
        dayDifference = (int)std::round(difference);
    }

    return dayDifference;
}

// Helper function that converts any unsigned integer to a year usable in the tm struct.
// NOTE: hasBeenSubtracted is used to determine which range the year should be limited to (if 1900 has already been subtracted from the year).
// subtractTmBaseYear - if this is true, then 1900 will be subtracted from the year before doing other validation
int DateContainer::convertToValidYear(int year, bool hasBeenSubtracted, bool subtractTmBaseYear)
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

// STATIC
DateContainer DateContainer::getCurrentSystemDate()
{ 
    time_t time = std::time(0);
    tm now = *localtime(&time);
    return DateContainer(now);
}
