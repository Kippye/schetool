#include <schedule_core.h>

#include "element.h"
#include "element_base.h"
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <string.h>
#include <vector>
#include <ctime>
#include <numeric>

// TEMP
#include <iostream>

// NOTE: If flags is ScheduleElementFlags_None, simply returns the first column it finds
size_t ScheduleCore::getFlaggedColumnIndex(ScheduleColumnFlags flags)
{
    for (size_t i = 0; i < m_schedule.size(); i++)
    {
        if (m_schedule.at(i).flags & flags)
        {
            return i;
        }
    }
    return 0;
}

// Private function, because it returns a mutable column pointer. NOTE: If flags is ScheduleElementFlags_None, simply returns the first column it finds
Column* ScheduleCore::getColumnWithFlags(ScheduleColumnFlags flags)
{
    return &m_schedule.at(getFlaggedColumnIndex(flags));
}

Column* ScheduleCore::getMutableColumn(size_t column)
{
    if (column > getColumnCount())
    {
        std::cout << "ScheduleCore::getModifiableColumn: index out of range. Returning last column" << std::endl;
        return &m_schedule.at(getColumnCount() - 1);
    }
    return &m_schedule.at(column);
}

// Sorts a copy of the column's rows. Then compares each element of the two rows vectors and returns a vector that contains which index of the OLD rows vector corresponds to that position in the NEW SORTED rows
std::vector<size_t> ScheduleCore::getColumnSortedNewIndices(size_t index)
{
    Column& column = m_schedule.at(index);
    std::vector<ElementBase*> rows = column.rows;
    std::vector<size_t> newIndices(rows.size());
    std::iota(newIndices.begin(), newIndices.end(), 0);
   
    m_columnSortComparison.setup(column.type, column.sort);
    std::sort(newIndices.begin(), newIndices.end(), [&](size_t i, size_t j){ return m_columnSortComparison(rows[i], rows[j]); });
    return newIndices;
}


void ScheduleCore::clearSchedule()
{
    m_schedule.clear();
    sortColumns();
}

void ScheduleCore::replaceSchedule(std::vector<Column>& columns)
{
    clearSchedule();

    m_schedule = columns;
    sortColumns();
}

const std::vector<Column>& ScheduleCore::getAllColumns()
{
    return m_schedule;
}

std::vector<Column>& ScheduleCore::getAllColumnsMutable()
{
    return m_schedule;
}
  
// Sorts every column's rows based on "sorter" columns
void ScheduleCore::sortColumns()
{
    for (size_t sorterColumn = 0; sorterColumn < getColumnCount(); sorterColumn++)
    {
        if (m_schedule.at(sorterColumn).sort != COLUMN_SORT_NONE)
        {
            m_sortedRowIndices = getColumnSortedNewIndices(sorterColumn);
            break;
        }
        // Last column is unsorted too which means we should not sort
        else if (sorterColumn == getColumnCount() - 1)
        {
            m_sortedRowIndices.resize(getRowCount());
            std::iota(m_sortedRowIndices.begin(), m_sortedRowIndices.end(), 0);
        }
    }
}


size_t ScheduleCore::getColumnCount()
{
    return m_schedule.size();
}

// Add a column from previous data. NOTE: Creates copies of all passed values, because this will probably mostly be used for duplicating columns
// TODO: FIX SELECTS PROBABLY
void ScheduleCore::addColumn(size_t index, const Column& column)
{
    std::cout << "Adding column: " << column.name << std::endl;

    // TODO: make sure that EVERY column has the same amount of rows!!!
    // TODO: give the new column correct creation date & time

    // the last index = just add to the end
    if (index == getColumnCount())
    {
        m_schedule.push_back(column);
    }
    else
    {
        m_schedule.insert(m_schedule.begin() + index, column);
    }

    // Sort columns just in case, because the added Column could have a sort other than COLUMN_SORT_NONE
    sortColumns();
}

void ScheduleCore::addDefaultColumn(size_t index)
{
    time_t t = std::time(nullptr);
    tm creationTime = *std::localtime(&t);

    Column addedColumn = Column(std::vector<ElementBase*>{}, SCH_TEXT, std::string("Text"), false);

    // the last index = just add to the end
    if (index == getColumnCount())
    {
        for (size_t i = 0; i < getRowCount(); i++)
        {
            addedColumn.rows.push_back((ElementBase*)new Element<std::string>(SCH_TEXT, std::string(""), DateContainer(creationTime), TimeContainer(creationTime)));
        }
        m_schedule.push_back(addedColumn);
    }
    else
    {
        for (size_t i = 0; i < getRowCount(); i++)
        {
            addedColumn.rows.push_back((ElementBase*)new Element<std::string>(SCH_TEXT, std::string(""), DateContainer(creationTime), TimeContainer(creationTime)));
        }
        m_schedule.insert(m_schedule.begin() + index, addedColumn);
    }

    // I think default columns don't cause a need for sorting, since their sort is always COLUMN_SORT_NONE
}

