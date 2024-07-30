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

    SECTION("TimeWrapper functions setMonthDay, setMonth, setYear")
    {
        dateContainer.getTime().setYear(1999);
        CHECK(dateContainer.getTime().getYear() == 1999);

        dateContainer.getTime().setMonth(7);
        CHECK(dateContainer.getTime().getMonth() == 7);
        dateContainer.getTime().setMonth(13);
        CHECK(dateContainer.getTime().getMonth() == 1);
        dateContainer.getTime().setMonth(-5);
        CHECK(dateContainer.getTime().getMonth() == 12);
        // clamping to month day count
        dateContainer.getTime().setMonth(2);
        dateContainer.getTime().setMonthDay(35);
        CHECK(dateContainer.getTime().getMonthDay() == 28);

        dateContainer.getTime().setMonth(3);
        dateContainer.getTime().setMonthDay(31);
        // set year to leap year
        dateContainer.getTime().setYear(2000);
        // setting month std::mins to new month day count
        dateContainer.getTime().setMonth(2);
        // february has 29 days in leap year
        CHECK(dateContainer.getTime().getMonthDay() == 29);
    }

    SECTION("Increment operators")
    {
        dateContainer.getTime().setYear(2000);
        dateContainer.getTime().setMonth(4);
        dateContainer.getTime().setMonthDay(29);
        dateContainer++;
        dateContainer++;
        // incremented to next month
        CHECK(dateContainer.getTime().getMonth() == 5);
        CHECK(dateContainer.getTime().getMonthDay() == 1);
        // decrement back to last month
        CHECK((--dateContainer).getTime().getMonthDay() == 30);
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