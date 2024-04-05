#pragma once

#include <element.h>

enum SCHEDULE_EDIT_TYPE
{
    SCHEDULE_EDIT_ELEMENT
};

class ScheduleEdit
{
    public:
        SCHEDULE_EDIT_TYPE type;  
        ScheduleEdit(SCHEDULE_EDIT_TYPE type) { this->type = type; }
};

class ElementEdit : public ScheduleEdit
{
    private:

    public:
        SCHEDULE_TYPE elementType;
        ElementBase* previousValue;
        ElementEdit(SCHEDULE_TYPE elementType, ElementBase* previousValue) : ScheduleEdit(SCHEDULE_EDIT_ELEMENT) 
        {
            this->elementType = elementType;
            this->previousValue = previousValue;
        }
};