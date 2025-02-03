#include "time_wrapper.h"

#include <chrono>

#include "util.h"

#ifdef __MINGW32__
#include "time_wrapper_mingw_impl.h"
#endif

using namespace std::chrono_literals;
using namespace std::chrono;

DateWrapper::DateWrapper(){}

DateWrapper::DateWrapper(unsigned int year, unsigned int month, unsigned int monthDay)
{
    m_year = std::max(year, 1678u);
    m_year = std::min(m_year, 2261u);
    m_month = std::max(month, 1u);
    m_month = std::min(m_month, 12u);
    m_monthDay = std::max(monthDay, 1u);
    m_monthDay = std::min(m_monthDay, 31u);
}

DateWrapper::DateWrapper(const year_month_day& date) : DateWrapper((int)date.year(), (unsigned int)date.month(), (unsigned int)date.day())
{}

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
    setTimeUTC(date, {});
}

TimeWrapper::TimeWrapper(const ClockTimeWrapper& clockTime)
{
    setTimeUTC({}, clockTime);
}

TimeWrapper::TimeWrapper(const DateWrapper& date, const ClockTimeWrapper& time)
{
    setTimeUTC(date, time);
}

TimeWrapper::TimeWrapper(unsigned int year, unsigned int month, unsigned int monthDay) : TimeWrapper(DateWrapper{year, month, monthDay})
{}

// Static function.
// Turns the input date and time into a time_t in UTC time.
// Thanks to the legend Howard Hinnant from StackOverflow!
std::time_t TimeWrapper::time_to_utc_time_t(const DateWrapper& date, const ClockTimeWrapper& time)
{
    auto ymd = year{(int)date.getYear()}/(date.getMonth())/date.getMonthDay();

    return system_clock::to_time_t(
        sys_days{ymd} + hours{time.getHours()} + minutes{time.getMinutes()}
    );
}


time_point<system_clock> TimeWrapper::getTimeUTC() const
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
    #endif

    return localTime;
}

bool TimeWrapper::getIsEmpty() const
{
    return m_empty;
}

void TimeWrapper::clear()
{
    setTimeUTC(DateWrapper(1970, 1, 1), ClockTimeWrapper(0, 0));
    m_empty = true;
}

DateWrapper TimeWrapper::getDateUTC() const
{
    return getDate(m_time);
}

DateWrapper TimeWrapper::getLocalDate() const
{
    return getDate(getLocalTime());
}

ClockTimeWrapper TimeWrapper::getClockTimeUTC() const
{
    return getClockTime(m_time);
} 

ClockTimeWrapper TimeWrapper::getLocalClockTime() const
{
    return getClockTime(getLocalTime());
}

void TimeWrapper::setTimeUTC(const DateWrapper& date, const ClockTimeWrapper& time)
{
    m_time = system_clock::from_time_t(time_to_utc_time_t(date, time));
    m_empty = false;
}

void TimeWrapper::setDateUTC(const DateWrapper& date)
{
    setTimeUTC(date, getClockTimeUTC());
}

unsigned int TimeWrapper::getMonthDayUTC(bool basedness) const
{
    return getMonthDay(m_time, basedness);
}

unsigned int TimeWrapper::getMonthDay(bool basedness) const
{
    return getMonthDay(getLocalTime(), basedness);
}

void TimeWrapper::setMonthDayUTC(unsigned int day, bool basedness)
{
    DateWrapper currentDate = getLocalDate();
    unsigned int max = basedness == ONE_BASED ? mytime::get_month_day_count(currentDate.getYear(), currentDate.getMonth()) // 1..dayCount
    : mytime::get_month_day_count(currentDate.getYear(), currentDate.getMonth()) - 1; // 0..dayCount - 1

    day = std::max(day, (unsigned int)basedness); // day >= basedness
    day = std::min(day, max); // day <= max day

    DateWrapper newDate = DateWrapper(currentDate.getYear(),
        currentDate.getMonth(),
        basedness == ONE_BASED ? day : day + 1
    );
    setTimeUTC(newDate, getLocalClockTime());
}

