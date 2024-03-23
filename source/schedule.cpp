#include <select_container.h>
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
    addColumn(getColumnCount(), Column{std::vector<std::any>{std::string("Zmitiusz"), std::string("Kip Kipperson")}, SCH_TEXT, std::string("Name"), true, ScheduleElementFlags_Name});
    addColumn(getColumnCount(), Column{std::vector<std::any>{std::byte(0), std::byte(0)}, SCH_BOOL, std::string("Finished"), true, ScheduleElementFlags_Finished});
    time_t now = time(0);
    tm defaultTime = *localtime(&now);
    addColumn(getColumnCount(), Column{std::vector<std::any>{Time(0, 0), Time(0, 0)}, SCH_TIME, std::string("Start"), true, ScheduleElementFlags_Start});
    addColumn(getColumnCount(), Column{std::vector<std::any>{Time(0, 0), Time(0, 0)}, SCH_TIME, std::string("Duration"), true, ScheduleElementFlags_Duration});
    addColumn(getColumnCount(), Column{std::vector<std::any>{Time(0, 0), Time(0, 0)}, SCH_TIME, std::string("End"), true, ScheduleElementFlags_End});
    addColumn(getColumnCount(), Column{std::vector<std::any>{Date(defaultTime), Date(defaultTime)}, SCH_DATE, std::string("Date"), false});
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

// Sorts a copy of the column's rows. Then compares each element of the two rows vectors and returns a vector that contains which index of the OLD rows vector corresponds to that position in the NEW SORTED rows
std::vector<size_t> Schedule::getColumnSortedNewIndices(unsigned int index)
{
    Column& column = m_schedule[index];
    std::vector<std::any> rows = column.rows;
    std::vector<size_t> newIndices(rows.size());
    std::iota(newIndices.begin(), newIndices.end(), 0);

    m_columnSortComparison.setup(column.type, column.sort);
    std::sort(newIndices.begin(), newIndices.end(), [&](size_t i, size_t j){ return m_columnSortComparison(rows[i], rows[j]); });
    return newIndices;
}

const Column* Schedule::getColumn(unsigned int column)
{
    return &m_schedule[column];
}

SelectOptions& Schedule::getColumnSelectOptions(unsigned int column)
{
    return m_schedule[column].selectOptions;
}

void Schedule::setColumnType(unsigned int column, SCHEDULE_TYPE type)
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

    m_schedule[column].type = type;
    // TODO: try to convert types..? i guess there's no point in doing that. only really numbers could be turned into text.
    // reset values to defaults
    resetColumn(column);
}

void Schedule::setColumnName(unsigned int column, const char* name)
{
    m_schedule[column].name = std::string(name);
}

void Schedule::setColumnSort(unsigned int column, COLUMN_SORT sortDirection)
{
    m_schedule[column].sort = sortDirection;
    if (sortDirection != COLUMN_SORT_NONE)
    {
        m_schedule[column].sorted = false;
        sortColumns();
    }
}

void Schedule::resetColumn(unsigned int column)
{
    unsigned int rowCount = m_schedule[column].rows.size();

    switch (m_schedule[column].type) 
    {
        case(SCH_BOOL):
        {
            for (unsigned int row = 0; row < rowCount; row++)
            {
                setElement<std::byte>(column, row, std::byte(0), false);
            }
            break;
        }
        case(SCH_INT):
        {
            for (unsigned int row = 0; row < rowCount; row++)
            {
                setElement<int>(column, row, 0, false);
            }
            break;
        }
        case(SCH_DOUBLE):
        {
            for (unsigned int row = 0; row < rowCount; row++)
            {
                setElement<double>(column, row, 0, false);
            }  
            break;
        }
        case(SCH_TEXT):
        {
            for (unsigned int row = 0; row < rowCount; row++)
            {
                setElement<std::string>(column, row, std::string(""), false);
            }     
            break;
        }
        case(SCH_SELECT):
        {
            for (unsigned int row = 0; row < rowCount; row++)
            {
                if (m_schedule[column].selectOptions.getIsMutable() == true)
                {
                    m_schedule[column].selectOptions.clearOptions();
                }
                setElement<Select>(column, row, Select(m_schedule[column].selectOptions), false);
            }     
            break;
        }
        case(SCH_TIME):
        {
            for (unsigned int row = 0; row < rowCount; row++)
            {
                time_t now = time(0);
                setElement<Time>(column, row, Time(0, 0), false);
            }
            break;
        }   
        case(SCH_DATE):
        {
            for (unsigned int row = 0; row < rowCount; row++)
            {
                time_t now = time(0);
                setElement<Date>(column, row, Date(*gmtime(&now)), false);
            }
            break;
        }   
    }
}

