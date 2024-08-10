#include <catch2/catch_test_macros.hpp>
#include <catch2/reporters/catch_reporter_event_listener.hpp>
#include <catch2/reporters/catch_reporter_registrars.hpp>

#include "time_wrapper.h"

inline const char* TEST_TZ = "Europe/Tallinn";

using namespace std::chrono;

inline bool check_equal(const ClockTimeWrapper& left, const ClockTimeWrapper& right)
{
    CHECK(left.getHours() == right.getHours());
    CHECK(left.getMinutes() == right.getMinutes());
    return (left.getHours() == right.getHours() && left.getMinutes() == right.getMinutes());
}

inline bool check_equal(const DateWrapper& left, const DateWrapper& right)
{
    CHECK(left.getYear() == right.getYear());
    CHECK(left.getMonth() == right.getMonth());
    CHECK(left.getMonthDay() == right.getMonthDay());
    return (left.getYear() == right.getYear() && left.getMonth() == right.getMonth() && left.getMonthDay() == right.getMonthDay());
}

TEST_CASE("TimeWrapper")
{
    SECTION("TimeWrapper constructors")
    {
        TimeWrapper empty = TimeWrapper();
        CHECK(empty.getIsEmpty() == true);

        auto sysNow = system_clock::now();
        TimeWrapper fromTimePoint = TimeWrapper(sysNow);
        // Dunno how to test this but it's hard for it NOT to work
        CHECK(fromTimePoint.getUtcTime() == sysNow);

        DateWrapper date1 = DateWrapper(1995, 8, 10);
        TimeWrapper fromDate = TimeWrapper(date1);
        auto utcDate1 = fromDate.getUtcDate();
        CHECK(check_equal(utcDate1, date1));

        ClockTimeWrapper clock1 = ClockTimeWrapper(35, -5);
        TimeWrapper onlyClock = TimeWrapper(clock1);
        auto utcClock1 = onlyClock.getUtcClockTime();
        CHECK(check_equal(utcClock1, clock1));

        DateWrapper date2 = DateWrapper(2007, 7, 13);
        ClockTimeWrapper clock2 = ClockTimeWrapper(12, 58);
        TimeWrapper bothParts = TimeWrapper(date2, clock2);
        auto utcDate2 = bothParts.getUtcDate();
        auto utcClock2 = bothParts.getUtcClockTime();
        CHECK(check_equal(utcDate2, date2));   
        CHECK(check_equal(utcClock2, clock2)); 
    }

    TimeWrapper timeWrapper;

    SECTION("Date setters and getters")
    {
        timeWrapper.setUtcYear(1999);
        CHECK(timeWrapper.getUtcYear() == 1999);

        SECTION("setUtcMonth(), getUtcMonth()")
        {
            timeWrapper.setUtcMonth(7);
            CHECK(timeWrapper.getUtcMonth() == 7);
            timeWrapper.setUtcMonth(13);
            CHECK(timeWrapper.getUtcMonth() == 1);
            timeWrapper.setUtcMonth(-5);
            CHECK(timeWrapper.getUtcMonth() == 12);
        }

        SECTION("Limiting to month day count")
        {
            // clamping to month day count
            timeWrapper.setUtcMonth(2);
            timeWrapper.setUtcMonthDay(35);
            CHECK(timeWrapper.getUtcMonthDay() == 28);
        }

        SECTION("Leap years and monthDay affected when changed")
        {
            timeWrapper.setUtcMonth(3);
            timeWrapper.setUtcMonthDay(31);
            // set year to leap year
            timeWrapper.setUtcYear(2000);
            // setting month moves to next month by leftover days
            timeWrapper.setUtcMonth(2);
            // february has 29 days in leap year
            // 31 - 29 = 2. day in march
            CHECK(timeWrapper.getUtcMonth() == 3);
            CHECK(timeWrapper.getUtcMonthDay() == 2);
        }
    }

    SECTION("getUtcWeekday()")
    {
        timeWrapper = TimeWrapper(DateWrapper(2024, 8, 6)); // Tuesday
        CHECK(timeWrapper.getUtcWeekDay(WEEK_START_MONDAY) == 2);
        CHECK(timeWrapper.getUtcWeekDay(WEEK_START_SUNDAY) == 3);
        CHECK(timeWrapper.getUtcWeekDay(WEEK_START_MONDAY, ZERO_BASED) == 1);
        CHECK(timeWrapper.getUtcWeekDay(WEEK_START_SUNDAY, ZERO_BASED) == 2);
        timeWrapper.setUtcMonthDay(25); // Sunday
        CHECK(timeWrapper.getUtcWeekDay(WEEK_START_MONDAY) == 7);
        CHECK(timeWrapper.getUtcWeekDay(WEEK_START_SUNDAY) == 1);
        CHECK(timeWrapper.getUtcWeekDay(WEEK_START_MONDAY, ZERO_BASED) == 6);
        CHECK(timeWrapper.getUtcWeekDay(WEEK_START_SUNDAY, ZERO_BASED) == 0);
    }

    SECTION("getDifference()")
    {
        TimeWrapper t2024_8_6_15h43m = TimeWrapper({2024, 8, 6}, {15, 43});
        CHECK(TimeWrapper::getDifference<years>( // Base time is earlier than subtracted
            TimeWrapper({1984, 8, 9}, {0, 0}), 
            t2024_8_6_15h43m) 
            == -40);
        CHECK(TimeWrapper::getDifference<months>( // Months between 1 year's date and a following year's date
            t2024_8_6_15h43m, 
            TimeWrapper({2025, 2, 26}, {0, 0}))
            == -6);
        CHECK(TimeWrapper::getDifference<days>(
            TimeWrapper({2024, 8, 29}, {0, 0}),
            t2024_8_6_15h43m)
            == 23);
        CHECK(TimeWrapper::getDifference<hours>(
            t2024_8_6_15h43m,  
            TimeWrapper({2024, 8, 5}, {22, 15})) 
            == 17);
    }

    // I don't know how to test local time without there being a set timezone..?
    SECTION("Time zones, local time")
    {
        timeWrapper = TimeWrapper(DateWrapper(2024, 8, 10), ClockTimeWrapper(15, 35));

        // Get time in the test timezone.
        auto localTime = timeWrapper.getLocalTime(TEST_TZ);
        // Check that the offset from UTC is correct.
        CHECK(TimeWrapper::getTimeComponents(localTime).second.getHours() == timeWrapper.getUtcClockTime().getHours() + 3);
    }
}