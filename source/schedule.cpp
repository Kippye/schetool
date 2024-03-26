#include <cstdio>
#include <cstdlib>
#include <schedule.h>
#include <string.h>
#include <time.h>
#include <vector>
#include <any>
#include <ctime>
#include <numeric>

// TEMP
#include <iostream>

void Schedule::test_setup()
{
    addColumn(getColumnCount(), Column{std::vector<Element*>{}, SCH_TEXT, std::string("Name"), true, ScheduleElementFlags_Name});
    addColumn(getColumnCount(), Column{std::vector<Element*>{}, SCH_BOOL, std::string("Finished"), true, ScheduleElementFlags_Finished});
    addColumn(getColumnCount(), Column{std::vector<Element*>{}, SCH_TIME, std::string("Start"), true, ScheduleElementFlags_Start});
    addColumn(getColumnCount(), Column{std::vector<Element*>{}, SCH_TIME, std::string("Duration"), true, ScheduleElementFlags_Duration});
    addColumn(getColumnCount(), Column{std::vector<Element*>{}, SCH_TIME, std::string("End"), true, ScheduleElementFlags_End});
    addColumn(getColumnCount(), Column{std::vector<Element*>{}, SCH_DATE, std::string("Date"), false});
}

// Private function, because it returns a mutable column pointer. NOTE: If flags is ScheduleElementFlags_None, simply returns the first column it finds
Column* Schedule::getColumnWithFlags(ScheduleElementFlags flags)
{
    for (Column& column: m_schedule)
    {
        if (column.flags & flags)
        {
            return &column;
        }
    }
    return nullptr;
}

Column* Schedule::getMutableColumn(size_t column)
{
    if (column > getColumnCount())
    {
        std::cout << "Schedule::getModifiableColumn: index out of range. Returning last column" << std::endl;
        return &m_schedule[getColumnCount() - 1];
    }
    return &m_schedule[column];
}

// Sorts a copy of the column's rows. Then compares each element of the two rows vectors and returns a vector that contains which index of the OLD rows vector corresponds to that position in the NEW SORTED rows
std::vector<size_t> Schedule::getColumnSortedNewIndices(size_t index)
{
    Column& column = m_schedule[index];
    std::vector<Element*> rows = column.rows;
    std::vector<size_t> newIndices(rows.size());
    std::iota(newIndices.begin(), newIndices.end(), 0);
   
    m_columnSortComparison.setup(column.type, column.sort);
    std::sort(newIndices.begin(), newIndices.end(), [&](size_t i, size_t j){ return m_columnSortComparison(rows[i], rows[j]); });
    return newIndices;
}

const Column* Schedule::getColumn(size_t column)
{
    return (const Column*)getMutableColumn(column);
}

const std::vector<Column>& Schedule::getColumns()
{
    return m_schedule;
}

std::vector<Column>& Schedule::getMutableColumns()
{
    return m_schedule;
}

SelectOptions& Schedule::getColumnSelectOptions(size_t column)
{
    return m_schedule[column].selectOptions;
}

void Schedule::setColumnType(size_t column, SCHEDULE_TYPE type)
{
    //if (type == m_schedule[column].type) { return; }

    if (type == SCH_SELECT)
    {
        m_schedule[column].selectOptions.setIsMutable(true);
    }
    // setup for weekday type
    else if (type == SCH_WEEKDAY)
    {
        m_schedule[column].selectOptions.clearOptions();
        m_schedule[column].selectOptions.addOption(std::string("Monday"));
        m_schedule[column].selectOptions.addOption(std::string("Tuesday"));
        m_schedule[column].selectOptions.addOption(std::string("Wednesday"));
        m_schedule[column].selectOptions.addOption(std::string("Thursday"));
        m_schedule[column].selectOptions.addOption(std::string("Friday"));
        m_schedule[column].selectOptions.addOption(std::string("Saturday"));
        m_schedule[column].selectOptions.addOption(std::string("Sunday"));
        // TODO: set up colours as well
        m_schedule[column].selectOptions.setIsMutable(false);
        // IMPORTANT!
        type = SCH_SELECT;
    }

    // TODO: try to convert types..? i guess there's no point in doing that. only really numbers could be turned into text.
    // reset values to defaults of the (new?) type
    resetColumn(column, type);
    // read resetColumn description for why this is run after
    m_schedule[column].type = type;
}

