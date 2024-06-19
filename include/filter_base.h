#pragma once

#include "element.h"

enum class Comparison
{
    Is,
    Contains,
    IsRelativeToToday,
    ContainsToday
};

struct TypeComparisonInfo
{
    public:
        std::vector<Comparison> comparisons = {};
        std::vector<std::string> names = {};

        TypeComparisonInfo(const std::vector<Comparison>& _comparisons, const std::vector<std::string>& _names) : comparisons(_comparisons), names(_names) 
        {
            
        }
};

class TypeComparisonOptions
{
    private:
        std::map<SCHEDULE_TYPE, int> m_selectedOptions =
        {
            { SCH_BOOL,     0 },
            { SCH_NUMBER,   0 },
            { SCH_DECIMAL,  0 },
            { SCH_TEXT,     0 },
            { SCH_SELECT,   0 },
            { SCH_WEEKDAY,  0 },
            { SCH_TIME,     0 },
            { SCH_DATE,     0 },
        };
        std::map<SCHEDULE_TYPE, std::vector<Comparison>> m_comparisonOptions =
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
    public:
        const std::map<Comparison, const char*> comparisonStrings =
        {
            { Comparison::Is, "is" },
            { Comparison::Contains, "contains" },
            { Comparison::IsRelativeToToday, "is relative to today" },
            { Comparison::ContainsToday, "contains today" },
        };
        TypeComparisonInfo getOptions(SCHEDULE_TYPE type);
        int getOptionSelection(SCHEDULE_TYPE type);
        const char* getComparisonString(Comparison comparison) const;
        void setOptionSelection(SCHEDULE_TYPE type, int selection);
};

class FilterBase
{
    public:
        virtual bool checkPasses(const ElementBase* element);
};