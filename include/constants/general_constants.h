#ifndef GENERAL_CONSTANTS
#define GENERAL_CONSTANTS

#include <vector>
#include <string>

constexpr int YEAR_MIN = 1678;
constexpr int YEAR_MAX = 2261;

enum SCHEDULE_TYPE {
    SCH_BOOL,
    SCH_NUMBER,
    SCH_DECIMAL,
    SCH_TEXT,
    SCH_SELECT,
    SCH_MULTISELECT,
    SCH_WEEKDAY,
    SCH_TIME,
    SCH_DATE,
    SCH_LAST,
};

namespace general_consts {
    const std::vector<std::string> weekdayNames = {
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