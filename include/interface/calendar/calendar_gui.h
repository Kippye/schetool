#pragma once

#include "gui.h"
#include "time_wrapper.h"
#include "date_container.h"
#include "schedule_core.h"
#include <optional>

class CalendarGui : public Gui {
    private:
        const ScheduleCore& m_scheduleCore;
        TimeWrapper m_viewedMonth;
        TimeWrapper m_selectedDate;
        std::optional<ImGuiID> m_hoveredItemChildID;

        void drawWeekdayHeaders(float headerWidth);
        void drawCalendarTable();
        void drawCalendarDayContent(size_t& dayIndex, int month, int dayNumber);
        void drawCalendarDayItems(const DateContainer& calendarDayDate);

    public:
        CalendarGui(const char* ID, const ScheduleCore& scheduleCore);
        void draw(const WindowSize& windowSize, Input& input, GuiTextures& guiTextures) override;
};