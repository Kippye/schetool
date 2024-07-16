#include "schedule_constants.h"

LogicalOperator::LogicalOperator(LogicalOperatorEnum type) : m_operatorType(type) {}

bool LogicalOperator::apply(bool a, bool b) const
{
    switch (m_operatorType)
    {
        case LogicalOperatorEnum::And:
        {
            return a && b;
        }
        case LogicalOperatorEnum::Or:
        {
            return a || b;
        }
        default:
        {
            printf("LogicalOperator::apply(%u, %u) Unimplemented binary operator %d\n", a, b, static_cast<int>(m_operatorType));
            return a;
        }
    }
}

using filter_consts::TypeComparisonInfo;

TypeComparisonInfo filter_consts::getComparisonInfo(SCHEDULE_TYPE type)
{
    if (typeComparisons.find(type) == typeComparisons.end()) { printf("filter_consts::getComparisonInfo(%d): No comparison info for type.\n", type); return TypeComparisonInfo({} , {}); }

    auto comparisons = typeComparisons.at(type);

    std::vector<std::string> comparisonNames = {};

    for (Comparison comparison : comparisons)
    {
        comparisonNames.push_back(std::string(comparisonStrings.at(comparison)));
    }

    return TypeComparisonInfo(typeComparisons.at(type), comparisonNames);
}