void ScheduleCore::removeColumn(size_t column)
{
    // a permanent column can't be removed
    if (m_schedule.at(column).permanent == true) { return; }

    bool resortRequired = m_schedule.at(column).sort != COLUMN_SORT_NONE;

    // the last index = pop from end
    if (column == getColumnCount() - 1)
    {
        m_schedule.pop_back();
    }
    else
    {
        m_schedule.erase(m_schedule.begin() + column); // invalidates pointers to Columns past this one
    }

    if (resortRequired)
    {
        sortColumns();
    }
}

const Column* ScheduleCore::getColumn(size_t column)
{
    return (const Column*)getMutableColumn(column);
}

void ScheduleCore::setColumnType(size_t column, SCHEDULE_TYPE type)
{
    //if (type == m_schedule[column].type) { return; }

    // for adding to edit history
    Column previousData = Column(m_schedule.at(column));

    if (type == SCH_SELECT)
    {
        m_schedule.at(column).selectOptions.setIsMutable(true);
    }
    // setup for weekday type
    else if (type == SCH_WEEKDAY)
    {
        m_schedule.at(column).selectOptions.clearOptions();
        m_schedule.at(column).selectOptions.addOption(std::string("Monday"));
        m_schedule.at(column).selectOptions.addOption(std::string("Tuesday"));
        m_schedule.at(column).selectOptions.addOption(std::string("Wednesday"));
        m_schedule.at(column).selectOptions.addOption(std::string("Thursday"));
        m_schedule.at(column).selectOptions.addOption(std::string("Friday"));
        m_schedule.at(column).selectOptions.addOption(std::string("Saturday"));
        m_schedule.at(column).selectOptions.addOption(std::string("Sunday"));
        // TODO: set up colours as well
        m_schedule.at(column).selectOptions.setIsMutable(false);
        // IMPORTANT!
        type = SCH_SELECT;
    }

    // TODO: try to convert types..? i guess there's no point in doing that. only really numbers could be turned into text.
    // reset values to defaults of the (new?) type
    resetColumn(column, type);
    // read resetColumn description for why this is run after
    m_schedule.at(column).type = type;
}

void ScheduleCore::setColumnName(size_t column, const char* name)
{
    Column previousData = Column(m_schedule.at(column));

    m_schedule.at(column).name = std::string(name);
}

void ScheduleCore::setColumnSort(size_t column, COLUMN_SORT sortDirection)
{
    Column previousData = Column(m_schedule.at(column));

    m_schedule.at(column).sort = sortDirection;
    sortColumns();
}

const SelectOptions& ScheduleCore::getColumnSelectOptions(size_t column)
{
    return m_schedule.at(column).selectOptions;
}

void ScheduleCore::modifyColumnSelectOptions(size_t column, OPTION_MODIFICATION selectModification, size_t firstIndex, size_t secondIndex, const std::vector<std::string>& optionNames)
{
    Column previousData = Column(m_schedule.at(column));

    switch(selectModification)
    {
        case(OPTION_MODIFICATION_ADD):
        { 
            if (optionNames.size() == 0)
            {
                std::cout << "ScheduleCore::modifyColumnSelectOptions: Failed to add Select option because no name was provided in the arguments" << std::endl;
                return;
            }
            m_schedule.at(column).selectOptions.addOption(optionNames[0]);
            break;
        }
        case(OPTION_MODIFICATION_REMOVE):
        { 
            m_schedule.at(column).selectOptions.removeOption(firstIndex);
            break;
        }
        case(OPTION_MODIFICATION_MOVE):
        { 
            m_schedule.at(column).selectOptions.moveOption(firstIndex, secondIndex);
            break;
        }
        case(OPTION_MODIFICATION_CLEAR):
        { 
            m_schedule.at(column).selectOptions.clearOptions();
            break;
        }
        case(OPTION_MODIFICATION_REPLACE):
        { 
            m_schedule.at(column).selectOptions.replaceOptions(optionNames);
            break;
        }
    }

    sortColumns();
}

