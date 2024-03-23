#include <algorithm>
#include <cctype>
#include <ctime>
#include <iterator>
#include <string.h>
#include <select_container.h>
#include <iostream>
#include <vector>
#include <util.h>

void SelectOptionChange::replace(SELECT_MODIFICATION type, size_t firstIndex, size_t secondIndex)
{
    this->type = type;
    this->firstIndex = firstIndex;
    this->secondIndex = secondIndex;
    applied = false;
}

SelectOptions::SelectOptions()
{

}

SelectOptions::SelectOptions(const std::vector<std::string>& options)
{
    m_options = options;
}

const std::vector<std::string>& SelectOptions::getOptions() const
{
    return m_options;
}

const SelectOptionChange& SelectOptions::getLastChange() const
{
    return m_lastModification;
}

void SelectOptions::setIsMutable(bool isMutable)
{
    m_mutable = isMutable;
}

bool SelectOptions::getIsMutable() const
{
    return m_mutable;
}

void SelectOptions::addOption(const std::string& option)
{
    m_options.push_back(option);
}

void SelectOptions::removeOption(const std::string& option)
{
    for (int i = m_options.size() - 1; i >= 0; i--)
    {
        if (m_options[i] == option)
        {
            m_lastModification.replace(SELECT_MODIFICATION_REMOVE, i, i);
            m_options.erase(m_options.begin() + i);
        }
    }
}

void SelectOptions::removeOption(size_t option)
{
    m_lastModification.replace(SELECT_MODIFICATION_REMOVE, option, option);
    m_options.erase(m_options.begin() + option);
}

void SelectOptions::moveOption(size_t firstIndex, size_t secondIndex)
{
    m_lastModification.replace(SELECT_MODIFICATION_MOVE, firstIndex, secondIndex);
    containers::move(m_options, firstIndex, secondIndex);
}

void SelectOptions::clearOptions()
{
    m_lastModification.replace(SELECT_MODIFICATION_CLEAR, 0, 0);
    m_options.clear();
}

// TEMP? idk this seems real unnecessary
void SelectOptions::modificationApplied()
{
    m_lastModification.applied = true;
}


Select::Select()
{

}

Select::Select(SelectOptions& s)
{
    m_options = &s;
}

const std::set<size_t>& Select::getSelection()
{
    return m_selection;
}

void Select::setSelected(size_t index, bool select)
{
    if (index > m_options->getOptions().size() - 1)
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

// Update the Select to recorrect its indices after a modification to the attached SelectOptions
void Select::update()
{
    const SelectOptionChange& lastChange = m_options->getLastChange();

    if (lastChange.applied) { return; }

    switch(lastChange.type)
    {
        // An option was removed. Reduce all indices after the removed by 1
        case (SELECT_MODIFICATION_REMOVE):
        {
            m_selection.erase(lastChange.firstIndex);

            for (size_t i = lastChange.firstIndex + 1; i < m_options->getOptions().size() + 1; i++)
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
        case (SELECT_MODIFICATION_MOVE):
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
        case (SELECT_MODIFICATION_CLEAR):
        {
            m_selection.clear();
            break;
        }
    }
}