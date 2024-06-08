#include <catch2/catch_test_macros.hpp>
#include <catch2/reporters/catch_reporter_event_listener.hpp>
#include <catch2/reporters/catch_reporter_registrars.hpp>

#include "date_container.h"

TEST_CASE("DateContainer")
{
    // TODO: Basic tests 

    SECTION("Relative date with offset 0")
    {
        DateContainer dateContainer = DateContainer(tm(), true, 0);
        CHECK(dateContainer.getIsRelative() == true);
        CHECK(dateContainer.getRelativeOffset() == 0);
        CHECK(dateContainer.getTime().tm_year == DateContainer::getCurrentSystemDate().getTime().tm_year);
        CHECK(dateContainer.getTime().tm_mon == DateContainer::getCurrentSystemDate().getTime().tm_mon);
        CHECK(dateContainer.getTime().tm_mday == DateContainer::getCurrentSystemDate().getTime().tm_mday);
    }
    SECTION("Relative date with offset 1")
    {
        DateContainer dateContainer = DateContainer(tm(), true, 1);
        CHECK(dateContainer.getIsRelative() == true);
        CHECK(dateContainer.getRelativeOffset() == 1);
        DateContainer currentSystemDate = DateContainer::getCurrentSystemDate();
        currentSystemDate.incrementMonthDay();
        CHECK(dateContainer.getTime().tm_year == currentSystemDate.getTime().tm_year);
        CHECK(dateContainer.getTime().tm_mon == currentSystemDate.getTime().tm_mon);
        CHECK(dateContainer.getTime().tm_mday == currentSystemDate.getTime().tm_mday);
    }
    SECTION("Relative date with offset -1 (Not implemented, should default to 0 offset behavior)")
    {
        DateContainer dateContainer = DateContainer(tm(), true, -1);
        CHECK(dateContainer.getIsRelative() == true);
        CHECK(dateContainer.getRelativeOffset() == -1);
        CHECK(dateContainer.getTime().tm_year == DateContainer::getCurrentSystemDate().getTime().tm_year);
        CHECK(dateContainer.getTime().tm_mon == DateContainer::getCurrentSystemDate().getTime().tm_mon);
        CHECK(dateContainer.getTime().tm_mday == DateContainer::getCurrentSystemDate().getTime().tm_mday);
    }
}