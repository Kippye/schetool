#pragma once

#include "gui.h"
#include "event.h"
#include "schedule_constants.h"
#include "time_wrapper.h"

class ViewTabBarGui : public Gui {
    private:
        static float height;
        ScheduleView m_selectedTab = ScheduleView::Table;
        std::optional<ScheduleView> m_forceTabSelectedIndex = std::nullopt;
        // Viewed date selection
        bool m_openDateSelectPopup = false;
        unsigned int m_dateSelectorYear = 1, m_dateSelectorMonth = 1;
        TimeWrapper m_viewedDateOverride = TimeWrapper();

    public:
        ViewTabBarGui(const char* ID);

        Event<ScheduleView> viewSwitched;
        Event<TimeWrapper> viewedDateChanged;

        void draw(GuiDrawArgs& args) override;
        // While this gui is meant for choosing the view, it might need to get an external update.
        // This is mostly when a file is read.
        // Note that this DOES NOT INVOKE the viewSwitched event!
        void setSelectedView(ScheduleView view);
        void clearDateOverride();
        // Static function. Assuming that there is only one ViewTabBarGui instance or they are all the same height.
        // Get the height of the ViewTabBarGui.
        static float getHeight();
};