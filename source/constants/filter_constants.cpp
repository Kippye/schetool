#include "filter_constants.h"
#include <stdexcept>
#include <format>
#include <iostream>

LogicalOperator::LogicalOperator(LogicalOperatorEnum type) : m_operatorType(type) {
}

bool LogicalOperator::apply(bool a, bool b) const {
    switch (m_operatorType) {
        case LogicalOperatorEnum::And: {
            return a && b;
        }
        case LogicalOperatorEnum::Or: {
            return a || b;
        }
        default: {
            throw std::runtime_error(std::format("LogicalOperator::apply({}, {}) Unimplemented binary operator {}", a, b, static_cast<int>(m_operatorType)));
        }
    }
}

LogicalOperatorEnum LogicalOperator::getOperatorType() const {
    return m_operatorType;
}

using filter_consts::TypeComparisonInfo;

TypeComparisonInfo filter_consts::getComparisonInfo(SCHEDULE_TYPE type) {
    if (typeComparisons.find(type) == typeComparisons.end()) {
        std::cout << std::format("filter_consts::getComparisonInfo({}): No comparison info for type.", (size_t)type) << std::endl;
        return TypeComparisonInfo({}, {});
    }

    auto comparisons = typeComparisons.at(type);

    std::vector<std::string> comparisonNames = {};

    for (Comparison comparison : comparisons) {
        comparisonNames.push_back(std::string(comparisonStrings.at(comparison)));
    }

    return TypeComparisonInfo(typeComparisons.at(type), comparisonNames);
}