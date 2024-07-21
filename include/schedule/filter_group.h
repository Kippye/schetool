#pragma once

#include <vector>
#include "filter.h"
#include "schedule_constants.h"
#include "element_base.h"

class FilterGroup
{
    private:
        std::vector<Filter> m_filters = {};
        std::string m_name = "Filter Group";
        LogicalOperator m_operator = LogicalOperatorEnum::And;
    public:
        FilterGroup() = default;
        FilterGroup(const std::vector<Filter>& filters, const std::string& name = "Filter Group", LogicalOperatorEnum logicalOperator = LogicalOperatorEnum::And);
        bool checkPasses(const ElementBase* element) const;
        bool hasFilterAt(size_t index) const;

        Filter& getFilter(size_t index);
        const Filter& getFilterConst(size_t index) const;
        std::vector<Filter>& getFilters();
        size_t getFilterCount() const;

        void setName(const std::string& name);
        std::string getName() const;

        void setOperator(LogicalOperatorEnum newOperator);
        LogicalOperatorEnum getOperatorType() const;

        void addFilter(const Filter& filter);

        template <typename T>
        void replaceFilter(size_t index, const Filter& filter);
        // {
        //     if (index >= m_rules.size()) { printf("Filter::replaceRule(%zu): Index out of range (size %zu)\n", index, m_rules.size()); return; }

        //     m_rules.at(index) = FilterRuleContainer(filterRule);
        // }

        void removeFilter(size_t index);
};