void ScheduleCore::resetColumn(size_t index, SCHEDULE_TYPE type)
{
    Column& column = *getMutableColumn(index);
    size_t rowCount = column.rows.size();

    time_t t = std::time(nullptr);
    tm creationTime = *std::localtime(&t);

    switch(type) 
    {
        case(SCH_BOOL):
        {
            for (size_t row = 0; row < rowCount; row++)
            {
                setElement(index, row, (ElementBase*)new Element<bool>(type, false, DateContainer(creationTime), TimeContainer(creationTime)), false);
            }
            break;
        }
        case(SCH_NUMBER):
        {
            for (size_t row = 0; row < rowCount; row++)
            {
                setElement(index, row, (ElementBase*)new Element<int>(type, 0, DateContainer(creationTime), TimeContainer(creationTime)), false);
            }
            break;
        }
        case(SCH_DECIMAL):
        {
            for (size_t row = 0; row < rowCount; row++)
            {
                setElement(index, row, (ElementBase*)new Element<double>(type, 0, DateContainer(creationTime), TimeContainer(creationTime)), false);
            }  
            break;
        }
        case(SCH_TEXT):
        {
            for (size_t row = 0; row < rowCount; row++)
            {
                setElement(index, row, (ElementBase*)new Element<std::string>(type, "", DateContainer(creationTime), TimeContainer(creationTime)), false);
            }     
            break;
        }
        case(SCH_SELECT):
        {
            for (size_t row = 0; row < rowCount; row++)
            {
                if (column.selectOptions.getIsMutable() == true)
                {
                    column.selectOptions.clearOptions();
                }
                auto selectElement = new Element<SelectContainer>(type, SelectContainer(column.selectOptions), DateContainer(creationTime), TimeContainer(creationTime));
                selectElement->getValueReference().listenToCallback();
                setElement(index, row, (ElementBase*)selectElement, false);
            }     
            break;
        }
        case(SCH_TIME):
        {
            for (size_t row = 0; row < rowCount; row++)
            {
                setElement(index, row, (ElementBase*)new Element<TimeContainer>(type, TimeContainer(0, 0), DateContainer(creationTime), TimeContainer(creationTime)), false);
            }
            break;
        }   
        case(SCH_DATE):
        {
            for (size_t row = 0; row < rowCount; row++)
            {
                setElement(index, row, (ElementBase*)new Element<DateContainer>(type, DateContainer(creationTime), DateContainer(creationTime), TimeContainer(creationTime)), false);
            }
            break;
        }
        default:
        {
            std::cout << "ScheduleCore::resetColumn: Resetting a column to type: " << type << " has not been implemented!" << std::endl;
            break;
        }
    }

    // NOTE: For performance's sake, the individual elements being reset do not resort, so we call it once here
    sortColumns();
}


// Return the number of rows in the schedule or 0 if there are no columns (which probably won't happen?) 
size_t ScheduleCore::getRowCount()
{
    return (m_schedule.size() > 0 ? m_schedule.at(0).rows.size() : 0);
}