void Schedule::setColumnName(size_t column, const char* name)
{
    m_schedule[column].name = std::string(name);
}

void Schedule::setColumnSort(size_t column, COLUMN_SORT sortDirection)
{
    m_schedule[column].sort = sortDirection;
    if (sortDirection != COLUMN_SORT_NONE)
    {
        m_schedule[column].sorted = false;
        sortColumns();
    }
}

size_t Schedule::getColumnCount()
{
    return m_schedule.size();
}

// Return the number of rows in the schedule or 0 if there are no columns (which probably won't happen?) 
size_t Schedule::getRowCount()
{
    return (m_schedule.size() > 0 ? m_schedule[0].rows.size() : 0);
}

// Sorts every column's rows based on "sorter" columns
void Schedule::sortColumns()
{
    for (size_t sorterColumn = 0; sorterColumn < getColumnCount(); sorterColumn++)
    {
        if (m_schedule[sorterColumn].sort != COLUMN_SORT_NONE)
        {
            std::vector<size_t> newRowIndices = getColumnSortedNewIndices(sorterColumn);

            // actually sort the rows vector in every column
            for (size_t affectedColumn = 0; affectedColumn < getColumnCount(); affectedColumn++)
            {
                // vector that will replace the old rows vector
                std::vector<Element*> sortedRows = {};
                std::vector<Element*>& unsortedRows = m_schedule[affectedColumn].rows;
                sortedRows.reserve(unsortedRows.size());

                for (size_t rowIndex = 0; rowIndex < newRowIndices.size(); rowIndex++)
                {
                    sortedRows.push_back(unsortedRows[newRowIndices[rowIndex]]);
                }

                m_schedule[affectedColumn].rows = sortedRows;
                m_schedule[affectedColumn].sorted = true;
            }
            // NOTE: for now, we only sort by one column
            break;
        }
    }
}

// Updates all Select type elements in the Column, if the Column is the right type
void Schedule::updateColumnSelects(size_t index)
{
    if (m_schedule[index].type == SCH_SELECT)
    {
        for (size_t i = 0; i < m_schedule[index].rows.size(); i++)
        {
            getMutableColumn(index)->getElement<Select>(i)->update();
        }
    }
    m_schedule[index].selectOptions.modificationApplied();
}

void Schedule::resetColumn(size_t index, SCHEDULE_TYPE type)
{
    Column& column = *getMutableColumn(index);
    size_t rowCount = column.rows.size();

    time_t t = std::time(nullptr);
    tm creationTime = *std::localtime(&t);

    switch (type) 
    {
        case(SCH_BOOL):
        {
            for (size_t row = 0; row < rowCount; row++)
            {
                setElement<Bool>(index, row, new Bool(false, type, DateContainer(creationTime), TimeContainer(creationTime)), false);
            }
            break;
        }
        case(SCH_NUMBER):
        {
            for (size_t row = 0; row < rowCount; row++)
            {
                setElement<Number>(index, row, new Number(0, type, DateContainer(creationTime), TimeContainer(creationTime)), false);
            }
            break;
        }
        case(SCH_DECIMAL):
        {
            for (size_t row = 0; row < rowCount; row++)
            {
                setElement<Decimal>(index, row, new Decimal(0, type, DateContainer(creationTime), TimeContainer(creationTime)), false);
            }  
            break;
        }
        case(SCH_TEXT):
        {
            for (size_t row = 0; row < rowCount; row++)
            {
                setElement<Text>(index, row, new Text("", type, DateContainer(creationTime), TimeContainer(creationTime)), false);
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
                std::cout << "Replacing with select" << std::endl;
                setElement<Select>(index, row, new Select(column.selectOptions, type, DateContainer(creationTime), TimeContainer(creationTime)), false);
            }     
            break;
        }
        case(SCH_TIME):
        {
            for (size_t row = 0; row < rowCount; row++)
            {
                setElement<Time>(index, row, new Time(0, 0, type, DateContainer(creationTime), TimeContainer(creationTime)), false);
            }
            break;
        }   
        case(SCH_DATE):
        {
            for (size_t row = 0; row < rowCount; row++)
            {
                setElement<Date>(index, row, new Date(creationTime, type, DateContainer(creationTime), TimeContainer(creationTime)), false);
            }
            break;
        }   
    }
}

