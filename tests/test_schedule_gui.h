#include <catch2/catch_test_macros.hpp>
#include <catch2/reporters/catch_reporter_event_listener.hpp>
#include <catch2/reporters/catch_reporter_registrars.hpp>

#include "schedule_gui.h"
#include "filter.h"

TEST_CASE("EditorFilterState")
{
    EditorFilterState editorFilterState;

    CHECK(editorFilterState.hasValidFilter() == false);
    editorFilterState.setType(SCH_NUMBER);
    CHECK(editorFilterState.getType() == SCH_NUMBER);
    editorFilterState.setFilter(std::make_shared<Filter<int>>(420));
    CHECK(editorFilterState.hasValidFilter() == true);
    CHECK(editorFilterState.getFilter<int>()->getPassValue() == 420);
}

TEST_CASE("ScheduleGui", "[gui]")
{
    ScheduleCore scheduleCore = ScheduleCore();
    ScheduleGui scheduleGui = ScheduleGui("ScheduleGui");
    scheduleGui.setScheduleCore(scheduleCore);

    SECTION("has subguis")
    {
        CHECK(scheduleGui.getSubGui<ElementEditorSubGui>("ElementEditorSubGui"));
        CHECK(scheduleGui.getSubGui<FilterEditorSubGui>("FilterEditorSubGui"));
    }
}