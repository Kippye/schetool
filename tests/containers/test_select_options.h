#include <catch2/catch_test_macros.hpp>
#include <catch2/reporters/catch_reporter_event_listener.hpp>
#include <catch2/reporters/catch_reporter_registrars.hpp>

#include "select_options.h"

TEST_CASE("SelectOptionsModification") {
    SECTION("Equality comparison operators") {
        SECTION("Basic fields") {
            SelectOptionsModification modA = SelectOptionsModification(OPTION_MODIFICATION_RENAME).firstIndex(6).name("ABC");
            SelectOptionsModification modB = SelectOptionsModification(OPTION_MODIFICATION_RENAME).firstIndex(6).name("ABC");
            CHECK(modA == modB);
            modA.color(SelectColor_Blue);
            CHECK(modA != modB);
        }
        SECTION("Options") {
            // neither mod has options
            SelectOptionsModification modA = SelectOptionsModification(OPTION_MODIFICATION_ADD).firstIndex(6);
            SelectOptionsModification modB = SelectOptionsModification(OPTION_MODIFICATION_ADD).firstIndex(6);
            CHECK(modA == modB);
            modA.options({SelectOption{"OPT1", SelectColor_Gray}});
            // modA has options, modB doesn't
            CHECK(modA != modB);
            // both have the same option
            modB.options({SelectOption{"OPT1", SelectColor_Gray}});
            CHECK(modA == modB);
            // modB has a different option than modA
            modB.options({SelectOption{"OPT2", SelectColor_Pink}});
            CHECK(modA != modB);
        }
    }
}
TEST_CASE("SelectOptions") {
    // TODO ...
}