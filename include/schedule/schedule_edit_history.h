#pragma once
#include <deque>
#include <memory>
#include <schedule_edit.h>
#include <schedule_core.h>

class ScheduleEditHistory
{
    private:
        std::deque<std::shared_ptr<ScheduleEdit>> m_editHistory = {};
        ScheduleCore& m_core;
        size_t m_editHistoryIndex = 0;
        bool m_editedSinceWrite = false;

        void addEdit(std::shared_ptr<ScheduleEdit> edit);
    public:
        ScheduleEditHistory() = delete;
        ScheduleEditHistory(ScheduleCore& scheduleCore);

        const std::deque<std::shared_ptr<ScheduleEdit>>& getEditHistory() const;
        size_t getEditHistoryIndex() const;
        // Clear the edit history. Call it when, for example, reading a Schedule from file.
        void clearEditHistory();
        bool getEditedSinceWrite() const;
        void setEditedSinceWrite(bool to);

        // Construct an edit of the given type and add it to the edit history.
        // Tip: Call the edit class constructor to see what arguments are required, then replace the constructor part with this function.
        template <typename T, typename... Args>
        void addEdit(Args&&... args)
        {
            addEdit(std::make_shared<T>(std::forward<Args>(args)...));
        }
        void removeFollowingEditHistory();
        // Undo an edit.
        // Returns true if an edit was undone.
        bool undo();
        // Redo an edit.
        // Returns true if an edit was reapplied.
        bool redo();
};