#include <catch2/catch_test_macros.hpp>
#include <catch2/reporters/catch_reporter_event_listener.hpp>
#include <catch2/reporters/catch_reporter_registrars.hpp>

#include "test_filter_editor_subgui.h"
#include "schedule_gui.h"

TEST_CASE("ScheduleGui", "[gui]")
{
    ScheduleCore scheduleCore = ScheduleCore();
    ScheduleEvents scheduleEvents = ScheduleEvents();
    ScheduleGui scheduleGui = ScheduleGui("ScheduleGui", scheduleCore, scheduleEvents);

    SECTION("has subguis")
    {
        CHECK(scheduleGui.getSubGui<ElementEditorSubGui>("ElementEditorSubGui"));
        CHECK(scheduleGui.getSubGui<FilterEditorSubGui>("FilterEditorSubGui"));
    }
}