#pragma once

#include <string>
#include <chrono>

namespace chrono = std::chrono;
typedef chrono::system_clock::time_point utc_tp;

template<class T>
struct is_duration : std::false_type{};
template<class Rep, class Period>
struct is_duration<chrono::duration<Rep, Period>> : std::true_type{};

template<class T>
struct is_time_point : std::false_type{};
template<class Clock, class Duration>
struct is_time_point<chrono::time_point<Clock, Duration>> : std::true_type{};

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
        unsigned int m_year = 1970;
        unsigned int m_month = 1;
        unsigned int m_monthDay = 1;
    public:
        DateWrapper();
        DateWrapper(unsigned int y, unsigned int m = 1, unsigned int mDay =  1);
        DateWrapper(const chrono::year_month_day& date);

        unsigned int getYear() const;
        unsigned int getMonth() const;
        unsigned int getMonthDay() const;

        bool operator==(const DateWrapper& other) const
        {
            return getYear() == other.getYear()
                && getMonth() == other.getMonth()
                && getMonthDay() == other.getMonthDay();
        }

        bool operator!=(const DateWrapper& other) const
        {
            return !(*this == other);
        }

        friend bool operator<(const DateWrapper& left, const DateWrapper& right)
        {
            return left.getYear() < right.getYear() || (left.getYear() == right.getYear() 
            && left.getMonth() < right.getMonth() || (left.getMonth() == right.getMonth()
            && left.getMonthDay() < right.getMonthDay()));
        }

        friend bool operator>(const DateWrapper& left, const DateWrapper& right)
        {
            return left.getYear() > right.getYear() || (left.getYear() == right.getYear() 
            && left.getMonth() > right.getMonth() || (left.getMonth() == right.getMonth()
            && left.getMonthDay() > right.getMonthDay()));
        }
};

class ClockTimeWrapper
{
    private:
        unsigned int m_hours = 0, m_minutes = 0;
    public:
        ClockTimeWrapper();
        ClockTimeWrapper(unsigned int hours, unsigned int minutes);
        ClockTimeWrapper(const chrono::hh_mm_ss<chrono::seconds>& hms);

        unsigned int getHours() const;
        unsigned int getMinutes() const;

        bool operator==(const ClockTimeWrapper& other) const
        {
            return getHours() == other.getHours()
                && getMinutes() == other.getMinutes();
        }

        bool operator!=(const ClockTimeWrapper& other) const
        {
            return !(*this == other);
        }

        friend bool operator<(const ClockTimeWrapper& left, const ClockTimeWrapper& right)
        {
            return left.getHours() < right.getHours() || (left.getHours () == right.getHours() 
            && left.getMinutes() < right.getMinutes());
        }

        friend bool operator>(const ClockTimeWrapper& left, const ClockTimeWrapper& right)
        {
            return left.getHours() > right.getHours() || (left.getHours () == right.getHours() 
            && left.getMinutes() > right.getMinutes());
        }
};

class TimeWrapper
{
    private:
        bool m_empty = true;
        utc_tp m_time; 
        
