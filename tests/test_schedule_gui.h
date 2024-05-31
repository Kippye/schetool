#include <catch2/catch_test_macros.hpp>
#include <catch2/reporters/catch_reporter_event_listener.hpp>
#include <catch2/reporters/catch_reporter_registrars.hpp>

#include "schedule_gui.h"

TEST_CASE("ScheduleGui", "[gui]")
{
    ScheduleCore scheduleCore = ScheduleCore();
    ScheduleGui scheduleGui = ScheduleGui("ScheduleGui");
    scheduleGui.setScheduleCore(scheduleCore);

    SECTION("has ElementEditorSubGui")
    {
        CHECK(scheduleGui.getSubGui<ElementEditorSubGui>("ElementEditorSubGui"));
    }
}