void TimeWrapper::addDays(int dayCount)
{
    m_time += days{dayCount};
}

unsigned int TimeWrapper::getWeekdayUTC(bool weekStart, bool basedness) const
{
    return getWeekday(m_time, weekStart, basedness);
}

unsigned int TimeWrapper::getWeekday(bool weekStart, bool basedness) const
{
    return getWeekday(getLocalTime(), weekStart, basedness);
}

unsigned int TimeWrapper::getMonthUTC(bool basedness) const
{
    return getMonth(m_time, basedness);
}

unsigned int TimeWrapper::getMonth(bool basedness) const
{
    return getMonth(getLocalTime(), basedness);
}

void TimeWrapper::setMonthUTC(int month, bool basedness)
{
    DateWrapper currentDate = getLocalDate();
    DateWrapper newDate = DateWrapper(currentDate.getYear(), 
        basedness == ONE_BASED ? (month < 1 ? 12 : (month > 12 ? 1 : month))
        : (month < 0 ? 12 : (month > 11 ? 1 : month + 1)),
        currentDate.getMonthDay()
    );
    setTimeUTC(newDate, getLocalClockTime());
}

void TimeWrapper::addMonths(int monthCount)
{
    m_time += months{monthCount};
}

unsigned int TimeWrapper::getYearUTC() const
{
    return getDateUTC().getYear();
}

unsigned int TimeWrapper::getYear() const
{
    return getLocalDate().getYear();
}

void TimeWrapper::setYearUTC(unsigned int year)
{
    DateWrapper currentDate = getLocalDate();
    DateWrapper newDate = DateWrapper(year, currentDate.getMonth(), currentDate.getMonthDay());
    setTimeUTC(newDate, getLocalClockTime());
}

std::string TimeWrapper::getStringUTC(TIME_FORMAT fmtType) const
{
    if (m_empty) { return ""; }
    return TimeWrapper::getString(m_time, fmtType);
}

std::string TimeWrapper::getString(TIME_FORMAT fmtType) const
{
    if (m_empty) { return ""; }
    return TimeWrapper::getString(getLocalTime(), fmtType);
}
 
std::string TimeWrapper::getDynamicFmtStringUTC(const std::string_view& fmt) const
{
    if (m_empty) { return ""; }
    return TimeWrapper::getDynamicFmtString(m_time, fmt);
}

std::string TimeWrapper::getDynamicFmtString(const std::string_view& fmt) const
{
    if (m_empty) { return ""; }
    return TimeWrapper::getDynamicFmtString(getLocalTime(), fmt);
}


TimeWrapper TimeWrapper::getCurrentTime()
{
    #ifdef PERFORM_UNIT_TESTS
    if (testCurrentTimeOverride.getIsEmpty() == false)
    {
        return testCurrentTimeOverride;
    }
    #endif
    return TimeWrapper(system_clock::now());
}

TimeWrapper TimeWrapper::getTimeWithOffsetSubtracted(const TimeWrapper &base)
{
    return TimeWrapper(base.getTimeUTC() - TimeWrapper::getTimezoneOffset());
}

int TimeWrapper::limitYearToValidRange(int year)
{
    year = std::max(year, 1678);
    year = std::min(year, 2261);
    return year;
}

chrono::minutes TimeWrapper::getTimezoneOffset()
{
    TimeWrapper time = TimeWrapper(DateWrapper(2025, 1, 14), ClockTimeWrapper(18, 00));
    return chrono::floor<minutes>(time.getLocalTime().time_since_epoch() - time.getTimeUTC().time_since_epoch());
}

#ifdef PERFORM_UNIT_TESTS
TimeWrapper TimeWrapper::testCurrentTimeOverride = TimeWrapper();
#endif