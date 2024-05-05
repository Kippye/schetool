#ifndef SCHEDULE_CONSTANTS
#define SCHEDULE_CONSTANTS
#include <map>

enum SCHEDULE_TYPE
{
    SCH_BOOL,
    SCH_NUMBER,
    SCH_DECIMAL,
    SCH_TEXT,
    SCH_SELECT,
    SCH_TIME,
    SCH_DATE,
    SCH_WEEKDAY,
    SCH_LAST,
};

namespace schedule_consts
{
    const std::map<SCHEDULE_TYPE, const char*> scheduleTypeNames =
    {
        {SCH_BOOL, "Checkbox"},
        {SCH_NUMBER, "Number"},
        {SCH_DECIMAL, "Decimal"},
        {SCH_TEXT, "Text"},
        {SCH_SELECT, "Select"},
        {SCH_WEEKDAY, "Select Weekday"},
        {SCH_TIME, "Time"},
        {SCH_DATE, "Date"},
    };
}
#endif