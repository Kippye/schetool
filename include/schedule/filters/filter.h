#pragma once

#include <vector>
#include <format>
#include "filter_constants.h"
#include "element_base.h"
#include "filters/filter_rule_container.h"

class Filter {
    private:
        std::vector<FilterRuleContainer> m_rules = {};
        LogicalOperator m_operator = LogicalOperatorEnum::Or;

    public:
        Filter() = default;
        Filter(const std::vector<FilterRuleContainer>& rules, LogicalOperatorEnum logicalOperator = LogicalOperatorEnum::Or);

        bool checkPasses(const ElementBase* element,
                         const TimeWrapper& currentTime = TimeWrapper::getCurrentTime(),
                         bool useDefaultValue = false) const;

        FilterRuleContainer getRule(size_t index);
        const FilterRuleContainer& getRuleConst(size_t index) const;
        std::vector<FilterRuleContainer>& getRules();
        size_t getRuleCount() const;

        void setOperator(LogicalOperatorEnum newOperator);
        LogicalOperatorEnum getOperatorType() const;

        template <typename T>
        void addRule(size_t ruleIndex, const FilterRule<T>& filterRule) {
            m_rules.insert(m_rules.begin() + ruleIndex, FilterRuleContainer(filterRule));
        }
        template <typename T>
        void addRule(const FilterRule<T>& filterRule) {
            addRule(getRuleCount(), filterRule);
        }

        template <typename T>
        void replaceRule(size_t index, const FilterRule<T>& filterRule) {
            if (index >= m_rules.size()) {
                throw std::out_of_range(std::format("Filter::replaceRule({}): Index out of range (size {})\n", index, m_rules.size()));
            }

            m_rules.at(index) = FilterRuleContainer(filterRule);
        }

        void removeRule(size_t index);
};