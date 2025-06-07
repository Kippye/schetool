#include <stdexcept>
#include "filters/filter.h"

Filter::Filter(const std::vector<FilterRuleContainer>& rules, LogicalOperatorEnum logicalOperator)
    : m_rules(rules), m_operator(logicalOperator) {
}

bool Filter::checkPasses(const ElementBase* element, const TimeWrapper& currentTime, bool useDefaultValue) const {
    bool passes = true;  // true by default so having 0 rules returns true

    for (size_t i = 0; i < m_rules.size(); i++) {
        passes = i == 0
            ? m_rules[i].checkPasses(
                  element, currentTime, useDefaultValue)  // store the first filter's result to properly handle all operators
            : m_operator.apply(
                  passes,
                  m_rules[i].checkPasses(
                      element,
                      currentTime,
                      useDefaultValue));  // after the first element, apply the operator to previous and current results
    }

    return passes;
}

FilterRuleContainer Filter::getRule(size_t index) {
    if (index < m_rules.size() == false) {
        printf("Filter::getRule(%zu): Index out of range(size %zu)\n", index, m_rules.size());
        return FilterRuleContainer();
    }

    return m_rules.at(index);
}

const FilterRuleContainer& Filter::getRuleConst(size_t index) const {
    if (index < m_rules.size() == false) {
        printf("Filter::getRuleConst(%zu): Index out of range(size %zu)\n", index, m_rules.size());
        throw std::out_of_range("Index out of range in reference getter.");
    }

    return m_rules.at(index);
}

std::vector<FilterRuleContainer>& Filter::getRules() {
    return m_rules;
}

size_t Filter::getRuleCount() const {
    return m_rules.size();
}

void Filter::setOperator(LogicalOperatorEnum newOperator) {
    m_operator = newOperator;  // implicitly convert enum to operator
}

LogicalOperatorEnum Filter::getOperatorType() const {
    return m_operator.getOperatorType();
}

void Filter::removeRule(size_t index) {
    if (index >= m_rules.size()) {
        printf("Filter::removeRule(%zu): Index out of range (size %zu)\n", index, m_rules.size());
        return;
    }

    m_rules.erase(m_rules.begin() + index);
}