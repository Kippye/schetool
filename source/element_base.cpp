#include <element_base.h>
#include <iostream>
#include <util.h>

DateContainer::DateContainer() {}
DateContainer::DateContainer(const tm& t)
{
    time = t;
}

std::string DateContainer::getString() const
{
    char output[1024];

    std::strftime(output, sizeof(output), "%d/%m/%y", &time);

    return std::string(output);
}

const tm* DateContainer::getTime() const
{
    return &time;
}

void DateContainer::setTime(const tm& time)
{
    this->time = tm(time);
}

void DateContainer::setMonthDay(unsigned int day)
{
    time.tm_mday = day;
}
// NOTE: If the given month is < 0, it will be set to 11. If it's > 11, it will be set to 0
void DateContainer::setMonth(int month)
{
    time.tm_mon = month < 0 ? 11 : (month > 11 ? 0 : month);
}
// NOTE: hasBeenSubtracted is used to determine which range the year should be limited to (if 1900 has already been subtracted from the year)
// subtractTmBaseYear - if this is true, then 1900 will be subtracted from the year before doing other validation
void DateContainer::setYear(int year, bool convert)
{
    year = convertToValidYear(year, !convert, convert);
    time.tm_year = year;
}

// Helper function that converts any unsigned integer to a year usable in the tm struct
int DateContainer::convertToValidYear(int year, bool hasBeenSubtracted, bool subtractTmBaseYear)
{
    // if subtractTmBaseYear = false just clamp, return
    // if subtactTmBaseYear = true, subtract, then clamp to diff range

    if (subtractTmBaseYear)
    {
        year -= 1900;
    }

    if (hasBeenSubtracted)
    {
        return std::max(std::min(year, 8000), -1900);
    }
    else
    {
        return std::max(std::min(year, 8000 + 1900), 0);
    }
}


TimeContainer::TimeContainer() {}
TimeContainer::TimeContainer(int h, int m)
{
    hours = h;
    minutes = m;
}
TimeContainer::TimeContainer(const tm& t)
{
    hours = t.tm_hour;
    minutes = t.tm_min; 
}

std::string TimeContainer::getString() const
{
    char output[1024];

    tm outputTime;
    outputTime.tm_hour = hours;
    outputTime.tm_min = minutes;
    std::strftime(output, sizeof(output), "%H:%M", &outputTime);

    return std::string(output);
}

int TimeContainer::getHours() const 
{
    return hours;
}

int TimeContainer::getMinutes() const
{
    return minutes;
}

void TimeContainer::setTime(int hours, int minutes)
{
    this->hours = hours;
    this->minutes = minutes;
}


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
    if (m_options.size() == SELECT_OPTION_COUNT_MAX) { return; }
    
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


SelectContainer::SelectContainer()
{

}

const std::set<size_t>& SelectContainer::getSelection() const
{
    return m_selection;
}

void SelectContainer::setSelected(size_t index, bool select)
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

void SelectContainer::replaceSelection(const std::set<size_t>& selection)
{
    m_selection = selection;
}

// Update the SelectContainer to recorrect its indices after a modification to the attached SelectOptions
void SelectContainer::update()
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


ElementBase::ElementBase()
{

} 

ElementBase::ElementBase(SCHEDULE_TYPE type, const DateContainer& creationDate, const TimeContainer& creationTime)
{
    m_type = type;
    m_creationDate = creationDate;
    m_creationTime = creationTime;
    m_editDate = creationDate;
    m_editTime = creationTime;
}

SCHEDULE_TYPE ElementBase::getType() const { return m_type; }
const DateContainer& ElementBase::getCreationDate() const { return m_creationDate; }
const TimeContainer& ElementBase::getCreationTime() const { return m_creationTime; }
const DateContainer& ElementBase::getEditDate() const { return m_editDate; }
const TimeContainer& ElementBase::getEditTime() const { return m_editTime; }

std::string ElementBase::getString() const
{
    return "This a base :/";
}