#pragma once

#include <string>
#include <memory>
#include <chrono>

typedef std::chrono::system_clock::time_point utc_tp;
using namespace std::chrono;

template<class T>
struct is_duration : std::false_type{};
template<class Rep, class Period>
struct is_duration<duration<Rep, Period>> : std::true_type{};

inline const bool WEEK_START_MONDAY = 0;
inline const bool WEEK_START_SUNDAY = 1;
inline const bool ZERO_BASED = 0;
inline const bool ONE_BASED = 1;

enum TIME_FORMAT
{
    TIME_FORMAT_DATE = 1,
    TIME_FORMAT_TIME = 2,
    TIME_FORMAT_FULL = 3,
};

struct TimeInput
{
    unsigned int hours = 0;
    unsigned int minutes = 0;

    TimeInput(unsigned int h = 0, unsigned int min = 0) : hours(h), minutes(min)
    {}
};

struct DateInput
{
    unsigned int year;
    unsigned int month = 1;
    unsigned int monthDay = 1;

    DateInput(unsigned int y, unsigned int m = 1, unsigned int mDay =  1) : year(y), month(m), monthDay(mDay)
    {}
};

class ClockTimeWrapper
{
    private:
        unsigned int m_hours = 0, m_minutes = 0;
    public:
        ClockTimeWrapper();
        ClockTimeWrapper(const TimeInput& time);
        ClockTimeWrapper(unsigned int hours, unsigned int minutes);

        unsigned int getHours() const;
        void setHours(unsigned int hours);

        unsigned int getMinutes() const;
        void setMinutes(unsigned int minutes);
};

class TimeWrapper
{
    private:
        bool m_empty = true;
        ClockTimeWrapper m_clockTime;
        utc_tp m_time;
        unsigned int m_year = 1900, m_month = 1, m_monthDay = 1;

        static std::time_t custom_timegm(const std::tm& t);
        static std::time_t time_to_utc_time_t(const DateInput& date, const TimeInput& time) ;
        // Constructs a disgusting tm struct.
        // NOTE: All the inputs are assumed to be 1-based.
        tm getTm(const DateInput& date, const TimeInput& time) const;
        void setTime(const DateInput& date, const TimeInput& time);
        local_time<seconds> getLocalTime() const;
    public:
        #ifdef PERFORM_UNIT_TESTS
        static void setCurrentTimeOverride(TimeWrapper time);
        #endif
        // Create an empty TimeWrapper.
        TimeWrapper();
        // Construct TimeWrapper from time_point (should be UTC)
        TimeWrapper(const time_point<system_clock>& tp);
        // Construct TimeWrapper from date
        TimeWrapper(const DateInput& date);
        // Construct ClockTimeWrapper from copy
        TimeWrapper(const ClockTimeWrapper& clockTime);
        // Construct TimeWrapper from date and time
        TimeWrapper(const DateInput& date, const TimeInput& time);
        // Construct ClockTimeWrapper from time and TimeWrapper from date 
        // TimeWrapper(const TimeInput& time, const DateInput& date);
        // TEMP
        TimeWrapper(unsigned int year, unsigned int month = 1, unsigned int monthDay = 1);

        ClockTimeWrapper& getClockTime();
        // Returns a string in the format: [DATE] (FULL WORD WEEKDAY + DATE) [CLOCKTIME] or an empty string if the TimeWrapper is empty.
        std::string getString(TIME_FORMAT = TIME_FORMAT_FULL) const;
        bool getIsEmpty() const;
        void clear();  
        // ONE_BASED: Get the month day (1..dayCount).
        // ZERO_BASED: (0..dayCount - 1).
        unsigned int getMonthDay(bool basedness = ONE_BASED) const;
        // ONE_BASED: Set the month day (1..dayCount).
        // ZERO_BASED: (0.. dayCount - 1).
        // NOTE: If the day provided is more than the days in the current month, the day will be set to it
        void setMonthDay(unsigned int day, bool basedness = ONE_BASED);
        // Add 1 to the month day. Handles incrementing months as well.
        void incrementMonthDay();
        // Subtract 1 from the month day. Handles decrementing months as well.
        void decrementMonthDay();

        // WEEK_START_MONDAY: Get the weekday from MON to SUN.
        // WEEK_START_SUNDAY: Get the weekday from SUN to SAT.
        // ONE_BASED: (1..7).
        // ZERO_BASED: (0..6).
        unsigned int getWeekDay(bool weekStart = WEEK_START_MONDAY, bool basedness = ONE_BASED) const;

        // ONE_BASED: Get the month (1..12).
        // ZERO_BASED: (0..11).
        unsigned int getMonth(bool basedness = ONE_BASED) const;   
        // ONE_BASED: If the given month is < 1, it will be passed as 12. If it's > 12, it will passed as 1.
        // ZERO_BASED: If the given month is < 0, it will be set to 11. If it's > 11, it will be set to 0.
        void setMonth(int month, bool basedness = ONE_BASED);
        // Add 1 to the month. Handles incrementing year as well.
        void incrementMonth();
        // Subtract 1 from the month. Handles decrementing year as well.
        void decrementMonth();

        // Get the year
        unsigned int getYear() const;
        // Set the year, it will be limited to the range (1900..8000)
        void setYear(unsigned int year);
        void incrementYear();
        void decrementYear();

        template <typename Duration>
        static Duration::rep getDifference(const TimeWrapper& base, const TimeWrapper& subtracted)
        {
            static_assert(is_duration<Duration>::value, "TimeWrapper::getDifference() only works with std::chrono::duration types!");
            return (floor<Duration>(base.m_time) - floor<Duration>(subtracted.m_time)).count();
        }

        // Return the year as years since 1900 (0...)
        static unsigned int toYearsSinceTm(unsigned int year);
        // Convert from the tm struct's years since 1900 to a normal year (i.e. 104 -> 1900 + 104 = 2004)
        static unsigned int fromYearsSinceTm(unsigned int yearsSinceTm);
        // Limit the given year to a range of (1900..8000)
        static unsigned int toValidYear(unsigned int year);

        bool operator==(const TimeWrapper& other) const
        {
            return m_time == other.m_time;
        }

        bool operator!=(const TimeWrapper& other) const
        {
            return m_time != other.m_time;
        }

        friend bool operator<(const TimeWrapper& left, const TimeWrapper& right)
        {
            return left.m_time < right.m_time;
        }

        friend bool operator>(const TimeWrapper& left, const TimeWrapper& right)
        {
            return left.m_time > right.m_time;
        }

        // Gets the current system date and time
        static TimeWrapper getCurrentSystemTime();
};