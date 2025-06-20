#pragma once

#include "gui.h"
#include "time_wrapper.h"

class CalendarGui : public Gui {
    private:
        TimeWrapper m_viewedMonth;
        TimeWrapper m_selectedDate;

        void drawWeekdayHeaders(float headerWidth);
        void drawCalendarTable();

    public:
        CalendarGui(const char* ID);
        void draw(const WindowSize& windowSize, Input& input, GuiTextures& guiTextures) override;
};