#pragma once

#include "event.h"

struct ScheduleEvents
{
    Event<size_t> columnRemoved;
    Event<size_t> columnAdded;
};