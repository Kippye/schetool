#include <schedule.h>
#include <string.h>
#include <time.h>
#include <vector>
#include <any>
#include <cstddef>
#include <ctime>
#include <time_container.h>

// TEMP
#include <iostream>

void Schedule::test_setup()
{
    addColumn(getColumnCount(), Column{std::vector<std::any>{std::string("Zmitiusz"), std::string("Kip Kipperson")}, SCH_TEXT, std::string("Name"), true, true, false, false, ScheduleElementFlags_Name});
    addColumn(getColumnCount(), Column{std::vector<std::any>{std::byte(0), std::byte(0)}, SCH_BOOL, std::string("Finished"), true, true, false, false, ScheduleElementFlags_Finished});
    time_t now = time(0);
    tm defaultTime = *localtime(&now);
    defaultTime.tm_hour = 0;
    defaultTime.tm_min = 0;
    addColumn(getColumnCount(), Column{std::vector<std::any>{Time(defaultTime), Time(defaultTime)}, SCH_TIME, std::string("Start"), true, false, true, false, ScheduleElementFlags_Start});
    addColumn(getColumnCount(), Column{std::vector<std::any>{Time(defaultTime), Time(defaultTime)}, SCH_TIME, std::string("Duration"), true, false, true, false, ScheduleElementFlags_Duration});
    addColumn(getColumnCount(), Column{std::vector<std::any>{Time(defaultTime), Time(defaultTime)}, SCH_TIME, std::string("End"), true, false, true, false, ScheduleElementFlags_End});
    addColumn(getColumnCount(), Column{std::vector<std::any>{Time(defaultTime), Time(defaultTime)}, SCH_TIME, std::string("Date"), false, true, false, false});
    std::cout << getColumn(5)->flags;
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

const Column* Schedule::getColumn(unsigned int column)
{
    return &m_schedule[column];
}

void Schedule::setColumnType(unsigned int column, SCHEDULE_TYPE type)
{
    if (type == m_schedule[column].type) { return; }
    m_schedule[column].type = type;
    // TODO: try to convert types..? i guess there's no point in doing that. only really numbers could be turned into text.
    // reset values to defaults
    resetColumn(column);
}

void Schedule::setColumnName(unsigned int column, const char* name)
{
    m_schedule[column].name = std::string(name);
}

void Schedule::setColumnDisplayDate(unsigned int column, bool displayDate)
{
    m_schedule[column].displayDate = displayDate;
}
void Schedule::setColumnDisplayTime(unsigned int column, bool displayTime)
{
    m_schedule[column].displayTime = displayTime;
}
void Schedule::setColumnDisplayWeekday(unsigned int column, bool displayWeekday)
{
    m_schedule[column].displayWeekday = displayWeekday;
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
                setElement<std::byte>(column, row, std::byte(0));
            }
            break;
        }
        case(SCH_INT):
        {
            for (unsigned int row = 0; row < rowCount; row++)
            {
                setElement<int>(column, row, 0);
            }
            break;
        }
        case(SCH_DOUBLE):
        {
            for (unsigned int row = 0; row < rowCount; row++)
            {
                setElement<double>(column, row, 0);
            }  
            break;
        }
        case(SCH_TEXT):
        {
            for (unsigned int row = 0; row < rowCount; row++)
            {
                setElement<std::string>(column, row, std::string(""));
            }     
            break;
        }
        case(SCH_TIME):
        {
            for (unsigned int row = 0; row < rowCount; row++)
            {
                time_t now = time(0);
                setElement<Time>(column, row, Time(*gmtime(&now)));
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
                case(SCH_TIME):
                { 
                    time_t now = time(0);
                    columnValues.push_back(Time(*localtime(&now)));      
                    break;
                }
            }
        }
    }
    // TODO: add row in the middle
    else
    {
        
    }
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
        m_schedule.push_back(Column{std::vector<std::any>(getRowCount(), std::string("")), SCH_TEXT, std::string("Text"), false, true, false, false});
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