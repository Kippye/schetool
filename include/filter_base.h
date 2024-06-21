#pragma once

#include "element.h"
#include "schedule_constants.h"

class FilterBase
{
    protected:
        Comparison m_comparison = Comparison::Is;
    public:
        bool isComparisonValidForElement(const ElementBase* element, bool printInvalidWarning = true);
        virtual bool checkPasses(const ElementBase* element);
        Comparison getComparison() const;
        void setComparison(Comparison comparison);
};