#include <catch2/catch_test_macros.hpp>
#include <catch2/reporters/catch_reporter_event_listener.hpp>
#include <catch2/reporters/catch_reporter_registrars.hpp>

#include "time_wrapper.h"

TEST_CASE("TimeWrapper")
{
    TimeWrapper timeWrapper;

    SECTION("setYear(), getYear()")
    {
        timeWrapper.setYear(1999);
        CHECK(timeWrapper.getYear() == 1999);

        SECTION("setMonth(), getMonth()")
        {
            timeWrapper.setMonth(7);
            CHECK(timeWrapper.getMonth() == 7);
            timeWrapper.setMonth(13);
            CHECK(timeWrapper.getMonth() == 1);
            timeWrapper.setMonth(-5);
            CHECK(timeWrapper.getMonth() == 12);
        }

        SECTION("Limiting to month day count")
        {
            // clamping to month day count
            timeWrapper.setMonth(2);
            timeWrapper.setMonthDay(35);
            CHECK(timeWrapper.getMonthDay() == 28);
        }

        SECTION("Leap years and monthDay limit on month change")
        {
            timeWrapper.setMonth(3);
            timeWrapper.setMonthDay(31);
            // set year to leap year
            timeWrapper.setYear(2000);
            // setting month std::mins to new month day count
            timeWrapper.setMonth(2);
            // february has 29 days in leap year
            CHECK(timeWrapper.getMonthDay() == 29);
        }
    }

    SECTION("TimeWrapper constructors")
    {
        TimeWrapper fromTm = TimeWrapper(tm{0, 15, 6, 3, 2, 124});
        CHECK(fromTm.getClockTime().getMinutes() == 15);
        CHECK(fromTm.getClockTime().getHours() == 6);
        CHECK(fromTm.getMonthDay() == 3);
        CHECK(fromTm.getMonth(ZERO_BASED) == 2);
        CHECK(fromTm.getYear() == 1900 + 124);

        TimeWrapper onlyClock = TimeWrapper(ClockTimeWrapper(35, -5));
        CHECK(onlyClock.getClockTime().getHours() == 24);
        CHECK(onlyClock.getClockTime().getMinutes() == 0);

        TimeWrapper fromValues = TimeWrapper(2024, 8, 6);
        CHECK(fromValues.getYear() == 2024);
        CHECK(fromValues.getMonth() == 8);
        CHECK(fromValues.getMonthDay() == 6);

        TimeWrapper fromClockAndValues = TimeWrapper(ClockTimeWrapper(12, 58), 1995, 7, 13);
        CHECK(fromClockAndValues.getClockTime().getHours() == 12);
        CHECK(fromClockAndValues.getClockTime().getMinutes() == 58);
        CHECK(fromClockAndValues.getYear() == 1995);
        CHECK(fromClockAndValues.getMonth() == 7);
        CHECK(fromClockAndValues.getMonthDay() == 13);
    }

    SECTION("getTm()")
    {
        timeWrapper = TimeWrapper(ClockTimeWrapper(13, 02), 2024, 8, 6);
        tm timeTm = timeWrapper.getTm();
        CHECK(timeTm.tm_hour == 13);
        CHECK(timeTm.tm_min == 2);
        CHECK(timeTm.tm_year == 2024 - 1900);
        CHECK(timeTm.tm_mon == 8 - 1);
        CHECK(timeTm.tm_mday == 6);
        CHECK(timeTm.tm_wday == 2);
        CHECK(timeTm.tm_yday == 219);
    }

    SECTION("getWeekday()")
    {
        timeWrapper = TimeWrapper(2024, 8, 6);
        CHECK(timeWrapper.getWeekDay() == 2);
        CHECK(timeWrapper.getWeekDay(ZERO_BASED) == 1);
        timeWrapper.setMonthDay(25);
        CHECK(timeWrapper.getWeekDay() == 7);
        CHECK(timeWrapper.getWeekDay(ZERO_BASED) == 6);
    }
}