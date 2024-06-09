#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <string.h>
#include <vector>
#include <ctime>
#include <numeric>
#include <schedule_core.h>
#include <element_base.h>

// TEMP
#include <iostream>

ScheduleCore::ScheduleCore()
{

}

// NOTE: If flags is ScheduleElementFlags_None, simply returns the first column it finds
size_t ScheduleCore::getFlaggedColumnIndex(ScheduleColumnFlags flags) const
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
        std::cout << "ScheduleCore::getMutableColumn: index out of range. Returning last column" << std::endl;
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

const std::vector<Column>& ScheduleCore::getAllColumns() const
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


size_t ScheduleCore::getColumnCount() const
{
    return m_schedule.size();
}

// Check if the index is less than size. If not, a general "index out of range" error is printed
bool ScheduleCore::existsColumnAtIndex(size_t index) const
{
    if (index < getColumnCount() == false) 
    { 
        printf("ScheduleCore::existsColumnAtIndex(%zu): Index not less than size (%zu)\n", index, getColumnCount());
        return false; 
    }
    return true;
}

// Add a column from previous data. NOTE: Creates copies of all passed values, because this will probably mostly be used for duplicating columns
void ScheduleCore::addColumn(size_t index, const Column& column)
{
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

    Column* addedColumn = getMutableColumn(index);
    if (addedColumn->type == SCH_SELECT)
    {
        addedColumn->selectOptions.clearListeners();

        for (size_t row = 0; row < addedColumn->rows.size(); row++)
        {
            addedColumn->selectOptions.addListener(row, ((Element<SelectContainer>*)addedColumn->getElement(row))->getValueReference());
        }
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

bool ScheduleCore::removeColumn(size_t column)
{
    // a permanent column can't be removed
    if ((existsColumnAtIndex(column) == false || m_schedule.at(column).permanent == true)) { return false; }

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

    return true;
}

const Column* ScheduleCore::getColumn(size_t column) const
{
    if (existsColumnAtIndex(column) == false)
    {
        std::cout << "ScheduleCore::getColumn: index out of range. Returning last column" << std::endl;
        return &m_schedule.at(getColumnCount() - 1);
    }
    return &m_schedule.at(column);
}

void ScheduleCore::setColumnElements(size_t index, const Column& columnData)
{
    if (existsColumnAtIndex(index) == false) { return; }
    if (getColumn(index)->type != columnData.type) { printf("ScheduleCore::setColumnElements: The target Column and columnData types must match but are %d and %d\n", getColumn(index)->type, columnData.type); return; }

    for (size_t row = 0; row < getRowCount(); row++)
    {
        // break early if the provided columnData was shorter than the result of getColumnCount()
        if (row >= columnData.rows.size())
        {
            break;
        }

        switch(getColumn(index)->type)
        {
            case(SCH_BOOL):
            {
                setElementValue(index, row, ((Element<bool>*)columnData.rows[row])->getValue());
                break;
            }
            case(SCH_NUMBER):
            {
                setElementValue(index, row, ((Element<int>*)columnData.rows[row])->getValue());
                break;
            }
            case(SCH_DECIMAL):
            {
                setElementValue(index, row, ((Element<double>*)columnData.rows[row])->getValue());
                break;
            }
            case(SCH_TEXT):
            {
                setElementValue(index, row, ((Element<std::string>*)columnData.rows[row])->getValue());
                break;
            }
            case(SCH_SELECT):
            { 
                // pass SelectContainer by const reference to avoid unnecessary copying (it will be copied anyway i think)
                setElementValue(index, row, ((Element<SelectContainer>*)columnData.rows[row])->getConstValueReference());
                break;
            }
            case(SCH_TIME):
            { 
                setElementValue(index, row, ((Element<TimeContainer>*)columnData.rows[row])->getValue());
                break;
            }
            case(SCH_DATE):
            { 
                setElementValue(index, row, ((Element<DateContainer>*)columnData.rows[row])->getValue());
                break;
            }
            default:
            {
                std::cout << "ScheduleCore::setColumnElements: Setting an Element of type: " << getColumn(index)->type << " has not been implemented!" << std::endl;
                break;
            }
        }
    }
}

bool ScheduleCore::setColumnType(size_t column, SCHEDULE_TYPE type)
{
    if (existsColumnAtIndex(column) == false){ return false; }
    if (getColumn(column)->permanent == true) { printf("ScheduleCore::setColumnType tried to set type of a permanent Column at %zu! Quitting.\n", column); return false; }

    // HACK y
    m_schedule.at(column).selectOptions.clearListeners();
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
    // NOTE!!! must sort AFTER this because otherwise we're doing some REALLY sus things. Sorting a column that contains X type values as if it was a Y type column. REALLY bad.
    sortColumns();
    return true;
}

bool ScheduleCore::setColumnName(size_t column, const std::string& name)
{
    if (existsColumnAtIndex(column) == false) { return false; }
    Column previousData = Column(m_schedule.at(column));

    m_schedule.at(column).name = name;
    return true;
}

bool ScheduleCore::setColumnSort(size_t column, COLUMN_SORT sortDirection)
{
    if (existsColumnAtIndex(column) == false) { return false; }
    Column previousData = Column(m_schedule.at(column));

    m_schedule.at(column).sort = sortDirection;
    sortColumns();
    return true;
}

const SelectOptions& ScheduleCore::getColumnSelectOptions(size_t column) const
{
    return m_schedule.at(column).selectOptions;
}

bool ScheduleCore::modifyColumnSelectOptions(size_t column, const SelectOptionsModification& selectOptionsModification)
{
    if (existsColumnAtIndex(column) == false) { return false; }
    
    if (m_schedule.at(column).selectOptions.applyModification(selectOptionsModification) == false) { std::cout << "ScheduleCore::modifySelectOptions: Applying the following modification failed:"; std::cout << selectOptionsModification.getDataString(); return false; }

    sortColumns();
    return true;
}

bool ScheduleCore::addColumnFilter(size_t column, const std::shared_ptr<FilterBase>& filter)
{
    if (existsColumnAtIndex(column) == false) { return false; }

    getMutableColumn(column)->addFilter(filter);
    return true;
}

bool ScheduleCore::removeColumnFilter(size_t column, size_t index)
{
    if (existsColumnAtIndex(column) == false) { return false; }

    getMutableColumn(column)->removeFilter(index);
    return true;
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
                setElement(index, row, (ElementBase*)new Element<std::string>(type, std::string(""), DateContainer(creationTime), TimeContainer(creationTime)), false);
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
                auto selectElement = new Element<SelectContainer>(type, SelectContainer(), DateContainer(creationTime), TimeContainer(creationTime));
                column.selectOptions.addListener(row, selectElement->getValueReference());
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
}


// Return the number of rows in the schedule or 0 if there are no columns (which probably won't happen?) 
size_t ScheduleCore::getRowCount() const
{
    return (m_schedule.size() > 0 ? m_schedule.at(0).rows.size() : 0);
}

bool ScheduleCore::existsRowAtIndex(size_t index) const
{
    if (index < getRowCount() == false) 
    { 
        printf("ScheduleCore::existsRowAtIndex(%zu): Index not less than size (%zu)\n", index, getRowCount());
        return false; 
    }
    return true;
}

void ScheduleCore::addRow(size_t index)
{
    std::vector<ElementBase*> elementCopies = {};

    // the last index = just add to the end
    if (index == getRowCount())
    {
        for (size_t i = 0; i < getColumnCount(); i++)
        {
            Column& column = m_schedule[i];
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
                    columnValues.push_back(new Element<std::string>(column.type, std::string(""), DateContainer(creationTime), TimeContainer(creationTime)));
                    break;
                }
                case(SCH_SELECT):
                { 
                    Element<SelectContainer>* selectElement = new Element<SelectContainer>(column.type, SelectContainer(), DateContainer(creationTime), TimeContainer(creationTime));
                    columnValues.push_back(selectElement);
                    column.selectOptions.addListener(index, selectElement->getValueReference());
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
        for (size_t i = 0; i < getColumnCount(); i++)
        {
            Column& column = m_schedule[i];
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
                    columnValues.insert(columnValues.begin() + index, new Element<std::string>(column.type, std::string(""), DateContainer(creationTime), TimeContainer(creationTime)));
                    break;
                }
                case(SCH_SELECT):
                {
                    Element<SelectContainer>* selectElement = new Element<SelectContainer>(column.type, SelectContainer(), DateContainer(creationTime), TimeContainer(creationTime));
                    columnValues.insert(columnValues.begin() + index, selectElement);
                    column.selectOptions.addListener(index, selectElement->getValueReference());
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

bool ScheduleCore::removeRow(size_t row)
{
    if (existsRowAtIndex(row) == false) { return false; }

    for (size_t i = 0; i < m_schedule.size(); i++)
    {
        delete m_schedule[i].rows[row];
        if (row == m_schedule[i].rows.size() - 1)
        {
            m_schedule[i].rows.pop_back();
        }
        else
        {
            m_schedule[i].rows.erase(m_schedule[i].rows.begin() + row);
        }
        // for Select Columns, remove the Element from SelectOptions listeners
        if (m_schedule[i].type == SCH_SELECT)
        {
            m_schedule[i].selectOptions.removeListener(row);
        }
    }

    sortColumns();
    return true;
}

std::vector<ElementBase*> ScheduleCore::getRow(size_t index)
{
    std::vector<ElementBase*> elementData = {};

    if (existsRowAtIndex(index) == false) { return elementData; }

    for (size_t col = 0; col < getColumnCount(); col++)
    {
        elementData.push_back(getElement(col, index));
    }

    return elementData;
}

bool ScheduleCore::setRow(size_t index, std::vector<ElementBase*> elementData)
{
    if (existsRowAtIndex(index) == false) { return false; }

    for (size_t col = 0; col < getColumnCount(); col++)
    {
        setElement(col, index, elementData[col], false);
    }

    sortColumns();
    return true;
}

std::vector<size_t> ScheduleCore::getSortedRowIndices() const
{
    return m_sortedRowIndices;
}