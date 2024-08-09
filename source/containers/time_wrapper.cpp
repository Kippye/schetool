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

void test()
{
    // UTC time
    auto utcNow = utc_clock::now();
    time_t utcTimeT = system_clock::to_time_t(utc_clock::to_sys(utcNow));
    std::cout << utcNow.time_since_epoch().count() << std::endl;
    tm UTC;
    UTC = *std::gmtime(&utcTimeT);
    char utcBuf[256];
    strftime(utcBuf, sizeof(utcBuf), "UTC time: %c timezone: %z", &UTC);
    std::cout << utcBuf << std::endl;

    // Ok let's get the local time now.
    tm localTime;
    localTime = *std::localtime(&utcTimeT);
    char locBuf[256];
    strftime(locBuf, sizeof(locBuf), "Local time: %c timezone: %z", &localTime);
    std::cout << locBuf << std::endl;

    // Ok now let's get the local date without using C functions
    auto sysNow = system_clock::now();
    std::cout << "Current TZ name: " << current_zone()->name() << std::endl;
    std::cout << "Current TZ offset: " << current_zone()->get_info(sysNow).offset << std::endl;
    std::cout << "Current TZ name: " << current_zone()->get_info(sysNow).abbrev << std::endl;
    auto zonedTime = zoned_time(current_zone(), sysNow);
    std::cout << "UTC offset: " << zonedTime.get_info().offset << std::endl;
    auto localZoneTime = zonedTime.get_local_time();
    std::cout << sysNow.time_since_epoch().count() << std::endl;
    std::cout << localZoneTime.time_since_epoch().count() << std::endl;
    // Truncate to days
    auto zonedDays = floor<days>(localZoneTime);
    year_month_day date = year_month_day(zonedDays);
    // zonedDays is truncated to days. So if we get the difference between localZoneTime and zonedDays, we get the amount of time left that is smaller than a day.
    hh_mm_ss clockTime = hh_mm_ss(floor<seconds>(localZoneTime - zonedDays));

    std::cout << std::format("{}{}", clockTime.hours(), clockTime.minutes()) << std::endl;

    // Output as formatted :D
    std::cout << std::format("Local date and time: {:%m/%d/%Y %R}", localZoneTime) << std::endl;

    // Convert year_month_day to a time point!
    // std::system_clock::time_point tp = std::sys_days{_ymd};

    // Use on android (and linux): ?
    // -DUSE_OS_TZDB=1
}

ClockTimeWrapper::ClockTimeWrapper(){}

ClockTimeWrapper::ClockTimeWrapper(const TimeInput& time)
{
    setHours(time.hours);
    setMinutes(time.minutes);
}

ClockTimeWrapper::ClockTimeWrapper(unsigned int hours, unsigned int minutes)
{
    setHours(hours);
    setMinutes(minutes);
}

unsigned int ClockTimeWrapper::getHours() const
{
    return m_hours;
}

void ClockTimeWrapper::setHours(unsigned int hours)
{
    m_hours = std::max(0u, std::min(hours, 23u));
}

unsigned int ClockTimeWrapper::getMinutes() const
{
    return m_minutes;
}

void ClockTimeWrapper::setMinutes(unsigned int minutes)
{
    m_minutes = std::max(0u, std::min(minutes, 59u));
}


TimeWrapper::TimeWrapper() : m_empty(true)
{}

TimeWrapper::TimeWrapper(const time_point<system_clock>& tp) : m_time(tp)
{}

TimeWrapper::TimeWrapper(const DateInput& date)
{
    setTime(date, {});
}

TimeWrapper::TimeWrapper(const ClockTimeWrapper& clockTime) : m_clockTime(clockTime)
{}

TimeWrapper::TimeWrapper(const DateInput& date, const TimeInput& time)
{
    setTime(date, time);
}

TimeWrapper::TimeWrapper(unsigned int year, unsigned int month, unsigned int monthDay) : TimeWrapper(DateInput{year, month, monthDay})
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
std::time_t TimeWrapper::time_to_utc_time_t(const DateInput& date, const TimeInput& time)
{
    auto ymd = year{(int)date.year}/(date.month)/date.monthDay;

    return system_clock::to_time_t(
        sys_days{ymd} + hours{time.hours} + minutes{time.minutes}
    );
}

tm TimeWrapper::getTm(const DateInput& date, const TimeInput& time) const
{
    return tm
    {
        0,
        (int)time.minutes, (int)time.hours,
        (int)date.monthDay, (int)date.month - 1, (int)date.year - 1900,
        0,
        0
    };
}

void TimeWrapper::setTime(const DateInput& date, const TimeInput& time)
{
    m_time = system_clock::from_time_t(time_to_utc_time_t(date, time));
}

