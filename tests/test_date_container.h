#include <catch2/catch_test_macros.hpp>
#include <catch2/reporters/catch_reporter_event_listener.hpp>
#include <catch2/reporters/catch_reporter_registrars.hpp>

#include "date_container.h"
#include <iostream>

TEST_CASE("DateContainer")
{
    DateContainer dateContainer;
    // TODO: Basic tests 

    /* Test functions:
        setYear()
        setMonth()
        setMonthDay()
        getString()
        getDayDifference()
        operator++
        operator--
    */

    SECTION("Functions setMonthDay, setMonth, setYear")
    {
        dateContainer.setYear(1999, true);
        CHECK(dateContainer.getYear(true) == 1999);

        dateContainer.setMonth(6);
        CHECK(dateContainer.getMonth() == 6);
        dateContainer.setMonth(12);
        CHECK(dateContainer.getMonth() == 0);
        dateContainer.setMonth(-5);
        CHECK(dateContainer.getMonth() == 11);
        // clamping to month day count
        dateContainer.setMonth(1);
        dateContainer.setMonthDay(35);
        CHECK(dateContainer.getMonthDay() == 28);

        dateContainer.setMonth(2);
        dateContainer.setMonthDay(31);
        // set year to leap year
        dateContainer.setYear(2000, true);
        // setting month std::mins to new month day count
        dateContainer.setMonth(1);
        // february has 29 days in leap year
        CHECK(dateContainer.getMonthDay() == 29);
    }

    SECTION("Increment operators")
    {
        dateContainer.setYear(2000, true);
        dateContainer.setMonth(3);
        dateContainer.setMonthDay(29);
        dateContainer++;
        dateContainer++;
        CHECK(dateContainer.getMonth() == 4);
        CHECK(dateContainer.getMonthDay() == 1);
        CHECK((--dateContainer).getMonthDay() == 30);
    }

    SECTION("Function getString() as relative")
    {
        dateContainer = DateContainer::getCurrentSystemDate();

        CHECK(dateContainer.getString(true) == "Today");

        dateContainer++;  
        CHECK(dateContainer.getString(true) == "Tomorrow");
         
        dateContainer++;
        CHECK(dateContainer.getString(true) == "in 2 days");
    }

    SECTION("Function getDayDifference()")
    {
        dateContainer = DateContainer::getCurrentSystemDate();
        DateContainer other = DateContainer::getCurrentSystemDate();

        SECTION("dateContainer < other")
        {
            // dateContainer > other
            int diff = 35;
            for (int i = 0; i < diff; i++)
            {
                other++;
            }
            CHECK(dateContainer.getDayDifference(other) == -diff);
        }
        SECTION("dateContainer > other")
        {
            // dateContainer < other
            int diff = 16;
            for (int i = 0; i < diff; i++)
            {
                other--;
            }
            CHECK(dateContainer.getDayDifference(other) == diff);
        }
    }
}