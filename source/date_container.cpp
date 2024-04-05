#include <date_container.h>

DateContainer::DateContainer() {}
DateContainer::DateContainer(const tm& t)
{
    time = t;
}

std::string DateContainer::getString() const
{
    char output[1024];

    std::strftime(output, sizeof(output), "%d/%m/%y", &time);

    return std::string(output);
}

const tm* DateContainer::getTime() const
{
    return &time;
}

void DateContainer::setTime(const tm& time)
{
    this->time = tm(time);
}

void DateContainer::setMonthDay(unsigned int day)
{
    time.tm_mday = day;
}
// NOTE: If the given month is < 0, it will be set to 11. If it's > 11, it will be set to 0
void DateContainer::setMonth(int month)
{
    time.tm_mon = month < 0 ? 11 : (month > 11 ? 0 : month);
}
// NOTE: hasBeenSubtracted is used to determine which range the year should be limited to (if 1900 has already been subtracted from the year)
// subtractTmBaseYear - if this is true, then 1900 will be subtracted from the year before doing other validation
void DateContainer::setYear(int year, bool convert)
{
    year = convertToValidYear(year, !convert, convert);
    time.tm_year = year;
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