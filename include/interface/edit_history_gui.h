#pragma once

#include <gui.h>
#include <schedule_edit_history.h>

class EditHistoryGui : public Gui {
    private:
        const ScheduleEditHistory* m_scheduleEditHistory;

    public:
        EditHistoryGui(const char* ID) : Gui(ID) {
        }

        Event<> undoEvent;
        Event<> redoEvent;

        void draw(Window& window, Input& input, GuiTextures& guiTextures) override;
        void passScheduleEditHistory(const ScheduleEditHistory*);
};