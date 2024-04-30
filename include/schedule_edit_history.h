#pragma once
#include <deque>
#include <schedule_edit.h>
#include <schedule_core.h>

class ScheduleEditHistory
{
    private:
        std::deque<ScheduleEdit*> m_editHistory = {};
        ScheduleCore* m_core;
        size_t m_editHistoryIndex = 0;
        bool m_editedSinceWrite = false;
    public:
        ScheduleEditHistory();
        ScheduleEditHistory(ScheduleCore* scheduleCore);

        const std::deque<ScheduleEdit*>& getEditHistory() const;
        size_t getEditHistoryIndex() const;
        // Clear the edit history. Call it when, for example, reading a Schedule from file.
        void clearEditHistory();
        bool getEditedSinceWrite() const;
        void setEditedSinceWrite(bool to);

        void addEdit(ScheduleEdit* edit);
        void removeFollowingEditHistory();
        void undo();
        void redo();
};