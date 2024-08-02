#pragma once

#include <memory>
#include "event.h"
#include "schedule_edit.h"

struct ScheduleEvents
{
    Event<size_t> columnRemoved;
    Event<size_t> columnAdded;
    Event<std::shared_ptr<const ScheduleEdit>> editUndone;
    Event<std::shared_ptr<const ScheduleEdit>> editRedone;
};