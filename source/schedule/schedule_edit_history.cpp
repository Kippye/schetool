#include <schedule_edit_history.h>
#include <iostream>

ScheduleEditHistory::ScheduleEditHistory(ScheduleCore& scheduleCore) : m_core(scheduleCore) {
}

const std::deque<std::shared_ptr<ScheduleEdit>>& ScheduleEditHistory::getEditHistory() const {
    return m_editHistory;
}

size_t ScheduleEditHistory::getEditHistoryIndex() const {
    return m_editHistoryIndex;
}

void ScheduleEditHistory::clearEditHistory() {
    if (m_editHistory.size() == 0) {
        return;
    }

    m_editHistory.clear();
}

bool ScheduleEditHistory::getEditedSinceWrite() const {
    return m_editedSinceWrite;
}

void ScheduleEditHistory::setEditedSinceWrite(bool to) {
    m_editedSinceWrite = to;
}

void ScheduleEditHistory::addEdit(std::shared_ptr<ScheduleEdit> edit) {
    removeFollowingEditHistory();
    m_editHistory.push_back(edit);
    m_editHistoryIndex = m_editHistory.size() - 1;
    setEditedSinceWrite(true);
}

void ScheduleEditHistory::removeFollowingEditHistory() {
    // added fix: if there is only one edit and it has been undone and another edit is added, that edit should be removed to avoid unwanted behavior
    if (m_editHistory.size() == 1 && m_editHistory.at(0)->getIsReverted() == true) {
        m_editHistory.pop_back();
        return;
    }
    if (m_editHistory.size() == 0) {
        return;
    }

    for (int i = m_editHistory.size() - 1; i > m_editHistoryIndex; i--) {
        m_editHistory.pop_back();
    }
}

bool ScheduleEditHistory::undo() {
    if (m_editHistory.size() == 0 || m_editHistoryIndex == 0 && m_editHistory[m_editHistoryIndex]->getIsReverted()) {
        return false;
    }

    std::shared_ptr<ScheduleEdit> edit = m_editHistory[m_editHistoryIndex];
    edit->revert(m_core);
    if (m_editHistoryIndex > 0) {
        m_editHistoryIndex--;
    }
    return true;
}

bool ScheduleEditHistory::redo() {
    if (m_editHistory.size() == 0 ||
        m_editHistoryIndex == m_editHistory.size() - 1 && m_editHistory[m_editHistoryIndex]->getIsReverted() == false)
    {
        return false;
    }

    // increase history index if (not 0th edit or 0th edit is reverted) AND there are later edits to redo
    if ((m_editHistoryIndex > 0 || m_editHistory[0]->getIsReverted() == false) && m_editHistoryIndex < m_editHistory.size() - 1)
    {
        m_editHistoryIndex++;
    }
    std::shared_ptr<ScheduleEdit> edit = m_editHistory[m_editHistoryIndex];
    edit->apply(m_core);
    return true;
}
