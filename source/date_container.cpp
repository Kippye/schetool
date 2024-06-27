#include <date_container.h>
#include "util.h"

DateContainer::DateContainer() {}
DateContainer::DateContainer(const tm& t, bool isRelative, int relativeOffset)
{
    time = t;
    m_isRelative = isRelative;
    m_relativeOffset = relativeOffset;
}

std::string DateContainer::getString() const
{
    char output[1024];

    if (m_isRelative == false)
    {
        std::strftime(output, sizeof(output), "%d/%m/%y", &time); 
    }
    else
    {
        if (m_relativeOffset > 1)
        {
            sprintf(output, "in %d days", m_relativeOffset);
        }
        else
        {        
            sprintf(output, m_relativeOffset == 
            0 ? "Today"
            : "Tomorrow");
        }
    }

    return std::string(output);
}

tm DateContainer::getTime() const
{
    if (m_isRelative)
    {
        DateContainer relativeDate = getCurrentSystemDate();
        // funny way to apply the offset, but we just increment the day by 1 over and over again, with any month / year changes being handled automatically
        for (int i = 0; i < m_relativeOffset; i++)
        {
            relativeDate.incrementMonthDay();
        }
        return relativeDate.getTime();
    }

    return time;
}

bool DateContainer::getIsRelative() const
{
    return m_isRelative;
}

size_t DateContainer::getRelativeOffset() const
{
    return m_relativeOffset;
}

void DateContainer::setTime(const tm& time)
{
    this->time = tm(time);
}

// NOTE: If the day provided is more than the days in the current month, the day will be clamped to it
void DateContainer::setMonthDay(unsigned int day)
{
    time.tm_mday = std::min(day, mytime::get_month_day_count(time));
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

// NOTE: If the given month is < 0, it will be set to 11. If it's > 11, it will be set to 0
void DateContainer::setMonth(int month)
{
    time.tm_mon = month < 0 ? 11 : (month > 11 ? 0 : month);
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

// NOTE: hasBeenSubtracted is used to determine which range the year should be limited to (if 1900 has already been subtracted from the year)
// subtractTmBaseYear - if this is true, then 1900 will be subtracted from the year before doing other validation
void DateContainer::setYear(int year, bool convert)
{
    year = convertToValidYear(year, !convert, convert);
    time.tm_year = year;
}

void DateContainer::incrementYear()
{
    time.tm_year = convertToValidYear(time.tm_year + 1);
}

// Helper function that converts any unsigned integer to a year usable in the tm struct
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
