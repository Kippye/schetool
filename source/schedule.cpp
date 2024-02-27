#include <schedule.h>
#include <string.h>
#include <vector>
#include <any>
#include <cstddef>
#include <ctime>
#include <time_container.h>

// TEMP
#include <iostream>

void Schedule::test_setup()
{
    addColumnWithData(0, std::vector<std::any>{std::string("Zmitiusz"), std::string("Kip Kipperson")}, SCH_TEXT, std::string("Name"), true);
    addColumnWithData(1, std::vector<std::any>{std::byte(0), std::byte(0)}, SCH_BOOL, std::string("Finished"), true);
    time_t now = time(0);
    addColumnWithData(2, std::vector<std::any>{Time(*gmtime(&now), true, false), Time(*localtime(&now), false, false)}, SCH_TIME, std::string("Date"));
    //m_schedule.push_back({std::vector<std::any>{std::string("Zmitiusz"), std::string("Kip Kipperson")}, SCH_TEXT, std::string("Name"), true});
    // m_schedule.push_back({std::vector<std::any>{std::byte(0), std::byte(0)}, SCH_BOOL, std::string("Finished"), true});
    // m_schedule.push_back({std::vector<std::any>{69, 1254544}, SCH_INT, std::string("Coolness"), false});
    // m_schedule.push_back({std::vector<std::any>{2.3, 0.0}, SCH_DOUBLE, std::string("Multiplier"), false});
    // m_schedule.push_back({std::vector<std::any>{Time(*gmtime(&now), false, true), Time(*localtime(&now), false, true)}, SCH_TIME, std::string("Time")});
    // m_schedule.push_back({std::vector<std::any>{Time(*gmtime(&now), false, false, true), Time(*localtime(&now), false, true, true)}, SCH_TIME, std::string("Weekday")});
}


SCHEDULE_TYPE Schedule::getColumnType(unsigned int column)
{
    return std::get<1>(m_schedule[column]);
}

void Schedule::setColumnType(unsigned int column, SCHEDULE_TYPE type)
{
    if (type == std::get<1>(m_schedule[column])) { return; }
    std::get<1>(m_schedule[column]) = type;
    // TODO: try to convert types..? i guess there's no point in doing that. only really numbers could be turned into text.
    // reset values to defaults
    resetColumn(column);
}

const char* Schedule::getColumnName(unsigned int column)
{
    return std::get<2>(m_schedule[column]).c_str();
}

void Schedule::setColumnName(unsigned int column, const char* name)
{
    std::get<2>(m_schedule[column]) = std::string(name);
}

bool Schedule::getColumnPermanent(unsigned int column)
{
    return std::get<3>(m_schedule[column]);
}

void Schedule::resetColumn(unsigned int column)
{
    std::vector<std::any>& rows = std::get<0>(m_schedule[column]);

    switch (std::get<1>(m_schedule[column])) 
    {
        case(SCH_BOOL):
        {
            for (unsigned int row = 0; row < std::get<0>(m_schedule[column]).size(); row++)
            {
                setElement<std::byte>(column, row, std::byte(0));
            }
            break;
        }
        case(SCH_INT):
        {
            for (unsigned int row = 0; row < std::get<0>(m_schedule[column]).size(); row++)
            {
                setElement<int>(column, row, 0);
            }
            break;
        }
        case(SCH_DOUBLE):
        {
            for (unsigned int row = 0; row < std::get<0>(m_schedule[column]).size(); row++)
            {
                setElement<double>(column, row, 0);
            }  
            break;
        }
        case(SCH_TEXT):
        {
            for (unsigned int row = 0; row < std::get<0>(m_schedule[column]).size(); row++)
            {
                setElement<std::string>(column, row, std::string(""));
            }     
            break;
        }
        case(SCH_TIME):
        {
            for (unsigned int row = 0; row < std::get<0>(m_schedule[column]).size(); row++)
            {
                time_t now = time(0);
                setElement<Time>(column, row, Time(*gmtime(&now), true, false));
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
    return (m_schedule.size() > 0 ? std::get<0>(m_schedule[0]).size() : 0);
}

void Schedule::addRow(unsigned int index)
{
    // the last index = just add to the end
    if (index == getRowCount())
    {
        for (auto& column : m_schedule)
        {
            std::vector<std::any>& columnValues = std::get<0>(column);
            switch(std::get<1>(column))
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
    for (auto& column : m_schedule)
    {
        std::vector<std::any>& columnValues = std::get<0>(column);
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

void Schedule::addColumn(unsigned int index)
{
    // the last index = just add to the end
    if (index == getColumnCount())
    {
        m_schedule.push_back({std::vector<std::any>(getRowCount(), std::string("")), SCH_TEXT, std::string("Text"), false});
    }
    else
    {
        // TODO: add in middle
    }
}

// Add a column from previous data. NOTE: Creates copies of all passed values, because this will probably mostly be used for duplicating columns
void Schedule::addColumnWithData(unsigned int index, std::vector<std::any> rows, SCHEDULE_TYPE type, std::string name, bool permanent)
{
    // TODO: make sure that EVERY column has the same amount of rows!!!

    // the last index = just add to the end
    if (index == getColumnCount())
    {
        m_schedule.push_back({rows, type, name, permanent});
    }
    else
    {
        // TODO: add in middle
    }
}

void Schedule::removeColumn(unsigned int index)
{
    // a permanent column can't be removed
    if (std::get<3>(m_schedule[index]) == true) { return; }

    // the last index = pop from end
    if (index == getColumnCount() - 1)
    {
        m_schedule.pop_back();
    }
    else
    {
        m_schedule.erase(m_schedule.begin() + index);
    }
}