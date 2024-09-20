#ifndef SCHEDULE_CONSTANTS
#define SCHEDULE_CONSTANTS
#include <map>
#include <vector>
#include <string>
#include "general_constants.h"

enum class ColumnResetOption
{
    Never,
    Daily,
    Weekly,
    Monthly
};

namespace schedule_consts
{
    // A "soft limit" for a sensible amount of columns that don't look too terrible.
    // It's not enforced anywhere other than the add column button.
    const size_t COLUMN_MAX_COUNT = 25;

    const size_t ELEMENT_TEXT_MAX_LENGTH = 1024;
    const size_t SELECT_OPTION_NAME_MAX_LENGTH = 20;
    const std::map<SCHEDULE_TYPE, const char*> scheduleTypeNames =
    {
        {SCH_BOOL, "Checkbox"},
        {SCH_NUMBER, "Number"},
        {SCH_DECIMAL, "Decimal"},
        {SCH_TEXT, "Text"},
        {SCH_SELECT, "Select"},
        {SCH_WEEKDAY, "Weekday"},
        {SCH_TIME, "Time"},
        {SCH_DATE, "Date"},
    };
    const std::map<ColumnResetOption, const char*> columnResetOptionStrings =
    {
        { ColumnResetOption::Never, "Never" },
        { ColumnResetOption::Daily, "Daily" },
        { ColumnResetOption::Weekly, "Weekly" },
        { ColumnResetOption::Monthly, "Monthly" }
    };
}
#endif