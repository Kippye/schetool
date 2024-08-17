#pragma once

#include <vector>
#include "schedule_constants.h"
#include "element_base.h"
#include "filter_rule_container.h"

class Filter
{
    private:
        std::vector<FilterRuleContainer> m_rules = {};
        LogicalOperator m_operator = LogicalOperatorEnum::Or;
    public:
        Filter() = default;
        Filter(const std::vector<FilterRuleContainer>& rules, LogicalOperatorEnum logicalOperator = LogicalOperatorEnum::Or);

        bool checkPasses(const ElementBase* element, const TimeWrapper& currentTime = TimeWrapper::getCurrentTime()) const;

        FilterRuleContainer getRule(size_t index);
        const FilterRuleContainer& getRuleConst(size_t index) const;
        std::vector<FilterRuleContainer>& getRules();
        size_t getRuleCount() const;

        void setOperator(LogicalOperatorEnum newOperator);
        LogicalOperatorEnum getOperatorType() const;

        template <typename T>
        void addRule(size_t ruleIndex, const FilterRule<T>& filterRule)
        {
            m_rules.insert(m_rules.begin() + ruleIndex, FilterRuleContainer(filterRule));
        }
        template <typename T>
        void addRule(const FilterRule<T>& filterRule)
        {
            addRule(getRuleCount(), filterRule);
        }

        template <typename T>
        void replaceRule(size_t index, const FilterRule<T>& filterRule)
        {
            if (index >= m_rules.size()) { printf("Filter::replaceRule(%zu): Index out of range (size %zu)\n", index, m_rules.size()); return; }

            m_rules.at(index) = FilterRuleContainer(filterRule);
        }

        void removeRule(size_t index);
};