#pragma once

#include "element.h"
#include "filter_constants.h"

class FilterRuleBase
{
    protected:
        Comparison m_comparison = Comparison::Is;
        bool m_dateCompareCurrent = false;
    public:
        bool isComparisonValidForElement(const ElementBase* element, bool printInvalidWarning = true) const;
        virtual bool checkPasses(const ElementBase* element, const TimeWrapper& currentTime = TimeWrapper::getCurrentTime()) const;
        virtual std::string getString() const;
        Comparison getComparison() const;
        void setComparison(Comparison comparison);
        virtual bool getDateCompareCurrent() const;
        virtual void setDatePassCompareCurrent(bool shouldCompareToCurrent);
};