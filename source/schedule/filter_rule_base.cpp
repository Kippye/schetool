#include "filter_rule_base.h"
#include <cstring>

bool FilterRuleBase::isComparisonValidForElement(const ElementBase* element, bool printInvalidWarning) const
{
    bool validComparison = false;
    for (Comparison comparison : filter_consts::getComparisonInfo(element->getType()).comparisons)
    {
        if (comparison == m_comparison)
        {
            validComparison = true;
            break;
        }
    }

    if (validComparison == false && printInvalidWarning)
    {
        printf("FilterRuleBase::isComparisonValidForElement(): Comparison %s is not valid for element type %d\n", filter_consts::comparisonStrings.at(m_comparison), element->getType());
    }

    return validComparison;
}

bool FilterRuleBase::checkPasses(const ElementBase* element, const TimeWrapper& currentTime, bool useDefaultValue) const
{
    if (isComparisonValidForElement(element) == false) { return false; }
    return true;
}
 
Comparison FilterRuleBase::getComparison() const
{
    return m_comparison;
}

std::string FilterRuleBase::getString() const
{
    return std::string(filter_consts::comparisonStrings.at(m_comparison)).append(" any");
}

void FilterRuleBase::setComparison(Comparison comparison)
{
    m_comparison = comparison;
}

bool FilterRuleBase::getDateCompareCurrent() const
{
    return false;
}

void FilterRuleBase::setDatePassCompareCurrent(bool shouldCompareToCurrent)
{
    m_dateCompareCurrent = shouldCompareToCurrent;
}