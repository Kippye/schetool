#pragma once

#include <vector>
#include "filter.h"
#include "schedule_constants.h"
#include "element_base.h"

class FilterGroup
{
    private:
        std::vector<Filter> m_filters = {};
        LogicalOperator m_operator = LogicalOperatorEnum::And;
    public:
        FilterGroup() = default;
        FilterGroup(const std::vector<Filter>& filters, LogicalOperatorEnum logicalOperator = LogicalOperatorEnum::And);
        bool checkPasses(const ElementBase* element) const;
        Filter getFilter(size_t index);
        std::vector<Filter>& getFilters();
        void setOperator(LogicalOperatorEnum newOperator);

        void addFilter(const Filter& filter);

        template <typename T>
        void replaceFilter(size_t index, const Filter& filter);
        // {
        //     if (index >= m_rules.size()) { printf("Filter::replaceRule(%zu): Index out of range (size %zu)\n", index, m_rules.size()); return; }

        //     m_rules.at(index) = FilterRuleContainer().fill(filterRule);
        // }

        void removeFilter(size_t index);
};