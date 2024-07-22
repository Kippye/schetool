#include <stdexcept>
#include "filter_group.h"

FilterGroup::FilterGroup(const std::vector<Filter>& filters, const std::string& name, LogicalOperatorEnum logicalOperator) 
: m_filters(filters), m_name(name), m_operator(logicalOperator)
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

bool FilterGroup::hasFilterAt(size_t index) const
{
    return index < m_filters.size();
}

Filter& FilterGroup::getFilter(size_t index)
{
    if (index < m_filters.size() == false) 
    { 
        printf("FilterGroup::getFilter(%zu): Index out of range(size %zu)\n", index, m_filters.size());
        throw std::out_of_range("Index out of range in reference getter.");
    }

    return m_filters.at(index);
}
  
const Filter& FilterGroup::getFilterConst(size_t index) const
{
    if (index < m_filters.size() == false) 
    { 
        printf("FilterGroup::getFilterConst(%zu): Index out of range(size %zu)\n", index, m_filters.size());
        throw std::out_of_range("Index out of range in reference getter.");
    }

    return m_filters.at(index);
}

std::vector<Filter>& FilterGroup::getFilters()
{
    return m_filters;
}

size_t FilterGroup::getFilterCount() const
{
    return m_filters.size();
}

void FilterGroup::setName(const std::string& name)
{
    m_name = name;
}

std::string FilterGroup::getName() const
{
    return m_name;
}

void FilterGroup::setOperator(LogicalOperatorEnum newOperator)
{
    m_operator = newOperator; // implicitly convert enum to operator
}

LogicalOperatorEnum FilterGroup::getOperatorType() const
{
    return m_operator.getOperatorType();
}

void FilterGroup::addFilter(size_t filterIndex, const Filter& filter)
{
    m_filters.insert(m_filters.begin() + filterIndex, filter);
}

void FilterGroup::addFilter(const Filter& filter)
{
    addFilter(getFilterCount(), filter);
}

void FilterGroup::removeFilter(size_t index)
{
    if (index >= m_filters.size()) { printf("FilterGroup::removeFilter(%zu): Index out of range (size %zu)\n", index, m_filters.size()); return; }

    m_filters.erase(m_filters.begin() + index);
}