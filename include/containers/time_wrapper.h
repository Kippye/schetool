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

class DateWrapper
{
    private:
        unsigned int m_year;
        unsigned int m_month = 1;
        unsigned int m_monthDay = 1;
    public:
        DateWrapper();
        DateWrapper(unsigned int y, unsigned int m = 1, unsigned int mDay =  1);
        DateWrapper(const year_month_day& date);

        unsigned int getYear() const;
        unsigned int getMonth() const;
        unsigned int getMonthDay() const;
};

class ClockTimeWrapper
{
    private:
        unsigned int m_hours = 0, m_minutes = 0;
    public:
        ClockTimeWrapper();
        ClockTimeWrapper(unsigned int hours, unsigned int minutes);
        ClockTimeWrapper(const hh_mm_ss<seconds>& hms);

        unsigned int getHours() const;
        unsigned int getMinutes() const;
};

class TimeWrapper
{
    private:
        bool m_empty = true;
        utc_tp m_time; 
        
        static std::time_t custom_timegm(const std::tm& t);
        static std::time_t time_to_utc_time_t(const DateWrapper& date, const ClockTimeWrapper& time) ;
        // Constructs a disgusting tm struct.
        // NOTE: All the inputs are assumed to be 1-based.
        tm getTm(const DateWrapper& date, const ClockTimeWrapper& time) const;
        void setTime(const DateWrapper& date, const ClockTimeWrapper& time);
    public:
        #ifdef PERFORM_UNIT_TESTS
        static void setCurrentTimeOverride(TimeWrapper time);
        #endif
        // Create an empty TimeWrapper.
        TimeWrapper();
        // Construct TimeWrapper from time_point (should be UTC)
        TimeWrapper(const time_point<system_clock>& tp);
        // Construct TimeWrapper from date (Clock time is 00:00)
        TimeWrapper(const DateWrapper& date);
        // Construct TimeWrapper that only contains a clock time (?)
        TimeWrapper(const ClockTimeWrapper& clockTime);
        // Construct TimeWrapper from date and time
        TimeWrapper(const DateWrapper& date, const ClockTimeWrapper& time);
        // TEMP
        TimeWrapper(unsigned int year, unsigned int month = 1, unsigned int monthDay = 1);

        time_point<system_clock> getUtcTime() const;
        local_time<seconds> getLocalTime(const std::string& timezoneName = "") const;
        ClockTimeWrapper getClockTime() const;
        DateWrapper getDate() const;
        // Set the date while keeping the clock time the same
        void setDate(const DateWrapper& date);

        // Returns a string in one of the pre-existing formats: Date only, Time only or full. 
        // Returns an empty string if the TimeWrapper is empty.
        std::string getString(TIME_FORMAT = TIME_FORMAT_FULL, const std::optional<local_time<seconds>> time = std::nullopt) const;
        // Format the time using the provided format string.
        std::string getDynamicFmtString(const std::string_view& fmt, const std::optional<local_time<seconds>> time = std::nullopt) const;
        bool getIsEmpty() const;
        void clear();  
        // ONE_BASED: Get the month day (1..dayCount).
        // ZERO_BASED: (0..dayCount - 1).
        unsigned int getMonthDay(bool basedness = ONE_BASED) const;
        // ONE_BASED: Set the month day (1..dayCount).
        // ZERO_BASED: (0.. dayCount - 1).
        // NOTE: If the day provided is more than the days in the current month, the day will be set to it
        void setMonthDay(unsigned int day, bool basedness = ONE_BASED);
        // Add this number of days to the time.
        // Can be a negative number to subtract days.
        void addDays(int days);

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
        // Add this number of months to the time.
        // Can be a negative number to subtract.
        void addMonths(int months);

        // Get the year
        unsigned int getYear() const;
        // Set the year
        void setYear(unsigned int year);

        template <typename Duration>
        static Duration::rep getDifference(const TimeWrapper& base, const TimeWrapper& subtracted)
        {
            static_assert(is_duration<Duration>::value, "TimeWrapper::getDifference() only works with std::chrono::duration types!");
            return (floor<Duration>(base.m_time) - floor<Duration>(subtracted.m_time)).count();
        }

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
        static TimeWrapper getCurrentTime();
        // Gets the amount of minutes the current timezone is offset from UTC time (including DST).
        // Example: The timezone is UTC + 3, returns 180min. The timezone is UTC - 2, returns -120min.
        static minutes getTimezoneOffset();
        static int limitYearToValidRange(int year);
};