local_time<seconds> TimeWrapper::getLocalTime() const
{
    auto zonedTime = zoned_time(current_zone(), m_time);
    auto localTime = floor<seconds>(zonedTime.get_local_time());
    auto localDays = floor<days>(localTime);
    year_month_day date = year_month_day(localDays);

    // Patch for MinGW (not only MinGW).
    // At least for me, getting current_zone() just returned UTC every time. This was not an issue with MSVC or non-MinGW gcc.
    // As a "fix", on MinGW32 the WIN32 API is used to get time zone information (UTC offset and DST offset, if it is active).
    // It probably has more issues than the usual implementation.
    #ifdef __MINGW32__
    int utcOffsetMins = getTimeZoneOffsetMinutesUTC((int)date.year());
    // std::cout << "UTC offset: " << utcOffsetMins << " (min)" << std::endl;
    // Add UTC offset BEFORE calling function to get DST offset.
    localTime += minutes{utcOffsetMins};
    // Update all the other kinds of time since the day etc might have changed and the clock time DEFINITELY did
    localDays = floor<days>(localTime);
    date = year_month_day(localDays);
    year_month_weekday weekday = year_month_weekday(localDays);
    hh_mm_ss localClockTime = hh_mm_ss(floor<seconds>(localTime - localDays));
    
    // Silly check to see if the weekday is the last of its kind in the month
    auto nextWeekDays = localDays + days{7};
    bool isLastWeekdayIndex = year_month_weekday(nextWeekDays).index() == 1;
    
    int daylightSavingsOffsetMins = getDaylightSavingsOffsetMinutes(
        (int)date.year(),
        (unsigned int)date.month(),
        weekday.weekday().c_encoding(),
        weekday.index(),
        isLastWeekdayIndex,
        (unsigned int)localClockTime.hours().count(),
        (unsigned int)localClockTime.minutes().count()
    );
    // std::cout << "DST offset: " << daylightSavingsOffsetMins << " (min)" << std::endl;
    // Apply DST offset
    localTime += minutes{daylightSavingsOffsetMins};
    #else
    // std::cout << "UTC offset: " << zonedTime.get_info().offset << std::endl;
    #endif

    return localTime;
}

ClockTimeWrapper& TimeWrapper::getClockTime()
{
    return m_clockTime;
} 

std::string TimeWrapper::getString(TIME_FORMAT fmtType) const
{
    switch(fmtType) 
    {
        case(TIME_FORMAT_DATE):
            return std::format("{:%d/%m/%Y}", getLocalTime());
        case(TIME_FORMAT_TIME):
            return std::format("{:%R}", getLocalTime());
        case(TIME_FORMAT_FULL):
        default:
            return std::format("{:%d/%m/%Y %R}", getLocalTime());
    }
}

bool TimeWrapper::getIsEmpty() const
{
    return m_empty;
}

void TimeWrapper::clear()
{
    // TODO: Reset date to some value
    m_empty = true;
}

unsigned int TimeWrapper::getMonthDay(bool basedness) const
{
    return basedness == ONE_BASED ? m_monthDay : m_monthDay - 1;
}

void TimeWrapper::setMonthDay(unsigned int day, bool basedness)
{
    unsigned int max = basedness == ONE_BASED ? mytime::get_month_day_count(m_year, m_month) // 1..dayCount
    : mytime::get_month_day_count(m_year, m_month) - 1; // 0..dayCount - 1

    m_monthDay = std::min(day, max);
    m_empty = false;
}

// Add 1 to the month day. If it reaches the amount of days in the month, the month day will be set to 1 and month will be incremented, too.
void TimeWrapper::incrementMonthDay()
{
    m_monthDay++;
    // reset to 1 if past current month
    if (m_monthDay > mytime::get_month_day_count(m_year, m_month))
    {
        m_monthDay = 1;
        incrementMonth();
    }
}

void TimeWrapper::decrementMonthDay()
{
    m_monthDay--;
    // reset to month day count if less than 1
    if (m_monthDay < 1)
    {
        decrementMonth();
        m_monthDay = mytime::get_month_day_count(m_year, m_month);
    }
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
    return basedness == ONE_BASED ? m_month : m_month - 1;
}

void TimeWrapper::setMonth(int month, bool basedness)
{
    if (basedness == ONE_BASED)
    {
        m_month = month < 1 ? 12 : (month > 12 ? 1 : month);
    }
    if (basedness == ZERO_BASED)
    {
        m_month = month < 0 ? 12 : (month > 11 ? 1 : month + 1);
    }
    // min the month day to the new month
    setMonthDay(m_monthDay);
    m_empty = false;
}

// Add 1 to the month. If it reaches 13, the month will be set to 1 and year will be incremented, too.
void TimeWrapper::incrementMonth()
{
    m_month++;

    if (m_month > 12)
    {
        m_month = 1;
        incrementYear();
    }
}

void TimeWrapper::decrementMonth()
{
    m_month--;

    if (m_month < 1)
    {
        m_month = 12;
        decrementYear();
    }
}

unsigned int TimeWrapper::getYear() const
{
    return m_year;
}

void TimeWrapper::setYear(unsigned int year)
{
    m_year = toValidYear(year);
    m_empty = false;
}

void TimeWrapper::incrementYear()
{
    m_year = toValidYear(++m_year);
}

void TimeWrapper::decrementYear()
{
    m_year = toValidYear(--m_year);
}

unsigned int TimeWrapper::toYearsSinceTm(unsigned int year)
{
    return std::min((int(year) - 1900), 1900);
}

unsigned int TimeWrapper::fromYearsSinceTm(unsigned int year)
{
    return toValidYear(1900u + year);
}

unsigned int TimeWrapper::toValidYear(unsigned int year)
{
    return std::max(std::min(year, 8000u), 1900u);
}

TimeWrapper TimeWrapper::getCurrentSystemTime()
{
    // #ifdef PERFORM_UNIT_TESTS
    // if (testOverrideCurrentTime.getIsEmpty() == false)
    // {
    //     return testOverrideCurrentTime;
    // }
    // #endif
    // time_t time = std::time(0);
    // tm now = *localtime(&time);
    // return TimeWrapper(now);
    return TimeWrapper(utc_clock::to_sys(utc_clock::now()));
}