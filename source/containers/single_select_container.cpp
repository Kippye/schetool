#include "single_select_container.h"
#include <stdexcept>
#include <format>
#include <iostream>

std::optional<size_t> SingleSelectContainer::getSelection() const {
    if (m_selection.size() == 0) {
        return std::nullopt;
    } else {
        return *m_selection.begin();
    }
}

void SingleSelectContainer::replaceSelection(const std::optional<size_t>& selection) {
    // Clear the selection to empty it or replace it
    m_selection.clear();
    if (selection.has_value())  // Replace with the contained index
    {
        m_selection.insert(selection.value());
    }
}

void SingleSelectContainer::setSelected(size_t index, bool select) {
    if (index > m_optionCount - 1) {
        throw std::out_of_range(std::format("SingleSelectContainer::setSelected({}, {}): Tried to change selection of an option that does not exist", index, select));
    }

    auto indexInSelection = m_selection.find(index);
    // already in selection
    if (indexInSelection != m_selection.end()) {
        if (select == false) {
            m_selection.erase(indexInSelection);
        }
    }
    // not in selection yet
    else
    {
        if (select == true) {
            m_selection.clear();
            m_selection.insert(index);
        }
    }
}

bool SingleSelectContainer::contains(const SingleSelectContainer& other) const {
    if (other.getSelection().has_value() && m_selection.find(other.getSelection().value()) == m_selection.end()) {
        return false;
    }
    return true;
}