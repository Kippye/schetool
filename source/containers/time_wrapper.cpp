#include "time_wrapper.h"

#include <chrono>
#include <iostream>
#include <ctime>
#include <format>

#include "util.h"

#ifdef __MINGW32__
#include "time_wrapper_impl_mingw.h"
#endif

using namespace std::chrono_literals;
using namespace std::chrono;

DateWrapper::DateWrapper(){}

DateWrapper::DateWrapper(unsigned int year, unsigned int month, unsigned int monthDay) : m_year(year), m_month(month), m_monthDay(monthDay)
{}

DateWrapper::DateWrapper(const year_month_day& date)
{
    m_year = std::max((int)date.year(), 1678);
    m_year = std::min(m_year, 2261u);
    m_month = std::max((unsigned int)date.month(), 1u);
    m_month = std::min(m_month, 12u);
    m_monthDay = std::max((unsigned int)date.day(), 1u);
    m_monthDay = std::min(m_monthDay, 31u);
}

unsigned int DateWrapper::getYear() const
{
    return m_year;
}
unsigned int DateWrapper::getMonth() const
{
    return m_month;
}
unsigned int DateWrapper::getMonthDay() const
{
    return m_monthDay;
}


ClockTimeWrapper::ClockTimeWrapper(){}

ClockTimeWrapper::ClockTimeWrapper(unsigned int hours, unsigned int minutes)
{
    m_hours = std::max(0u, std::min(hours, 23u));
    m_minutes = std::max(0u, std::min(minutes, 59u));;
}

ClockTimeWrapper::ClockTimeWrapper(const hh_mm_ss<seconds>& hms)
{
    m_hours = hms.hours().count();
    m_minutes = hms.minutes().count();
}

unsigned int ClockTimeWrapper::getHours() const
{
    return m_hours;
}

unsigned int ClockTimeWrapper::getMinutes() const
{
    return m_minutes;
}


TimeWrapper::TimeWrapper() : m_empty(true)
{}

TimeWrapper::TimeWrapper(const time_point<system_clock>& tp) : m_time(tp), m_empty(false)
{}

TimeWrapper::TimeWrapper(const DateWrapper& date)
{
    setTime(date, {});
}

TimeWrapper::TimeWrapper(const ClockTimeWrapper& clockTime)
{
    setTime({}, clockTime);
}

TimeWrapper::TimeWrapper(const DateWrapper& date, const ClockTimeWrapper& time)
{
    setTime(date, time);
}

TimeWrapper::TimeWrapper(unsigned int year, unsigned int month, unsigned int monthDay) : TimeWrapper(DateWrapper{year, month, monthDay})
{}

// Static function.
// Takes a tm struct and returns it as a time_t in UTC time.
// Thanks to the legend Howard Hinnant from StackOverflow!
std::time_t TimeWrapper::custom_timegm(const std::tm& t)
{
    auto ymd = year{1900 + t.tm_year}/(1 + t.tm_mon)/t.tm_mday;

    return system_clock::to_time_t(
        sys_days{ymd} + hours{t.tm_hour} + minutes{t.tm_min}
    );
}

// Static function.
// Turns the input date and time into a time_t in UTC time.
std::time_t TimeWrapper::time_to_utc_time_t(const DateWrapper& date, const ClockTimeWrapper& time)
{
    auto ymd = year{(int)date.getYear()}/(date.getMonth())/date.getMonthDay();

    return system_clock::to_time_t(
        sys_days{ymd} + hours{time.getHours()} + minutes{time.getMinutes()}
    );
}

tm TimeWrapper::getTm(const DateWrapper& date, const ClockTimeWrapper& time) const
{
    return tm
    {
        0,
        (int)time.getMinutes(), (int)time.getHours(),
        (int)date.getMonthDay(), (int)date.getMonth() - 1, (int)date.getYear() - 1900,
        0,
        0
    };
}

void TimeWrapper::setTime(const DateWrapper& date, const ClockTimeWrapper& time)
{
    m_time = system_clock::from_time_t(time_to_utc_time_t(date, time));
    m_empty = false;
}

time_point<system_clock> TimeWrapper::getUtcTime() const
{
    return m_time;
}

local_time<seconds> TimeWrapper::getLocalTime(const std::string& timezoneName) const
{
    const time_zone* timezone = current_zone();
    bool usingSpecificTimezone = false;
    if (timezoneName != "")
    {
        try
        {
            timezone = get_tzdb().locate_zone(timezoneName);
            usingSpecificTimezone = true;
        }
        catch(const std::runtime_error& err)
        {
            printf("TimeWrapper::getLocalTime(%s): %s\n", timezoneName.c_str(), err.what());
        }
    }
    auto zonedTime = zoned_time(timezone, m_time);
    auto localTime = floor<seconds>(zonedTime.get_local_time());

    // Patch for MinGW (not specifically MinGW32).
    // At least for me, getting current_zone() just returned UTC every time. This was not an issue with MSVC or non-MinGW gcc.
    // As a "fix", on MinGW32 the WIN32 API is used to get time zone information (UTC offset and DST offset, if it is active).
    // It probably has more issues than the usual implementation.
    #ifdef __MINGW32__
    if (usingSpecificTimezone == false)
    {
        applyLocalTimeOffsetFromUTC(localTime);
    }
    #else
    // std::cout << "UTC offset: " << zonedTime.get_info().offset << std::endl;
    #endif

    // int i = 0;
    // for (const  auto& z : std::chrono::get_tzdb().zones)
    // {    
    //     std::cout << i << ": " <<  z.name() << std::endl;   
    //     i++;
    // }        

    return localTime;
}

