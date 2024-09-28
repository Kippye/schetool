#include <select_container.h>
#include <iostream>

const std::set<size_t> SelectContainer::getSelection() const
{
    return m_selection;
}

void SelectContainer::setSelected(size_t index, bool select)
{
    if (index > m_optionCount - 1)
    {
        std::cout << "Tried to change selection of an option that does not exist" << std::endl;
        return;
    }

    auto indexInSelection = m_selection.find(index);
    // already in selection
    if (indexInSelection != m_selection.end())
    {
        if (select == false)
        {
            m_selection.erase(indexInSelection);
        }
    }
    // not in selection yet
    else
    {
        if (select == true)
        {
            m_selection.insert(index);
        }
    }
}

void SelectContainer::replaceSelection(const std::set<size_t>& selection)
{
    // TODO: Check if the new selection is valid
    m_selection = selection;
}

// Update the SelectContainer to recorrect its indices after a modification to the attached SelectOptions
void SelectContainer::update(const SelectOptionUpdateInfo& lastChange, size_t optionCount)
{
    m_optionCount = optionCount;

    switch(lastChange.type)
    {
        case(OPTION_MODIFICATION_COUNT_UPDATE):
        {
            return;
        }
        case(OPTION_MODIFICATION_ADD):
        {
            // Currently, options can only be "pushed back" so no selections are invalidated. We chillin.
            break;
        }
        // An option was removed. Reduce all indices after the removed by 1
        case (OPTION_MODIFICATION_REMOVE):
        {
            if (m_selection.find(lastChange.firstIndex) != m_selection.end()) 
            { 
                m_selection.erase(lastChange.firstIndex); 
            }

            for (size_t i = lastChange.firstIndex + 1; i < optionCount + 1; i++)
            {
                if (m_selection.find(i) != m_selection.end())
                {
                    m_selection.erase(i);
                    m_selection.insert(i - 1);
                }
            }
            break;
        }
        // An option was moved from one index to another. 
        case (OPTION_MODIFICATION_MOVE):
        {
            bool addSecondIndex = false;

            if (m_selection.find(lastChange.firstIndex) != m_selection.end())
            {
                m_selection.erase(lastChange.firstIndex);
                addSecondIndex = true;
            }

            // If firstIndex > secondIndex, add 1 to every index between firstIndex (excluded) and secondIndex (included).
            if (lastChange.firstIndex > lastChange.secondIndex)
            {
                for (size_t i = lastChange.secondIndex; i < lastChange.firstIndex; i++)
                {
                    if (m_selection.find(i) != m_selection.end())
                    {
                        m_selection.erase(i);
                        m_selection.insert(i + 1);
                    }
                }
            }
            // If firstIndex < secondIndex, subtract 1 from every index between firstIndex (excluded) and lastIndex (included).
            else if (lastChange.firstIndex < lastChange.secondIndex)
            {
                for (size_t i = lastChange.secondIndex; i > lastChange.firstIndex; i--)
                {
                    if (m_selection.find(i) != m_selection.end())
                    {
                        m_selection.erase(i);
                        m_selection.insert(i - 1);
                    }
                }
            }

            // add the index that the (selected) option was moved to
            if (addSecondIndex)
            {
                m_selection.insert(lastChange.secondIndex);
            }
            break;
        }
        case (OPTION_MODIFICATION_RENAME):
        {
            // The container only cares about the index of the option, which has not changed.
            break;
        }
        case (OPTION_MODIFICATION_RECOLOR):
        {
            // The container only cares about the index of the option, which has not changed.
            break;
        }
        case (OPTION_MODIFICATION_REPLACE):
        {
            // TODO HOW??
            break;
        }
        case (OPTION_MODIFICATION_CLEAR):
        {
            m_selection.clear();
            break;
        }
    }
}

bool SelectContainer::contains(const SelectContainer& other) const
{
    for (size_t otherSelected : other.getSelection())
    {
        if (m_selection.find(otherSelected) == m_selection.end())
        {
            return false;
        }
    }
    return true;
}