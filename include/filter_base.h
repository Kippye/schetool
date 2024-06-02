#pragma once

#include "element.h"

class FilterBase
{
    public:
        virtual bool checkPasses(const ElementBase* element);
};