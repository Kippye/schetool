#include <algorithm>
#include <cctype>
#include <ctime>
#include <iterator>
#include <string.h>
#include <date_container.h>
#include <iostream>

Date::Date()
{

}

Date::Date(const tm& time)
{
    m_stored_time = tm(time);
}

const tm* Date::getTime()
{
    return &m_stored_time;
}
// Returns the string representation of the stored date
std::string Date::getString()
{
    try
    {
        char output[1024];

        std::strftime(output, sizeof(output), "%d/%m/%y", &m_stored_time);

        return std::string(output);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << "\n";
        std::cout << "std::string Date::getString(): Failed to convert current date to a string. Invalid date?" << std::endl;
        return std::string("");
    }
}

void Date::setTime(const tm& time)
{
    m_stored_time = tm(time);
}

void Date::setMonthDay(unsigned int day)
{
    m_stored_time.tm_mday = day;
}
// NOTE: If the given month is < 0, it will be set to 11. If it's > 11, it will be set to 0
void Date::setMonth(int month)
{
    m_stored_time.tm_mon = month < 0 ? 11 : (month > 11 ? 0 : month);
}
// NOTE: hasBeenSubtracted is used to determine which range the year should be limited to (if 1900 has already been subtracted from the year)
// subtractTmBaseYear - if this is true, then 1900 will be subtracted from the year before doing other validation
void Date::setYear(int year, bool convert)
{
    year = convertToValidYear(year, !convert, convert);
    m_stored_time.tm_year = year;
}

// Helper function that converts any unsigned integer to a year usable in the tm struct
int Date::convertToValidYear(int year, bool hasBeenSubtracted, bool subtractTmBaseYear)
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