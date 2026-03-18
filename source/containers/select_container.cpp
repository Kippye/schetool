#include "select_container.h"
#include <stdexcept>
#include <format>

const std::set<size_t> SelectContainer::getSelection() const {
    return m_selection;
}

void SelectContainer::setSelected(size_t index, bool select) {
    if (index > m_optionCount - 1) {
        throw std::out_of_range(std::format(
            "SelectContainer::setSelected({}, {}): Tried to change selection of an option that does not exist", index, select));
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
            m_selection.insert(index);
        }
    }
}

void SelectContainer::replaceSelection(const std::set<size_t>& selection) {
    // TODO: Check if the new selection is valid
    m_selection = selection;
}

// Update the SelectContainer to recorrect its indices after a modification to the attached SelectOptions
void SelectContainer::update(const SelectOptionUpdateInfo& lastChange, size_t optionCount) {
    m_optionCount = optionCount;

    // NOTE: Here, we assume that the required indices HAVE values.
    // The SelectOptions or whatever is updating this SelectContainer must correctly fill these values.
    switch (lastChange.type) {
        case (OPTION_MODIFICATION_COUNT_UPDATE): {
            return;
        }
        case (OPTION_MODIFICATION_ADD): {
            // Added at end, we can chill
            if (lastChange.firstIndex.value_or(optionCount - 1) == optionCount - 1) {
                return;
            }
            // Added in the middle, we need to do some moving
            for (int i = optionCount - 1; i >= (int)lastChange.firstIndex.value(); i--) {
                if (m_selection.find(i) != m_selection.end()) {
                    m_selection.erase(i);
                    m_selection.insert(i + 1);
                }
            }
            break;
        }
        // An option was removed. Reduce all indices after the removed by 1
        case (OPTION_MODIFICATION_REMOVE): {
            if (m_selection.find(lastChange.firstIndex.value()) != m_selection.end()) {
                m_selection.erase(lastChange.firstIndex.value());
            }

            for (size_t i = lastChange.firstIndex.value() + 1; i < optionCount + 1; i++) {
                if (m_selection.find(i) != m_selection.end()) {
                    m_selection.erase(i);
                    m_selection.insert(i - 1);
                }
            }
            break;
        }
        // An option was moved from one index to another.
        case (OPTION_MODIFICATION_MOVE): {
            bool addSecondIndex = false;

            if (m_selection.find(lastChange.firstIndex.value()) != m_selection.end()) {
                m_selection.erase(lastChange.firstIndex.value());
                addSecondIndex = true;
            }

            // If firstIndex > secondIndex, add 1 to every index between firstIndex (excluded) and secondIndex (included).
            if (lastChange.firstIndex.value() > lastChange.secondIndex.value()) {
                for (size_t i = lastChange.secondIndex.value(); i < lastChange.firstIndex.value(); i++) {
                    if (m_selection.find(i) != m_selection.end()) {
                        m_selection.erase(i);
                        m_selection.insert(i + 1);
                    }
                }
            }
            // If firstIndex < secondIndex, subtract 1 from every index between firstIndex (excluded) and lastIndex (included).
            else if (lastChange.firstIndex.value() < lastChange.secondIndex.value())
            {
                for (size_t i = lastChange.secondIndex.value(); i > lastChange.firstIndex.value(); i--) {
                    if (m_selection.find(i) != m_selection.end()) {
                        m_selection.erase(i);
                        m_selection.insert(i - 1);
                    }
                }
            }

            // add the index that the (selected) option was moved to
            if (addSecondIndex) {
                m_selection.insert(lastChange.secondIndex.value());
            }
            break;
        }
        case (OPTION_MODIFICATION_RENAME): {
            // The container only cares about the index of the option, which has not changed.
            break;
        }
        case (OPTION_MODIFICATION_RECOLOR): {
            // The container only cares about the index of the option, which has not changed.
            break;
        }
        case (OPTION_MODIFICATION_REPLACE): {
            // TODO HOW??
            // Remove selection indices that are now out of range
            std::erase_if(m_selection, [this](size_t i) { return i >= m_optionCount; });
            break;
        }
        case (OPTION_MODIFICATION_CLEAR): {
            m_selection.clear();
            break;
        }
    }
}

bool SelectContainer::contains(const SelectContainer& other) const {
    for (size_t otherSelected : other.getSelection()) {
        if (m_selection.find(otherSelected) == m_selection.end()) {
            return false;
        }
    }
    return true;
}