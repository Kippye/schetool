#include "filter_group.h"

FilterGroup::FilterGroup(const std::vector<Filter>& filters, LogicalOperatorEnum logicalOperator) 
: m_filters(filters), m_operator(logicalOperator)
{}

bool FilterGroup::checkPasses(const ElementBase* element) const
{
    bool passes = true; // true by default so having 0 filters returns true

    for (size_t i = 0; i < m_filters.size(); i++)
    {
        passes = i == 0 
            ? m_filters[i].checkPasses(element) // store the first filter's result to properly handle all operators
            : m_operator.apply(passes, m_filters[i].checkPasses(element)); // after the first element, apply the operator to previous and current results
    }

    return passes;
}

Filter FilterGroup::getFilter(size_t index)
{
    if (index < m_filters.size() == false) 
    { 
        printf("FilterGroup::getFilter(%zu): Index out of range(size %zu)\n", index, m_filters.size());
        return Filter();
    }

    return m_filters.at(index);
}

std::vector<Filter>& FilterGroup::getFilters()
{
    return m_filters;
}

void FilterGroup::setOperator(LogicalOperatorEnum newOperator)
{
    m_operator = newOperator; // implicitly convert enum to operator
}

void FilterGroup::addFilter(const Filter& filter)
{
    m_filters.push_back(filter);
}

void FilterGroup::removeFilter(size_t index)
{
    if (index >= m_filters.size()) { printf("FilterGroup::removeFilter(%zu): Index out of range (size %zu)\n", index, m_filters.size()); return; }

    m_filters.erase(m_filters.begin() + index);
}