#pragma once
#include <deque>
#include <vector>
#include <element_base.h>
#include <schedule_edit.h>
#include <schedule_column.h>

class ScheduleEditHistory
{
    private:
        std::deque<ScheduleEdit*> m_editHistory = {};
        ScheduleEditFunctions m_editFunctions;
        size_t m_editHistoryIndex = 0;
        bool m_editedSinceWrite = false;
    public:
        ScheduleEditHistory();
        ScheduleEditHistory(const ScheduleEditFunctions& editFunctions);

        const std::deque<ScheduleEdit*>& getEditHistory();
        size_t getEditHistoryIndex();
        // Clear the edit history. Call it when, for example, reading a Schedule from file.
        void clearEditHistory();
        bool getEditedSinceWrite();
        void setEditedSinceWrite(bool to);

        void addEdit(ScheduleEdit* edit);
        void removeFollowingEditHistory();
        void undo();
        void redo();
};