#include "util.h"
#include "time_wrapper.h"

ClockTimeWrapper::ClockTimeWrapper(){}

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

TimeWrapper::TimeWrapper(const tm& t) : m_clockTime(t.tm_hour, t.tm_min)
{
    setTime(t);
}

TimeWrapper::TimeWrapper(unsigned int year, unsigned int month, unsigned int monthDay)
{
    setYear(year);
    setMonth(month);
    setMonthDay(monthDay);
}

TimeWrapper::TimeWrapper(const ClockTimeWrapper& clockTime) : m_clockTime(clockTime)
{}

TimeWrapper::TimeWrapper(const ClockTimeWrapper& clockTime, unsigned int year, unsigned int month, unsigned int monthDay) : TimeWrapper(year, month, monthDay)
{
    m_clockTime = clockTime;
}

ClockTimeWrapper& TimeWrapper::getClockTime()
{
    return m_clockTime;
}

tm TimeWrapper::getTm() const
{
    tm timeAsTm = tm();
    timeAsTm.tm_year = toYearsSinceTm(m_year);
    timeAsTm.tm_mon = m_month - 1;
    timeAsTm.tm_mday = m_monthDay;
    timeAsTm.tm_hour = m_clockTime.getHours();
    timeAsTm.tm_min = m_clockTime.getMinutes();
    time_t asTime = mktime(&timeAsTm);
    return *localtime(&asTime);
}

std::string TimeWrapper::getString() const
{
    std::cout << m_clockTime.getHours() << std::endl;
    tm timeTm = getTm();
    std::cout << timeTm.tm_hour << std::endl;
    char timeBuf[256];
    strftime(timeBuf, sizeof(timeBuf), "%x (%a %b %d) %H:%M", &timeTm);
    return std::string(timeBuf);
}

bool TimeWrapper::getIsEmpty() const
{
    return m_empty;
}

void TimeWrapper::clear()
{
    // reset date, but set month day to 1 since that is already a valid date (01/01/1900)
    m_year = 1900;
    m_month = 1;
    m_monthDay = 1;
    m_empty = true;
}

void TimeWrapper::setTime(const tm& time)
{
    setYear(fromYearsSinceTm(time.tm_year));
    setMonth(time.tm_mon, ZERO_BASED);
    setMonthDay(time.tm_mday);
    m_empty = false;
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
    unsigned int weekday = getTm().tm_wday; 
    // tm weekdays start from Sunday by default, so modify it here if it has to start from Monday instead.
    if (weekStart == WEEK_START_MONDAY)
    {
        weekday = weekday == 0 ? 6 : weekday - 1;
    }

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
    #ifdef PERFORM_UNIT_TESTS
    if (testOverrideCurrentTime.getIsEmpty() == false)
    {
        return testOverrideCurrentTime;
    }
    #endif
    time_t time = std::time(0);
    tm now = *localtime(&time);
    return TimeWrapper(now);
}

#ifdef PERFORM_UNIT_TESTS
TimeWrapper TimeWrapper::testOverrideCurrentTime = TimeWrapper();
#endif