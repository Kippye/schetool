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
    IsNot,
    Contains,
    IsRelativeToToday,
    IsEmpty,
    ContainsToday,
    // TEMP
    ContainsTodayOrIsEmpty
};

namespace schedule_consts
{
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
        { SCH_BOOL,     { Comparison::Is, Comparison::IsNot } },
        { SCH_NUMBER,   { Comparison::Is, Comparison::IsNot } },
        { SCH_DECIMAL,  { Comparison::Is, Comparison::IsNot } },
        { SCH_TEXT,     { Comparison::Is, Comparison::IsNot } },
        { SCH_SELECT,   { Comparison::Is, Comparison::IsNot, Comparison::Contains } },
        { SCH_WEEKDAY,  { Comparison::Is, Comparison::IsNot, Comparison::Contains, Comparison::ContainsToday, Comparison::ContainsTodayOrIsEmpty } },
        { SCH_TIME,     { Comparison::Is, Comparison::IsNot } },
        { SCH_DATE,     { Comparison::Is, Comparison::IsNot, Comparison::IsEmpty, Comparison::IsRelativeToToday } },
    };

    const std::map<Comparison, const char*> comparisonStrings =
    {
        { Comparison::Is, "is" },
        { Comparison::IsNot, "is not" },
        { Comparison::Contains, "contains" },
        { Comparison::IsRelativeToToday, "is relative to today" },
        { Comparison::IsEmpty, "is empty" },
        { Comparison::ContainsToday, "contains today" },
        { Comparison::ContainsTodayOrIsEmpty, "contains today or is empty" },
    };

    TypeComparisonInfo getComparisonInfo(SCHEDULE_TYPE type);
}
#endif