#pragma once
#include <vector>
#include <any>
#include <map>
#include <string>

const unsigned int ELEMENT_TEXT_MAX_LENGTH = 1024;
const unsigned int COLUMN_NAME_MAX_LENGTH = 64;

enum SCHEDULE_TYPE
{
    SCH_BOOL,
    SCH_INT,
    SCH_DOUBLE,
    SCH_TEXT,
    SCH_TIME,
    SCH_LAST,
    // TODO: Enum? how would that work?
};

class Schedule
{
    /* IDEA how i could make select or multiple select ones:
    // a specific class that contains -> a vector of strings (and colours)
    // then i can just use it as a type in std::any
    // and later it won't matter what enum it is since they're all the same class */

    private:
        // a tuple containing: 0 - rows; 1 - column type; 2 - column name; 3 - whether the column is permanent
        std::vector<std::tuple<std::vector<std::any>, SCHEDULE_TYPE, std::string, bool>> m_schedule = {};

    public:
        const std::map<SCHEDULE_TYPE, const char*> scheduleTypeNames =
        {
            {SCH_BOOL, "Bool"},
            {SCH_INT, "Integer"},
            {SCH_DOUBLE, "Decimal"},
            {SCH_TEXT, "Text"},
            {SCH_TIME, "Time"},
        };
        // TEMP
        void test_setup();

        SCHEDULE_TYPE getColumnType(unsigned int column);
        void setColumnType(unsigned int column, SCHEDULE_TYPE type);
        const char* getColumnName(unsigned int column);
        void setColumnName(unsigned int column, const char* name);
        bool getColumnPermanent(unsigned int column);

        void resetColumn(unsigned int column);

        unsigned int getColumnCount();
        unsigned int getRowCount();

        void addRow(unsigned int index);
        void removeRow(unsigned int index);
        void addColumn(unsigned int index);
        void addColumnWithData(unsigned int index, std::vector<std::any> rows, SCHEDULE_TYPE type = SCH_TEXT, std::string name = "New Column", bool permanent = false);
        void removeColumn(unsigned int index);

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