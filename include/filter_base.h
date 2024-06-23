#pragma once

#include "element.h"
#include "schedule_constants.h"

class FilterBase
{
    protected:
        Comparison m_comparison = Comparison::Is;
    public:
        bool isComparisonValidForElement(const ElementBase* element, bool printInvalidWarning = true) const;
        virtual bool checkPasses(const ElementBase* element) const;
        Comparison getComparison() const;
        void setComparison(Comparison comparison);
};