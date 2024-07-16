#pragma once

#include <vector>
#include <memory>
#include "schedule_constants.h"
#include "element_base.h"
#include "filter_rule_container.h"

class Filter
{
    private:
        std::vector<FilterRuleContainer> m_rules = {};
        LogicalOperator m_operator = LogicalOperatorEnum::And;
    public:
        bool checkPasses(const ElementBase* element) const;
        FilterRuleContainer getRule(size_t index);
        std::vector<FilterRuleContainer>& getRules();
        void setOperator(LogicalOperatorEnum newOperator);

        template <typename T>
        void addRule(const FilterRule<T>& filterRule)
        {
            m_rules.push_back(FilterRuleContainer().fill(filterRule));
        }

        template <typename T>
        void replaceRule(size_t index, const FilterRule<T>& filterRule)
        {
            if (index >= m_rules.size()) { printf("Filter::replaceRule(%zu): Index out of range (size %zu)\n", index, m_rules.size()); return; }

            m_rules.at(index) = FilterRuleContainer().fill(filterRule);
        }

        void removeRule(size_t index);
};