ClockTimeWrapper TimeWrapper::getClockTime() const
{
    auto localTime = getLocalTime();
    auto localDays = floor<days>(localTime);
    return ClockTimeWrapper(hh_mm_ss(floor<seconds>(localTime - localDays)));
} 

DateWrapper TimeWrapper::getDate() const
{
    auto localTime = getLocalTime();
    auto localDays = floor<days>(localTime);
    return DateWrapper(year_month_day(localDays));
}

void TimeWrapper::setDate(const DateWrapper& date)
{
    setTime(date, getClockTime());
}

std::string TimeWrapper::getString(TIME_FORMAT fmtType, const std::optional<local_time<seconds>> time) const
{
    const auto& timeToFormat = time.has_value() ? time.value() : getLocalTime();

    if (m_empty) { return ""; }
    switch(fmtType) 
    {
        case(TIME_FORMAT_DATE):
            return std::format("{:%d/%m/%Y}", timeToFormat);
        case(TIME_FORMAT_TIME):
            return std::format("{:%R}", timeToFormat);
        case(TIME_FORMAT_FULL):
        default:
            return std::format("{:%d/%m/%Y %R}", timeToFormat);
    }
}

std::string TimeWrapper::getDynamicFmtString(const std::string_view& fmt, const std::optional<local_time<seconds>> time) const
{
    const auto& timeToFormat = time.has_value() ? time.value() : getLocalTime();
    return std::vformat(fmt, std::make_format_args(timeToFormat));
}

bool TimeWrapper::getIsEmpty() const
{
    return m_empty;
}

void TimeWrapper::clear()
{
    setTime(DateWrapper(1970, 1, 1), ClockTimeWrapper(0, 0));
    m_empty = true;
}

unsigned int TimeWrapper::getMonthDay(bool basedness) const
{
    return basedness == ONE_BASED ? getDate().getMonthDay() : getDate().getMonthDay() - 1;
}

void TimeWrapper::setMonthDay(unsigned int day, bool basedness)
{
    DateWrapper currentDate = getDate();
    unsigned int max = basedness == ONE_BASED ? mytime::get_month_day_count(currentDate.getYear(), currentDate.getMonth()) // 1..dayCount
    : mytime::get_month_day_count(currentDate.getYear(), currentDate.getMonth()) - 1; // 0..dayCount - 1

    day = std::max(day, (unsigned int)basedness); // day >= basedness
    day = std::min(day, max); // day <= max day

    DateWrapper newDate = DateWrapper(currentDate.getYear(),
        currentDate.getMonth(),
        basedness == ONE_BASED ? day : day + 1
    );
    setTime(newDate, getClockTime());
}

void TimeWrapper::addDays(int dayCount)
{
    m_time += days{dayCount};
}

unsigned int TimeWrapper::getWeekDay(bool weekStart, bool basedness) const
{
    auto zonedDays = floor<days>(getLocalTime());
    year_month_weekday ymw = year_month_weekday(zonedDays);
    unsigned int weekday = weekStart == WEEK_START_MONDAY ? ymw.weekday().iso_encoding() - 1 // The ISO encoding means Monday = 1,  Sunday = 7
        : ymw.weekday().c_encoding(); // The C encoding means Sunday = 0, Saturday = 6

    return basedness + weekday;
}

unsigned int TimeWrapper::getMonth(bool basedness) const
{
    return basedness == ONE_BASED ? getDate().getMonth() : getDate().getMonth() - 1;
}

void TimeWrapper::setMonth(int month, bool basedness)
{
    DateWrapper currentDate = getDate();
    DateWrapper newDate = DateWrapper(currentDate.getYear(), 
        basedness == ONE_BASED ? (month < 1 ? 12 : (month > 12 ? 1 : month))
        : (month < 0 ? 12 : (month > 11 ? 1 : month + 1)),
        currentDate.getMonthDay()
    );
    setTime(newDate, getClockTime());
}

void TimeWrapper::addMonths(int monthCount)
{
    m_time += months{monthCount};
}

unsigned int TimeWrapper::getYear() const
{
    return getDate().getYear();
}

void TimeWrapper::setYear(unsigned int year)
{
    DateWrapper currentDate = getDate();
    DateWrapper newDate = DateWrapper(year, currentDate.getMonth(), currentDate.getMonthDay());
    setTime(newDate, getClockTime());
}

TimeWrapper TimeWrapper::getCurrentTime()
{
    // #ifdef PERFORM_UNIT_TESTS
    // if (testOverrideCurrentTime.getIsEmpty() == false)
    // {
    //     return testOverrideCurrentTime;
    // }
    // #endif
    return TimeWrapper(utc_clock::to_sys(utc_clock::now()));
}

int TimeWrapper::limitYearToValidRange(int year)
{
    year = std::max(year, 1678);
    year = std::min(year, 2261);
    return year;
}