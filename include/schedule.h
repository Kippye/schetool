#pragma once
#include <vector>
#include <any>
#include <iostream>
#include <time_container.h>

enum SCHEDULE_TYPE
{
    SCH_BOOL,
    SCH_INT,
    SCH_DOUBLE,
    SCH_TEXT,
    SCH_TIME,
    // TODO: Enum? how would that work?
};
// This component handles the following:
// The actual vectors, maps containing schedule data
// Any modifications to the schedule data (coming from the Interface)
// So yeah this is mostly just a container for the data
// There will be separate components for stuff like the time that will also affect this
class Schedule
{
    // NOTE: Ok. what kind of things does this have to support?
    // It has to be 2-Dimensional and contain many different types of values (Text, enums, times, dates, bools)
    // 1. Obviously, editing the fields themselves
    // 2. Reordering both rows and columns!!
    // 3. Deleting rows (and columns?)

    /* IDEA how i could make select or multiple select ones:
    // a specific class that contains -> a vector of strings (and colours)
    // then i can just use it as a type in std::any
    // and later it won't matter what enum it is since they're all the same class
     maybe i'll have some kinda IDs for them since? actually no they'll all be in the same column anyway! */
  
    // NOTE: it seems like imgui's column reordering is purely visual, which is actually SMART! why would i reorder the underlying data if i can just reorder the visual part?
    // and i guess to save the ordering i could just save it somehow, then reapply it (imgui probs supports that)
    private:
        std::vector<std::tuple<std::vector<std::any>, SCHEDULE_TYPE, std::string>> m_schedule = {};

    public:
        // TEMP
        void test_setup();
        SCHEDULE_TYPE getColumnType(unsigned int column);
        void setColumnType(unsigned int column, SCHEDULE_TYPE type);
        const char* getColumnName(unsigned int column);
        void setColumnName(unsigned int column, std::string name);
        unsigned int getColumnCount();
        unsigned int getRowCount();
        void addRow(unsigned int index);
        void removeRow(unsigned int index);
        template <typename T>
        T getElement(unsigned int column, unsigned int row)
        {
            return std::any_cast<T>(std::get<0>(m_schedule[column])[row]);
        }
        template <typename T>
        void setElement(unsigned int column, unsigned int row, T value)
        {
            std::get<0>(m_schedule[column])[row] = value;
        }
};