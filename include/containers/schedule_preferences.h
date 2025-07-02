#pragma once

#include "schedule_constants.h"

// Class for containing file-specific / schedule preferences, such as which view the user wants to see.
class SchedulePreferences {
    private:
        ScheduleView m_view = ScheduleView::Table;

    public:
        SchedulePreferences() {
        }
        SchedulePreferences(ScheduleView view) {
            m_view = view;
        }

        ScheduleView getView() const {
            return m_view;
        }
        void setView(ScheduleView view) {
            m_view = view;
        }
};