#ifndef FILTER_CONSTANTS
#define FILTER_CONSTANTS

#include <map>
#include <vector>
#include <string>

#include "general_constants.h"

enum class Comparison
{
    Is,
    IsNot,
    IsLessThan,
    IsMoreThan,
    Contains,
    DoesNotContain,
    IsEmpty,
    ContainsToday,
};

enum class LogicalOperatorEnum 
{
    And,
    Or
};

class LogicalOperator
{
    private:
        LogicalOperatorEnum m_operatorType = LogicalOperatorEnum::And;    
    public:
        LogicalOperator() = default;
        LogicalOperator(LogicalOperatorEnum type);
        
        LogicalOperatorEnum getOperatorType() const;

        bool apply(bool a, bool b) const;
};

namespace filter_consts
{
    const size_t FILTER_GROUP_NAME_MAX_LENGTH = 30;

    const std::map<LogicalOperatorEnum, const char*> logicalOperatorStrings  =
    {
        { LogicalOperatorEnum::And, "AND" },
        { LogicalOperatorEnum::Or, "OR" }
    };

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
        { SCH_BOOL,         { Comparison::Is, Comparison::IsNot } },
        { SCH_NUMBER,       { Comparison::Is, Comparison::IsNot, Comparison::IsLessThan, Comparison::IsMoreThan } },
        { SCH_DECIMAL,      { Comparison::Is, Comparison::IsNot, Comparison::IsLessThan, Comparison::IsMoreThan } },
        { SCH_TEXT,         { Comparison::Is, Comparison::IsNot } },
        { SCH_SELECT,       { Comparison::Is, Comparison::IsNot, Comparison::IsEmpty } },
        { SCH_MULTISELECT,  { Comparison::Is, Comparison::IsNot, Comparison::IsEmpty,  Comparison::Contains, Comparison::DoesNotContain } },
        { SCH_WEEKDAY,      { Comparison::Is, Comparison::IsNot, Comparison::IsEmpty, Comparison::Contains, Comparison::DoesNotContain, Comparison::ContainsToday } },
        { SCH_TIME,         { Comparison::Is, Comparison::IsNot } },
        { SCH_DATE,         { Comparison::Is, Comparison::IsNot, Comparison::IsLessThan, Comparison::IsMoreThan, Comparison::IsEmpty } },
    };

    // Contains the strings to display in the filter editor for each comparison.
    const std::map<Comparison, const char*> comparisonStrings =
    {
        { Comparison::Is,                   "is" },
        { Comparison::IsNot,                "is not" },
        { Comparison::IsLessThan,           "is less than" },
        { Comparison::IsMoreThan,           "is more than" },
        { Comparison::Contains,             "contains" },
        { Comparison::DoesNotContain,       "does not contain" },
        { Comparison::ContainsToday,        "contains today" },
        { Comparison::IsEmpty,              "is empty" },
    };

    TypeComparisonInfo getComparisonInfo(SCHEDULE_TYPE type);
}
#endif