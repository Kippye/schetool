#pragma once

#include "gui.h"
#include "event.h"
#include "schedule_constants.h"
#include "time_wrapper.h"

class ViewTabBarGui : public Gui {
    private:
        static float height;
        ScheduleView m_selectedView = ScheduleView::Table;
        // Viewed date selection
        bool m_openDateSelectPopup = false;
        unsigned int m_dateSelectorYear = 1, m_dateSelectorMonth = 1;
        TimeWrapper m_viewedDateOverride = TimeWrapper();

    public:
        ViewTabBarGui(const char* ID);

        Event<ScheduleView> viewSwitched;
        Event<TimeWrapper> viewedDateChanged;

        void draw(const WindowSize& windowSize, Input& input, GuiTextures& guiTextures) override;
        void clearDateOverride();
        // Static function. Assuming that there is only one ViewTabBarGui instance or they are all the same height.
        // Get the height of the ViewTabBarGui.
        static float getHeight();
};