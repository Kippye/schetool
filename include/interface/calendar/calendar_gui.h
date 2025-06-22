#pragma once

#include "gui.h"
#include "time_wrapper.h"
#include "date_container.h"
#include "schedule_core.h"
#include "schedule_events.h"
#include "schedule_coordinates.h"
#include "element_display_templates.h"
#include <optional>

typedef ScheduleCoordinates TableCoordinates;

class CalendarGui : public Gui {
    private:
        const ScheduleCore& m_scheduleCore;
        TimeWrapper m_viewedMonth;
        TimeWrapper m_selectedDate;
        TimeWrapper m_scheduleDateOverride = TimeWrapper();
        std::optional<ImGuiID> m_hoveredItemChildID;
        // Only meaningful while draw() - specifically drawCalendarTable() - is running.
        TableCoordinates m_currentTableCoords = TableCoordinates(0, 0);
        // TEMP?
        std::optional<GuiPassReferences> m_guiPass = std::nullopt;

        std::function<void(TimeWrapper)> viewedDateChangedListener = [&](TimeWrapper newDateOverride) {
            m_scheduleDateOverride = newDateOverride;
        };

        void drawWeekdayHeaders(float headerWidth);
        void drawCalendarTable();
        void drawCalendarDayContent(size_t& dayIndex, int month, int dayNumber);
        void drawCalendarDayItems(const DateContainer& calendarDayDate);
        void drawItemProperty(ScheduleCoordinates coords);
        template <typename T>
        T getElementValue(ScheduleCoordinates coords, bool useDefaultValue) const {
            return useDefaultValue == true ? Element<T>::getDefaultValue()
                                           : m_scheduleCore.getElementValueConstRef<T>(coords.column(), coords.row());
        }

    public:
        CalendarGui(const char* ID, const ScheduleCore& scheduleCore, ScheduleEvents& scheduleEvents);
        void draw(const WindowSize& windowSize, Input& input, GuiTextures& guiTextures) override;
};