#include "schedule_constants.h"

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