        static std::time_t time_to_utc_time_t(const DateWrapper& date, const ClockTimeWrapper& time) ;
        template <typename Timepoint>
        static DateWrapper getDate(const Timepoint& tp)
        {
            static_assert(is_time_point<Timepoint>::value, "TimeWrapper::getDate() only works with std::chrono::time_point types!");
            return getTimeComponents(tp).first;
        }
        template <typename Timepoint>
        static ClockTimeWrapper getClockTime(const Timepoint& tp)
        {
            static_assert(is_time_point<Timepoint>::value, "TimeWrapper::getClockTime() only works with std::chrono::time_point types!");
            return getTimeComponents(tp).second;
        }
        template <typename Timepoint>
        static unsigned int getMonthDay(const Timepoint& tp, bool basedness = ONE_BASED)
        {
            static_assert(is_time_point<Timepoint>::value, "TimeWrapper::getMonthDay() only works with std::chrono::time_point types!");
            return basedness == ONE_BASED ? getDate(tp).getMonthDay() : getDate(tp).getMonthDay() - 1;
        }
        template <typename Timepoint>
        static unsigned int getWeekday(const Timepoint& tp, bool weekStart = WEEK_START_MONDAY, bool basedness = ONE_BASED)
        {
            static_assert(is_time_point<Timepoint>::value, "TimeWrapper::getWeekday() only works with std::chrono::time_point types!");
            auto zonedDays = chrono::floor<chrono::days>(tp);
            chrono::year_month_weekday ymw = chrono::year_month_weekday(zonedDays);
            unsigned int weekday = weekStart == WEEK_START_MONDAY ? ymw.weekday().iso_encoding() - 1 // The ISO encoding means Monday = 1,  Sunday = 7
                : ymw.weekday().c_encoding(); // The C encoding means Sunday = 0, Saturday = 6

            return basedness + weekday;
        }
        template <typename Timepoint>
        static unsigned int getMonth(const Timepoint& tp, bool basedness = ONE_BASED)
        {
            static_assert(is_time_point<Timepoint>::value, "TimeWrapper::getMonth() only works with std::chrono::time_point types!");
            return basedness == ONE_BASED ? getDate(tp).getMonth() : getDate(tp).getMonth() - 1;
        }
    public:
        #ifdef PERFORM_UNIT_TESTS
        static TimeWrapper testCurrentTimeOverride;
        #endif
        // Create an empty TimeWrapper.
        TimeWrapper();
        // Construct TimeWrapper from time_point (should be UTC)
        TimeWrapper(const chrono::time_point<chrono::system_clock>& tp);
        // Construct TimeWrapper from date (Clock time is 00:00)
        TimeWrapper(const DateWrapper& date);
        // Construct TimeWrapper that only contains a clock time (?)
        TimeWrapper(const ClockTimeWrapper& clockTime);
        // Construct TimeWrapper from date and time
        TimeWrapper(const DateWrapper& date, const ClockTimeWrapper& time);
        // TEMP
        TimeWrapper(unsigned int year, unsigned int month = 1, unsigned int monthDay = 1);

        bool getIsEmpty() const;
        void clear();  

        chrono::time_point<chrono::system_clock> getTimeUTC() const;
        chrono::local_time<chrono::seconds> getLocalTime(const std::string& timezoneName = "") const;

        DateWrapper getDateUTC() const;
        DateWrapper getLocalDate() const;
        ClockTimeWrapper getClockTimeUTC() const;
        ClockTimeWrapper getLocalClockTime() const;

        // Set the UTC date and clock time
        void setTimeUTC(const DateWrapper& date, const ClockTimeWrapper& time);
        // Set the UTC date while keeping the clock time the same
        void setDateUTC(const DateWrapper& date);

        // ONE_BASED: Get the UTC month day (1..dayCount).
        // ZERO_BASED: (0..dayCount - 1).
        unsigned int getMonthDayUTC(bool basedness = ONE_BASED) const;
        // ONE_BASED: Get the month day (1..dayCount).
        // ZERO_BASED: (0..dayCount - 1).
        unsigned int getMonthDay(bool basedness = ONE_BASED) const;
        // ONE_BASED: Set the month day (1..dayCount).
        // ZERO_BASED: (0.. dayCount - 1).
        // NOTE: If the day provided is more than the days in the current month, the day will be set to it
        void setMonthDayUTC(unsigned int day, bool basedness = ONE_BASED);
        // Add this number of days to the time.
        // Can be a negative number to subtract days.
        void addDays(int days);

        // WEEK_START_MONDAY: Get the UTC weekday from MON to SUN.
        // WEEK_START_SUNDAY: Get the UTC weekday from SUN to SAT.
        // ONE_BASED: (1..7).
        // ZERO_BASED: (0..6).
        unsigned int getWeekdayUTC(bool weekStart = WEEK_START_MONDAY, bool basedness = ONE_BASED) const;
        // WEEK_START_MONDAY: Get the weekday from MON to SUN.
        // WEEK_START_SUNDAY: Get the weekday from SUN to SAT.
        // ONE_BASED: (1..7).
        // ZERO_BASED: (0..6).
        unsigned int getWeekday(bool weekStart = WEEK_START_MONDAY, bool basedness = ONE_BASED) const;

        // ONE_BASED: Get the UTC month (1..12).
        // ZERO_BASED: (0..11).
        unsigned int getMonthUTC(bool basedness = ONE_BASED) const; 
        // ONE_BASED: Get the month (1..12).
        // ZERO_BASED: (0..11).
        unsigned int getMonth(bool basedness = ONE_BASED) const;   
        // ONE_BASED: If the given month is < 1, it will be passed as 12. If it's > 12, it will passed as 1.
        // ZERO_BASED: If the given month is < 0, it will be set to 11. If it's > 11, it will be set to 0.
        void setMonthUTC(int month, bool basedness = ONE_BASED);
        // Add this number of months to the time.
        // Can be a negative number to subtract.
        void addMonths(int months);

        // Get the UTC year
        unsigned int getYearUTC() const;
        // Get the year
        unsigned int getYear() const;
        // Set the year
        void setYearUTC(unsigned int year);