void Schedule::addRow(size_t index)
{
    // the last index = just add to the end
    if (index == getRowCount())
    {
        for (Column& column : m_schedule)
        {
            std::vector<Element*>& columnValues = column.rows;
            time_t t = std::time(nullptr);
            tm creationTime = *std::localtime(&t);
            std::cout << creationTime.tm_hour << std::endl;
            std::cout << creationTime.tm_min << std::endl;

            switch(column.type)
            {
                case(SCH_BOOL):
                {
                    columnValues.push_back(new Bool(false, column.type, DateContainer(creationTime), TimeContainer(creationTime)));
                    break;
                }
                case(SCH_NUMBER):
                {
                    columnValues.push_back(new Number(0, column.type, DateContainer(creationTime), TimeContainer(creationTime)));
                    break;
                }
                case(SCH_DECIMAL):
                {
                    columnValues.push_back(new Decimal(0, column.type, DateContainer(creationTime), TimeContainer(creationTime)));
                    break;
                }
                case(SCH_TEXT):
                {
                    columnValues.push_back(new Text("", column.type, DateContainer(creationTime), TimeContainer(creationTime)));
                    break;
                }
                case(SCH_SELECT):
                { 
                    columnValues.push_back(new Select(column.selectOptions, column.type, DateContainer(creationTime), TimeContainer(creationTime)));      
                    break;
                }
                case(SCH_TIME):
                { 
                    columnValues.push_back(new Time(0, 0, column.type, DateContainer(creationTime), TimeContainer(creationTime)));      
                    break;
                }
                case(SCH_DATE):
                { 
                    columnValues.push_back(new Date(creationTime, column.type, DateContainer(creationTime), TimeContainer(creationTime)));      
                    break;
                }
            }
        }
    }
    // TODO: add row in the middle
    else
    {
        
    }

    sortColumns();
}

void Schedule::removeRow(size_t index)
{
    for (Column& column : m_schedule)
    {
        std::vector<Element*>& columnValues = column.rows;
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
} 

void Schedule::addDefaultColumn(size_t index)
{
    time_t t = std::time(nullptr);
    tm creationTime = *std::localtime(&t);

    // the last index = just add to the end
    if (index == getColumnCount())
    {
        std::vector<Element*> addedElements = {};

        for (size_t i = 0; i < getRowCount(); i++)
        {
            addedElements.push_back((Element*)new Text(std::string(""), SCH_TEXT, DateContainer(creationTime), TimeContainer(creationTime)));
        }
        m_schedule.push_back(Column{addedElements, SCH_TEXT, std::string("Text"), false});
    }
    else
    {
        // TODO: add in middle
    }
}

// Add a column from previous data. NOTE: Creates copies of all passed values, because this will probably mostly be used for duplicating columns
void Schedule::addColumn(size_t index, const Column& column)
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
        // TODO: add in middle
    }
}

void Schedule::removeColumn(size_t column)
{
    // a permanent column can't be removed
    if (m_schedule[column].permanent == true) { return; }

    Column* mutableColumn = getMutableColumn(column);
    for (size_t i = 0; i < mutableColumn->rows.size(); i++)
    {
        delete mutableColumn->rows[i];
    }

    // the last index = pop from end
    if (column == getColumnCount() - 1)
    {
        m_schedule.pop_back();
    }
    else
    {
        m_schedule.erase(m_schedule.begin() + column);
    }
}