unsigned int Schedule::getColumnCount()
{
    return m_schedule.size();
}

// Return the number of rows in the schedule or 0 if there are no columns (which probably won't happen?) 
unsigned int Schedule::getRowCount()
{
    return (m_schedule.size() > 0 ? m_schedule[0].rows.size() : 0);
}

// Sorts every column's rows based on "sorter" columns
void Schedule::sortColumns()
{
    for (int sorterColumn = 0; sorterColumn < getColumnCount(); sorterColumn++)
    {
        if (m_schedule[sorterColumn].sort != COLUMN_SORT_NONE)
        {
            std::vector<size_t> newRowIndices = getColumnSortedNewIndices(sorterColumn);

            // actually sort the rows vector in every column
            for (int affectedColumn = 0; affectedColumn < getColumnCount(); affectedColumn++)
            {
                // vector that will replace the old rows vector
                std::vector<std::any> sortedRows = {};
                std::vector<std::any>& unsortedRows = m_schedule[affectedColumn].rows;
                sortedRows.reserve(unsortedRows.size());

                for (int rowIndex = 0; rowIndex < newRowIndices.size(); rowIndex++)
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
void Schedule::updateColumnSelects(unsigned int index)
{
    // TODO: some way to check if it's any type of Select rather than just one by one YESS i can use flags maybe?!!
    if (m_schedule[index].type == SCH_SELECT)
    {
        for (int i = 0; i < m_schedule[index].rows.size(); i++)
        {
            Select sel = getElement<Select>(index, i);
            sel.update();
            setElement<Select>(index, i, sel);
        }
    }
    m_schedule[index].selectOptions.modificationApplied();
}

void Schedule::addRow(unsigned int index)
{
    // the last index = just add to the end
    if (index == getRowCount())
    {
        for (Column& column : m_schedule)
        {
            std::vector<std::any>& columnValues = column.rows;
            switch(column.type)
            {
                case(SCH_BOOL):
                {
                    columnValues.push_back(std::byte(0));
                    break;
                }
                case(SCH_INT):
                {
                    columnValues.push_back(0);
                    break;
                }
                case(SCH_DOUBLE):
                {
                    columnValues.push_back(0.0);
                    break;
                }
                case(SCH_TEXT):
                {
                    columnValues.push_back(std::string(""));
                    break;
                }
                case(SCH_SELECT):
                { 
                    columnValues.push_back(Select(m_schedule[index].selectOptions));      
                    break;
                }
                case(SCH_TIME):
                { 
                    columnValues.push_back(Time(0, 0));      
                    break;
                }
                case(SCH_DATE):
                { 
                    time_t now = time(0);
                    columnValues.push_back(Date(*localtime(&now)));      
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

void Schedule::removeRow(unsigned int index)
{
    for (Column& column : m_schedule)
    {
        std::vector<std::any>& columnValues = column.rows;
        if (index == columnValues.size() - 1)
        {
            columnValues.pop_back();
        }
        else
        {
            columnValues.erase(columnValues.begin() + index);
        }
    }
} 

void Schedule::addDefaultColumn(unsigned int index)
{
    // the last index = just add to the end
    if (index == getColumnCount())
    {
        m_schedule.push_back(Column{std::vector<std::any>(getRowCount(), std::string("")), SCH_TEXT, std::string("Text"), false});
    }
    else
    {
        // TODO: add in middle
    }
}

// Add a column from previous data. NOTE: Creates copies of all passed values, because this will probably mostly be used for duplicating columns
void Schedule::addColumn(unsigned int index, Column& column)
{
    // TODO: make sure that EVERY column has the same amount of rows!!!

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

void Schedule::removeColumn(unsigned int column)
{
    // a permanent column can't be removed
    if (m_schedule[column].permanent == true) { return; }

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