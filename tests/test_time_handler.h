#include <catch2/catch_test_macros.hpp>
#include <catch2/reporters/catch_reporter_event_listener.hpp>
#include <catch2/reporters/catch_reporter_registrars.hpp>

#include "time_wrapper.h"
#include "time_handler.h"

TEST_CASE("TimeHandler")
{
    IO_Handler ioHandler;
    Schedule schedule;
    // set up schedule
    schedule.addColumn(schedule.getColumnCount(), Column({}, SCH_BOOL, "DailyReset"), false);
    schedule.setColumnResetOption(schedule.getColumnCount() - 1, ColumnResetOption::Daily, false);
    schedule.addColumn(schedule.getColumnCount(), Column({}, SCH_BOOL, "WeeklyReset"), false);
    schedule.setColumnResetOption(schedule.getColumnCount() - 1, ColumnResetOption::Weekly, false);
    schedule.addColumn(schedule.getColumnCount(), Column({}, SCH_BOOL, "MonthlyReset"), false);
    schedule.setColumnResetOption(schedule.getColumnCount() - 1, ColumnResetOption::Monthly, false);
    schedule.addRow(0, false);

    for (size_t i = 0; i < schedule.getColumnCount(); i++)
    {
        schedule.setElementValue(i, 0, true);
    }

    TimeHandler timeHandler;
    timeHandler.init(ioHandler, schedule);

    TimeWrapper::testOverrideCurrentTime.clear();

    SECTION("Same date - no resets")
    {
        TimeWrapper::testOverrideCurrentTime = TimeWrapper(2012, 6, 15);
        timeHandler.applyResetsSinceEditTime(TimeWrapper::testOverrideCurrentTime);
        timeHandler.timeTick();
        CHECK(schedule.getElementValue<bool>(0, 0) == true);
        CHECK(schedule.getElementValue<bool>(1, 0) == true);
        CHECK(schedule.getElementValue<bool>(2, 0) == true);
    }

    SECTION("Different day - daily resets")
    {
        TimeWrapper editTime = TimeWrapper(2016, 7, 4);
        TimeWrapper::testOverrideCurrentTime = editTime;
        TimeWrapper::testOverrideCurrentTime.incrementMonthDay();
        timeHandler.applyResetsSinceEditTime(editTime);
        timeHandler.timeTick();
        CHECK(schedule.getElementValue<bool>(0, 0) == false);
        CHECK(schedule.getElementValue<bool>(1, 0) == true);
        CHECK(schedule.getElementValue<bool>(2, 0) == true);
    }

    SECTION("Different week - weekly resets")
    {
        TimeWrapper editTime = TimeWrapper::getCurrentSystemTime();
        TimeWrapper::testOverrideCurrentTime = editTime;
        for (size_t i = 0; i < 8 - editTime.getWeekDay(); i++)
        {
            TimeWrapper::testOverrideCurrentTime.incrementMonthDay();
        }
        timeHandler.applyResetsSinceEditTime(editTime);
        timeHandler.timeTick();
        CHECK(schedule.getElementValue<bool>(0, 0) == false);
        CHECK(schedule.getElementValue<bool>(1, 0) == false);
        CHECK(schedule.getElementValue<bool>(2, 0) == true);
    }

    SECTION("Different month - monthly resets")
    {
        TimeWrapper editTime = TimeWrapper(2024, 8, 7);
        TimeWrapper::testOverrideCurrentTime = editTime;
        TimeWrapper::testOverrideCurrentTime.incrementMonth();
        timeHandler.applyResetsSinceEditTime(editTime);
        timeHandler.timeTick();
        CHECK(schedule.getElementValue<bool>(0, 0) == false);
        CHECK(schedule.getElementValue<bool>(1, 0) == false);
        CHECK(schedule.getElementValue<bool>(2, 0) == false);
    }

    SECTION("Error case: Previous time's month day is more recent than current - nothing happens")
    {
        TimeWrapper editTime = TimeWrapper::getCurrentSystemTime();
        editTime.incrementMonthDay();
        timeHandler.applyResetsSinceEditTime(editTime);
        timeHandler.timeTick();
        CHECK(schedule.getElementValue<bool>(0, 0) == true);
        CHECK(schedule.getElementValue<bool>(1, 0) == true);
        CHECK(schedule.getElementValue<bool>(2, 0) == true);
    }

    SECTION("Error case: Previous time's month is more recent than current - nothing happens")
    {
        TimeWrapper editTime = TimeWrapper::getCurrentSystemTime();
        editTime.incrementMonth();
        timeHandler.applyResetsSinceEditTime(editTime);
        timeHandler.timeTick();
        CHECK(schedule.getElementValue<bool>(0, 0) == true);
        CHECK(schedule.getElementValue<bool>(1, 0) == true);
        CHECK(schedule.getElementValue<bool>(2, 0) == true);
    }
}