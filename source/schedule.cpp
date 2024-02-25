#include <schedule.h>
#include <string.h>
#include <vector>
#include <any>

#include <iostream>

// TEMP
#include <time_container.h>
#include <ctime>

void Schedule::test_setup()
{
    m_schedule.push_back({std::vector<std::any>{std::string("Zmitiusz"), std::string("Kip Kipperson")}, SCH_TEXT, std::string("Name")});
    m_schedule.push_back({std::vector<std::any>{std::byte(0), std::byte(0)}, SCH_BOOL, std::string("Finished")});
    m_schedule.push_back({std::vector<std::any>{69, 1254544}, SCH_INT, std::string("Coolness")});
    m_schedule.push_back({std::vector<std::any>{2.3, 0.0}, SCH_DOUBLE, std::string("Multiplier")});
    time_t now = time(0);
    m_schedule.push_back({std::vector<std::any>{Time(*gmtime(&now), true, false), Time(*localtime(&now), false, false)}, SCH_TIME, std::string("Date")});
    m_schedule.push_back({std::vector<std::any>{Time(*gmtime(&now), false, true), Time(*localtime(&now), false, true)}, SCH_TIME, std::string("Time")});
    m_schedule.push_back({std::vector<std::any>{Time(*gmtime(&now), false, false, true), Time(*localtime(&now), false, true, true)}, SCH_TIME, std::string("Weekday")});
}

const char* Schedule::getColumnName(unsigned int column)
{
    return std::get<2>(m_schedule[column]).c_str();
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

SCHEDULE_TYPE Schedule::getColumnType(unsigned int column)
{
    return std::get<1>(m_schedule[column]);
}

void Schedule::addRow(unsigned int index)
{
    // -1 = the last index = just add to the end
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