void ScheduleCore::addRow(size_t index)
{
    std::vector<ElementBase*> elementCopies = {};

    // the last index = just add to the end
    if (index == getRowCount())
    {
        for (Column& column : m_schedule)
        {
            std::vector<ElementBase*>& columnValues = column.rows;
            time_t t = std::time(nullptr);
            tm creationTime = *std::localtime(&t);

            switch(column.type)
            {
                case(SCH_BOOL):
                {
                    columnValues.push_back(new Element<bool>(column.type, false, DateContainer(creationTime), TimeContainer(creationTime)));
                    break;
                }
                case(SCH_NUMBER):
                {
                    columnValues.push_back(new Element<int>(column.type, 0, DateContainer(creationTime), TimeContainer(creationTime)));
                    break;
                }
                case(SCH_DECIMAL):
                {
                    columnValues.push_back(new Element<double>(column.type, 0, DateContainer(creationTime), TimeContainer(creationTime)));
                    break;
                }
                case(SCH_TEXT):
                {
                    columnValues.push_back(new Element<std::string>(column.type, "", DateContainer(creationTime), TimeContainer(creationTime)));
                    break;
                }
                case(SCH_SELECT):
                { 
                    columnValues.push_back(new Element<SelectContainer>(column.type, SelectContainer(column.selectOptions), DateContainer(creationTime), TimeContainer(creationTime)));      
                    ((Element<SelectContainer>*)columnValues.back())->getValueReference().listenToCallback();
                    break;
                }
                case(SCH_TIME):
                { 
                    columnValues.push_back(new Element<TimeContainer>(column.type, TimeContainer(0, 0), DateContainer(creationTime), TimeContainer(creationTime)));      
                    break;
                }
                case(SCH_DATE):
                { 
                    columnValues.push_back(new Element<DateContainer>(column.type, DateContainer(creationTime), DateContainer(creationTime), TimeContainer(creationTime)));      
                    break;
                }
                default:
                {
                    std::cout << "ScheduleCore::addRow: Adding rows of type: " << column.type << " has not been implemented!" << std::endl;
                    break;
                }
            }
        }
    }
    else
    {
        for (Column& column : m_schedule)
        {
            std::vector<ElementBase*>& columnValues = column.rows;
            time_t t = std::time(nullptr);
            tm creationTime = *std::localtime(&t);

            switch(column.type)
            {
                case(SCH_BOOL):
                {
                    columnValues.insert(columnValues.begin() + index, new Element<bool>(column.type, false, DateContainer(creationTime), TimeContainer(creationTime)));
                    break;
                }
                case(SCH_NUMBER):
                {
                    columnValues.insert(columnValues.begin() + index, new Element<int>(column.type, 0, DateContainer(creationTime), TimeContainer(creationTime)));
                    break;
                }
                case(SCH_DECIMAL):
                {
                    columnValues.insert(columnValues.begin() + index, new Element<double>(column.type, 0, DateContainer(creationTime), TimeContainer(creationTime)));
                    break;
                }
                case(SCH_TEXT):
                {
                    columnValues.insert(columnValues.begin() + index, new Element<std::string>(column.type, "", DateContainer(creationTime), TimeContainer(creationTime)));
                    break;
                }
                case(SCH_SELECT):
                { 
                    columnValues.insert(columnValues.begin() + index, new Element<SelectContainer>(column.type, SelectContainer(column.selectOptions), DateContainer(creationTime), TimeContainer(creationTime)));      
                    ((Element<SelectContainer>*)columnValues.at(index))->getValueReference().listenToCallback();
                    break;
                }
                case(SCH_TIME):
                { 
                    columnValues.insert(columnValues.begin() + index, new Element<TimeContainer>(column.type, TimeContainer(0, 0), DateContainer(creationTime), TimeContainer(creationTime)));      
                    break;
                }
                case(SCH_DATE):
                { 
                    columnValues.insert(columnValues.begin() + index, new Element<DateContainer>(column.type, DateContainer(creationTime), DateContainer(creationTime), TimeContainer(creationTime)));      
                    break;
                }
                default:
                {
                    std::cout << "ScheduleCore::addRow: Adding rows of type: " << column.type << " has not been implemented!" << std::endl;
                    break;
                }
            }
        }
    }

    sortColumns();
}

void ScheduleCore::removeRow(size_t index)
{
    if (getColumn(0)->rows.size() - 1 < index) { std::cout << "ScheduleCore::removeRow: No row found at index: " << index << std::endl; return; }

    std::vector<ElementBase*> elementCopies = {};

    for (Column& column : m_schedule)
    {
        std::vector<ElementBase*>& columnValues = column.rows;

        if (index == columnValues.size() - 1)
        {
            delete columnValues[index];
            columnValues.pop_back();
        }
        else
        {
            delete columnValues[index];
            columnValues.erase(columnValues.begin() + index);
        }
    }

    sortColumns();
}

std::vector<ElementBase*> ScheduleCore::getRow(size_t index)
{
    std::vector<ElementBase*> elementData = {};

    if (getColumn(0)->rows.size() - 1 < index) { std::cout << "ScheduleCore::getRow: No row found at index: " << index << std::endl; return elementData; }

    for (size_t col = 0; col < getColumnCount(); col++)
    {
        elementData.push_back(getElement(col, index));
    }

    return elementData;
}

void ScheduleCore::setRow(size_t index, std::vector<ElementBase*> elementData)
{
    if (getColumn(0)->rows.size() - 1 < index) { std::cout << "ScheduleCore::setRow: No row found at index: " << index << std::endl; return; }

    for (size_t col = 0; col < getColumnCount(); col++)
    {
        //std::cout << col << ": " << elementData[col]->getString() << std::endl;
        setElement(col, index, elementData[col], false);
    }

    sortColumns();
}

std::vector<size_t> ScheduleCore::getSortedRowIndices()
{
    return m_sortedRowIndices;
}