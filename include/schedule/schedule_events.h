#pragma once

#include "event.h"
#include "schedule_edit.h"

struct ScheduleEvents
{
    Event<size_t> columnRemoved;
    Event<size_t> columnAdded;
    Event<const ScheduleEdit*> editUndone;
    Event<const ScheduleEdit*> editRedone;
};