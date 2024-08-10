#include <catch2/catch_test_macros.hpp>
#include <catch2/reporters/catch_reporter_event_listener.hpp>
#include <catch2/reporters/catch_reporter_registrars.hpp>

#include "date_container.h"

TEST_CASE("DateContainer")
{
    DateContainer dateContainer;
    // TODO: Basic tests 

    /* Test functions:
        getString()
        getDayDifference()
        operator++
        operator--
    */

    SECTION("Increment operators")
    {
        dateContainer.getTime().setUtcYear(2000);
        dateContainer.getTime().setUtcMonth(4);
        dateContainer.getTime().setUtcMonthDay(29);
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