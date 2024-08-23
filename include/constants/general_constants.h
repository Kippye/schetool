#ifndef GENERAL_CONSTANTS
#define GENERAL_CONSTANTS

#include <vector>
#include <string>

enum SCHEDULE_TYPE
{
    SCH_BOOL,
    SCH_NUMBER,
    SCH_DECIMAL,
    SCH_TEXT,
    SCH_SELECT,
    SCH_WEEKDAY,
    SCH_TIME,
    SCH_DATE,
    SCH_LAST,
};

namespace general_consts
{
    const std::vector<std::string> weekdayNames =
    {
        "Monday",
        "Tuesday",
        "Wednesday",
        "Thursday",
        "Friday",
        "Saturday",
        "Sunday",
    };
}
#endif