#include "filter_base.h"
#include <cstring>

bool FilterBase::isComparisonValidForElement(const ElementBase* element, bool printInvalidWarning) const
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
        printf("FilterBase::isComparisonValidForElement(): Comparison %s is not valid for element type %d\n", filter_consts::comparisonStrings.at(m_comparison), element->getType());
    }

    return validComparison;
}

bool FilterBase::checkPasses(const ElementBase* element) const
{
    if (isComparisonValidForElement(element) == false) { return false; }
    return true;
}
 
Comparison FilterBase::getComparison() const
{
    return m_comparison;
}

void FilterBase::setComparison(Comparison comparison)
{
    m_comparison = comparison;
}