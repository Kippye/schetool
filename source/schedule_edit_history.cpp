#include <schedule_edit_history.h>
#include <iostream>

ScheduleEditHistory::ScheduleEditHistory()
{
    std::cout << "Schedule edit history created with default constructor. A pointer to the Schedule core must be provided!" << std::endl;
}

ScheduleEditHistory::ScheduleEditHistory(ScheduleCore* scheduleCore)
{
    m_core = scheduleCore;
}

const std::deque<ScheduleEdit*>& ScheduleEditHistory::getEditHistory() const
{
    return m_editHistory;
}

size_t ScheduleEditHistory::getEditHistoryIndex() const
{
    return m_editHistoryIndex;
}

void ScheduleEditHistory::clearEditHistory()
{
    if (m_editHistory.size() == 0) { return; }

    for (int i = m_editHistory.size() - 1; i > 0; i--)
    {
        delete m_editHistory[i];
    }

    m_editHistory.clear();
}

bool ScheduleEditHistory::getEditedSinceWrite() const
{
    return m_editedSinceWrite;
}

void ScheduleEditHistory::setEditedSinceWrite(bool to)
{
    m_editedSinceWrite = to;
}

void ScheduleEditHistory::addEdit(ScheduleEdit* edit)
{
    std::cout << "Added Schedule Edit of type: " << edit->getType() << std::endl;
    removeFollowingEditHistory();
    m_editHistory.push_back(edit);
    m_editHistoryIndex = m_editHistory.size() - 1;
}

void ScheduleEditHistory::removeFollowingEditHistory()
{
    if (m_editHistory.size() == 0) { return; }

    for (int i = m_editHistory.size() - 1; i > m_editHistoryIndex; i--)
    {
        delete m_editHistory[i];
        m_editHistory.pop_back();
    }
}

void ScheduleEditHistory::undo()
{
    if (m_editHistory.size() == 0 || m_editHistoryIndex == 0 && m_editHistory[m_editHistoryIndex]->getIsReverted()) { return; }

    ScheduleEdit* edit = m_editHistory[m_editHistoryIndex];
    edit->revert(m_core); 
    if (m_editHistoryIndex > 0) { m_editHistoryIndex--; }
}

void ScheduleEditHistory::redo()
{
    if (m_editHistory.size() == 0 || m_editHistoryIndex == m_editHistory.size() - 1 && m_editHistory[m_editHistoryIndex]->getIsReverted() == false) { return; }

    if ((m_editHistoryIndex > 0 || m_editHistory[0]->getIsReverted() == false) && m_editHistoryIndex < m_editHistory.size() - 1) { m_editHistoryIndex++; }
    ScheduleEdit* edit = m_editHistory[m_editHistoryIndex];
    edit->apply(m_core);
}
