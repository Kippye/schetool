#include "filters/filter_rule_base.h"
#include <cstring>
#include <algorithm>

bool FilterRuleBase::isComparisonValidForElement(std::weak_ptr<const ElementBase> element, bool printInvalidWarning) const {
    auto elementAccess = element.lock();
    bool validForElement = std::ranges::any_of(filter_consts::getComparisonInfo(elementAccess->getType()).comparisons,
                                               [this](Comparison comparison) { return comparison == m_comparison; });

    if (validForElement == false && printInvalidWarning) {
        printf("FilterRuleBase::isComparisonValidForElement(): Comparison %s is not valid for element type %d\n",
               filter_consts::comparisonStrings.at(m_comparison),
               elementAccess->getType());
    }

    return validForElement;
}

bool FilterRuleBase::checkPasses(std::weak_ptr<const ElementBase> element,
                                 const TimeWrapper& currentTime,
                                 bool useDefaultValue) const {
    return isComparisonValidForElement(element);
}

Comparison FilterRuleBase::getComparison() const {
    return m_comparison;
}

std::string FilterRuleBase::getString() const {
    return std::string(filter_consts::comparisonStrings.at(m_comparison)).append(" any");
}

void FilterRuleBase::setComparison(Comparison comparison) {
    m_comparison = comparison;
}

bool FilterRuleBase::getDateCompareCurrent() const {
    return false;
}

void FilterRuleBase::setDatePassCompareCurrent(bool shouldCompareToCurrent) {
    m_dateCompareCurrent = shouldCompareToCurrent;
}