        // Returns a string with this TimeWrapper's UTC time using one of the pre-existing formats: Date only, Time only or full. 
        // Returns an empty string if the TimeWrapper is empty.
        std::string getStringUTC(TIME_FORMAT = TIME_FORMAT_FULL) const;
        // Returns a string with this TimeWrapper's local time in the current time zone using one of the pre-existing formats: Date only, Time only or full. 
        // Returns an empty string if the TimeWrapper is empty.
        std::string getString(TIME_FORMAT = TIME_FORMAT_FULL) const;
        // Format this TimeWrapper's UTC time using the provided format string.
        // Returns an empty string if the TimeWrapper is empty.
        std::string getDynamicFmtStringUTC(const std::string_view& fmt) const;
        // Format this TimeWrapper's local time in the current time zone using the provided format string.
        // Returns an empty string if the TimeWrapper is empty.
        std::string getDynamicFmtString(const std::string_view& fmt) const;

        template <typename Duration>
        static Duration::rep getDifferenceUTC(const TimeWrapper& base, const TimeWrapper& subtracted)
        {
            static_assert(is_duration<Duration>::value, "TimeWrapper::getDifferenceUTC() only works with std::chrono::duration types!");
            return (chrono::floor<Duration>(base.m_time) - chrono::floor<Duration>(subtracted.m_time)).count();
        }
        template <typename Duration>
        static Duration::rep getDifference(const TimeWrapper& base, const TimeWrapper& subtracted)
        {
            static_assert(is_duration<Duration>::value, "TimeWrapper::getDifference() only works with std::chrono::duration types!");
            return (chrono::floor<Duration>(base.getLocalTime()) - chrono::floor<Duration>(subtracted.getLocalTime())).count();
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

        friend bool operator<=(const TimeWrapper& left, const TimeWrapper& right)
        {
            return left.m_time < right.m_time || left.m_time == right.m_time;
        }

        friend bool operator>(const TimeWrapper& left, const TimeWrapper& right)
        {
            return left.m_time > right.m_time;
        }

        friend bool operator>=(const TimeWrapper& left, const TimeWrapper& right)
        {
            return left.m_time > right.m_time || left.m_time == right.m_time;
        }

        template <typename Timepoint>
        static std::pair<DateWrapper, ClockTimeWrapper> getTimeComponents(const Timepoint& time)
        {
            static_assert(is_time_point<Timepoint>::value, "TimeWrapper::getTimeComponents() only works with std::chrono::time_point types!");
            auto timeDays = chrono::floor<chrono::days>(time);
            return { DateWrapper(chrono::year_month_day(timeDays)), ClockTimeWrapper(chrono::hh_mm_ss(chrono::floor<chrono::seconds>(time - timeDays))) };
        }

        // Turn a time to a string using the format type.
        // Returns a string in one of the pre-existing formats: Date only, Time only or full. 
        template <typename Timepoint>
        static std::string getString(const Timepoint& time, TIME_FORMAT fmtType = TIME_FORMAT_FULL)
        {
            static_assert(is_time_point<Timepoint>::value, "TimeWrapper::getString() only works with std::chrono::time_point types!");
            switch(fmtType) 
            {
                case(TIME_FORMAT_DATE):
                    return std::format("{:%d/%m/%Y}", time);
                case(TIME_FORMAT_TIME):
                    return std::format("{:%R}", time);
                case(TIME_FORMAT_FULL):
                default:
                    return std::format("{:%d/%m/%Y %R}", time);
            }
        }

        // Format the time using the provided format string.
        template <typename Timepoint>
        static std::string getDynamicFmtString(const Timepoint& time, const std::string_view& fmt)
        {
            static_assert(is_time_point<Timepoint>::value, "TimeWrapper::getDynamicFmtString() only works with std::chrono::time_point types!");
            return std::vformat(fmt, std::make_format_args(time));
        }
        // Gets the current system date and time
        static TimeWrapper getCurrentTime();
        // Creates a new TimeWrapper with the timezone offset subtracted.
        // Example: 
        // Input TimeWrapper: 15:30.
        // Time zone: UTC + 2.
        // Output TimeWrapper: 13:30.
        static TimeWrapper getTimeWithOffsetSubtracted(const TimeWrapper& base);
        // Gets the amount of minutes the current timezone is offset from UTC time (including DST).
        // Example: The timezone is UTC + 3, returns 180min. The timezone is UTC - 2, returns -120min.
        static chrono::minutes getTimezoneOffset();
        static int limitYearToValidRange(int year);
};