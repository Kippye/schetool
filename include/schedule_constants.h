#ifndef SCHEDULE_CONSTANTS
#define SCHEDULE_CONSTANTS
#include <map>
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

enum class Comparison
{
    Is,
    Contains,
    IsRelativeToToday,
    ContainsToday
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
        {SCH_WEEKDAY, "Weekday"},
        {SCH_TIME, "Time"},
        {SCH_DATE, "Date"},
    };
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

namespace filter_consts
{
    struct TypeComparisonInfo
    {
        public:
            const std::vector<Comparison> comparisons;
            const std::vector<std::string> names;

            TypeComparisonInfo(const std::vector<Comparison>& _comparisons, const std::vector<std::string>& _names) : comparisons(_comparisons), names(_names) 
            {
                
            }
    };

    const std::map<SCHEDULE_TYPE, std::vector<Comparison>> typeComparisons =
    {
        { SCH_BOOL,     { Comparison::Is } },
        { SCH_NUMBER,   { Comparison::Is } },
        { SCH_DECIMAL,  { Comparison::Is } },
        { SCH_TEXT,     { Comparison::Is } },
        { SCH_SELECT,   { Comparison::Is, Comparison::Contains } },
        { SCH_WEEKDAY,  { Comparison::Is, Comparison::Contains, Comparison::ContainsToday } },
        { SCH_TIME,     { Comparison::Is } },
        { SCH_DATE,     { Comparison::Is, Comparison::IsRelativeToToday } },
    };

    const std::map<Comparison, const char*> comparisonStrings =
    {
        { Comparison::Is, "is" },
        { Comparison::Contains, "contains" },
        { Comparison::IsRelativeToToday, "is relative to today" },
        { Comparison::ContainsToday, "contains today" },
    };

    TypeComparisonInfo getComparisonInfo(SCHEDULE_TYPE type);
}
#endif