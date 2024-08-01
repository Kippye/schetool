#pragma once
#include <deque>
#include <schedule_edit.h>
#include <schedule_core.h>

class ScheduleEditHistory
{
    private:
        std::deque<ScheduleEdit*> m_editHistory = {};
        ScheduleCore& m_core;
        size_t m_editHistoryIndex = 0;
        bool m_editedSinceWrite = false;
    public:
        ScheduleEditHistory() = delete;
        ScheduleEditHistory(ScheduleCore& scheduleCore);

        const std::deque<ScheduleEdit*>& getEditHistory() const;
        size_t getEditHistoryIndex() const;
        // Clear the edit history. Call it when, for example, reading a Schedule from file.
        void clearEditHistory();
        bool getEditedSinceWrite() const;
        void setEditedSinceWrite(bool to);

        void addEdit(ScheduleEdit* edit);
        template <typename T, typename... Args>
        void addEdit(Args&&... args)
        {
            addEdit(new T(std::forward<Args>(args)...));
        }
        void removeFollowingEditHistory();
        // Undo an edit.
        // Returns true if an edit was undone.
        bool undo();
        // Redo an edit.
        // Returns true if an edit was reapplied.
        bool redo();
};