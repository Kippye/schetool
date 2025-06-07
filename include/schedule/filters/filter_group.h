#pragma once

#include <vector>
#include "filters/filter.h"
#include "filter_constants.h"
#include "element_base.h"

class FilterGroup {
    private:
        std::vector<Filter> m_filters = {};
        std::string m_name = "Filter Group";
        bool m_enabled = true;
        LogicalOperator m_operator = LogicalOperatorEnum::Or;

    public:
        FilterGroup() = default;
        FilterGroup(const std::vector<Filter>& filters,
                    const std::string& name = "Filter Group",
                    LogicalOperatorEnum logicalOperator = LogicalOperatorEnum::Or,
                    bool enabled = true);
        bool checkPasses(const ElementBase* element,
                         const TimeWrapper& currentTime = TimeWrapper::getCurrentTime(),
                         bool useDefaultValue = false) const;
        bool hasFilterAt(size_t index) const;

        Filter& getFilter(size_t index);
        const Filter& getFilterConst(size_t index) const;
        std::vector<Filter>& getFilters();
        size_t getFilterCount() const;

        void setName(const std::string& name);
        std::string getName() const;

        void setEnabled(bool enabled);
        bool getIsEnabled() const;

        void setOperator(LogicalOperatorEnum newOperator);
        LogicalOperatorEnum getOperatorType() const;

        void addFilter(size_t filterIndex, const Filter& filter);
        void addFilter(const Filter& filter);
        void removeFilter(size_t index);
};