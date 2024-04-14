#include "element.h"
#include "element_base.h"
#include "input.h"
#include "schedule_edit.h"
#include <cstdio>
#include <cstdlib>
#include <schedule.h>
#include <string.h>
#include <time.h>
#include <vector>
#include <ctime>
#include <numeric>

// TEMP
#include <iostream>

void Schedule::init(Input& input)
{
    input.addCallbackListener(INPUT_CALLBACK_SC_UNDO, undoCallback);
    input.addCallbackListener(INPUT_CALLBACK_SC_REDO, redoCallback);
}

void Schedule::createDefaultSchedule()
{
    clearSchedule();
    clearEditHistory();

    addColumn(getColumnCount(), Column(std::vector<ElementBase*>{}, SCH_TEXT, std::string("Name"), true, ScheduleElementFlags_Name), false);
    addColumn(getColumnCount(), Column(std::vector<ElementBase*>{}, SCH_BOOL, std::string("Finished"), true, ScheduleElementFlags_Finished), false);
    addColumn(getColumnCount(), Column(std::vector<ElementBase*>{}, SCH_TIME, std::string("Start"), true, ScheduleElementFlags_Start), false);
    addColumn(getColumnCount(), Column(std::vector<ElementBase*>{}, SCH_TIME, std::string("Duration"), true, ScheduleElementFlags_Duration), false);
    addColumn(getColumnCount(), Column(std::vector<ElementBase*>{}, SCH_TIME, std::string("End"), true, ScheduleElementFlags_End), false);
}

void Schedule::setScheduleName(const std::string& name)
{
    m_scheduleName = name;
}

std::string Schedule::getScheduleName()
{
    return m_scheduleName;
}

const std::deque<ScheduleEdit*>& Schedule::getEditHistory()
{
    return m_editHistory;
}

size_t Schedule::getEditHistoryIndex()
{
    return m_editHistoryIndex;
}

void Schedule::clearEditHistory()
{
    if (m_editHistory.size() == 0) { return; }

    for (int i = m_editHistory.size() - 1; i > 0; i--)
    {
        delete m_editHistory[i];
    }

    m_editHistory.clear();
}

bool Schedule::getEditedSinceWrite()
{
    return m_editedSinceWrite;
}

void Schedule::setEditedSinceWrite(bool to)
{
    m_editedSinceWrite = to;
}

void Schedule::clearSchedule()
{
    for (Column& column: m_schedule)
    {
        delete &column;
    }
    m_schedule.clear();
}

void Schedule::replaceSchedule(std::vector<Column>& columns)
{
    clearSchedule();
    clearEditHistory();

    m_schedule = columns;
}

// NOTE: If flags is ScheduleElementFlags_None, simply returns the first column it finds
size_t Schedule::getFlaggedColumnIndex(ScheduleElementFlags flags)
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
Column* Schedule::getColumnWithFlags(ScheduleElementFlags flags)
{
    return &m_schedule.at(getFlaggedColumnIndex(flags));
}

Column* Schedule::getMutableColumn(size_t column)
{
    if (column > getColumnCount())
    {
        std::cout << "Schedule::getModifiableColumn: index out of range. Returning last column" << std::endl;
        return &m_schedule.at(getColumnCount() - 1);
    }
    return &m_schedule.at(column);
}

// Sorts a copy of the column's rows. Then compares each element of the two rows vectors and returns a vector that contains which index of the OLD rows vector corresponds to that position in the NEW SORTED rows
std::vector<size_t> Schedule::getColumnSortedNewIndices(size_t index)
{
    Column& column = m_schedule.at(index);
    std::vector<ElementBase*> rows = column.rows;
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

const SelectOptions& Schedule::getColumnSelectOptions(size_t column)
{
    return m_schedule.at(column).selectOptions;
}

void Schedule::modifyColumnSelectOptions(size_t column, OPTION_MODIFICATION selectModification, size_t firstIndex, size_t secondIndex, const std::vector<std::string>& optionNames, bool addToHistory)
{
    Column previousData = Column(m_schedule.at(column));

    switch(selectModification)
    {
        case(OPTION_MODIFICATION_ADD):
        { 
            if (optionNames.size() == 0)
            {
                std::cout << "Schedule::modifyColumnSelectOptions: Failed to add Select option because no name was provided in the arguments" << std::endl;
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

    if (addToHistory)
    {
        addScheduleEdit(new ColumnPropertyEdit(this, column, COLUMN_PROPERTY_SELECT_OPTIONS, previousData, m_schedule.at(column)));
    }
}

void Schedule::setColumnType(size_t column, SCHEDULE_TYPE type, bool addToHistory)
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

    if (addToHistory)
    {
        addScheduleEdit(new ColumnPropertyEdit(this, column, COLUMN_PROPERTY_TYPE, previousData, m_schedule.at(column)));
    }
    
    setEditedSinceWrite(true);
}

void Schedule::setColumnName(size_t column, const char* name, bool addToHistory)
{
    Column previousData = Column(m_schedule.at(column));

    m_schedule.at(column).name = std::string(name);

    if (addToHistory)
    {
        addScheduleEdit(new ColumnPropertyEdit(this, column, COLUMN_PROPERTY_NAME, previousData, m_schedule.at(column)));
    }

    setEditedSinceWrite(true);
}

void Schedule::setColumnSort(size_t column, COLUMN_SORT sortDirection, bool addToHistory)
{
    Column previousData = Column(m_schedule.at(column));

    m_schedule.at(column).sort = sortDirection;
    if (sortDirection != COLUMN_SORT_NONE)
    {
        m_schedule.at(column).sorted = false;
        sortColumns();
    }

    if (addToHistory)
    {
        addScheduleEdit(new ColumnPropertyEdit(this, column, COLUMN_PROPERTY_SORT, previousData, m_schedule.at(column)));
    }

    setEditedSinceWrite(true);
}

size_t Schedule::getColumnCount()
{
    return m_schedule.size();
}

// Return the number of rows in the schedule or 0 if there are no columns (which probably won't happen?) 
size_t Schedule::getRowCount()
{
    return (m_schedule.size() > 0 ? m_schedule.at(0).rows.size() : 0);
}

// Sorts every column's rows based on "sorter" columns
void Schedule::sortColumns()
{
    for (size_t sorterColumn = 0; sorterColumn < getColumnCount(); sorterColumn++)
    {
        if (m_schedule.at(sorterColumn).sort != COLUMN_SORT_NONE)
        {
            std::vector<size_t> newRowIndices = getColumnSortedNewIndices(sorterColumn);

            // actually sort the rows vector in every column
            for (size_t affectedColumn = 0; affectedColumn < getColumnCount(); affectedColumn++)
            {
                // vector that will replace the old rows vector
                std::vector<ElementBase*> sortedRows = {};
                std::vector<ElementBase*>& unsortedRows = m_schedule.at(affectedColumn).rows;
                sortedRows.reserve(unsortedRows.size());

                for (size_t rowIndex = 0; rowIndex < newRowIndices.size(); rowIndex++)
                {
                    sortedRows.push_back(unsortedRows[newRowIndices[rowIndex]]);
                }

                m_schedule.at(affectedColumn).rows = sortedRows;
                m_schedule.at(affectedColumn).sorted = true;
            }
            // NOTE: for now, we only sort by one column
            break;
        }
    }
}

void Schedule::resetColumn(size_t index, SCHEDULE_TYPE type)
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
                setElement(index, row, (ElementBase*)new Element<SelectContainer>(type, SelectContainer(column.selectOptions), DateContainer(creationTime), TimeContainer(creationTime)), false);
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
            std::cout << "Resetting a column to type: " << type << " has not been implemented!" << std::endl;
            break;
        }
    }
}

void Schedule::addRow(size_t index, bool addToHistory)
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
                    std::cout << "Adding rows of type: " << column.type << " has not been implemented!" << std::endl;
                    break;
                }
            }

            // add a copy for edit history
            if (addToHistory)
            {
                elementCopies.push_back(columnValues.back()->getCopy());
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
                    std::cout << "Adding rows of type: " << column.type << " has not been implemented!" << std::endl;
                    break;
                }
            }

            // add a copy for edit history
            if (addToHistory)
            {
                elementCopies.push_back(columnValues.at(index)->getCopy());
            }
        }
    }

    if (addToHistory)
    {
        addScheduleEdit(new RowEdit(this, false, index, elementCopies));
    }

    setEditedSinceWrite(true);

    sortColumns();
}

void Schedule::removeRow(size_t index, bool addToHistory)
{
    if (getColumn(0)->rows.size() - 1 < index) { std::cout << "Schedule::removeRow: No row found at index: " << index << std::endl; return; }

    std::vector<ElementBase*> elementCopies = {};

    for (Column& column : m_schedule)
    {
        std::vector<ElementBase*>& columnValues = column.rows;

        if (addToHistory)
        {
            elementCopies.push_back(columnValues[index]->getCopy());
        }

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

    // add a remove RowEdit to the edit history with copies of the removed Elements
    if (addToHistory)
    {
        addScheduleEdit(new RowEdit(this, true, index, elementCopies));
    }

    setEditedSinceWrite(true);
}

void Schedule::setRow(size_t index, std::vector<ElementBase*> elementData)
{
    if (getColumn(0)->rows.size() - 1 < index) { std::cout << "Schedule::setRow: No row found at index: " << index << std::endl; return; }

    for (size_t col = 0; col < getColumnCount(); col++)
    {
        //std::cout << col << ": " << elementData[col]->getString() << std::endl;
        setElement(col, index, elementData[col]);
    } 
}

std::vector<ElementBase*> Schedule::getRow(size_t index)
{
    std::vector<ElementBase*> elementData = {};

    if (getColumn(0)->rows.size() - 1 < index) { std::cout << "Schedule::getRow: No row found at index: " << index << std::endl; return elementData; }

    for (size_t col = 0; col < getColumnCount(); col++)
    {
        elementData.push_back(getElement(col, index));
    }

    return elementData;
}

void Schedule::addDefaultColumn(size_t index, bool addToHistory)
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

    if (addToHistory)
    {
        addScheduleEdit(new ColumnEdit(this, false, index, addedColumn));
    }

    setEditedSinceWrite(true);
}

// Add a column from previous data. NOTE: Creates copies of all passed values, because this will probably mostly be used for duplicating columns
void Schedule::addColumn(size_t index, const Column& column, bool addToHistory)
{
    std::cout << "Adding column: " << column.name << std::endl;

    if (addToHistory)
    {
        addScheduleEdit(new ColumnEdit(this, false, index, column));
    }

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

    setEditedSinceWrite(true);
}

void Schedule::removeColumn(size_t column, bool addToHistory)
{
    // a permanent column can't be removed
    if (m_schedule.at(column).permanent == true) { return; }

    Column* mutableColumn = getMutableColumn(column);

    // oh cheese.
    if (addToHistory)
    {
        addScheduleEdit(new ColumnEdit(this, true, column, *mutableColumn));
    }

    // NOTE: removing from the vector destroys the Column, making mutableColumn invalid
    // the last index = pop from end
    if (column == getColumnCount() - 1)
    {
        m_schedule.pop_back();
    }
    else
    {
        // BUG why does erasing this one DELETE the last one??
        m_schedule.erase(m_schedule.begin() + column); // invalidates pointers to Columns past this one
    }  

    setEditedSinceWrite(true);
}

void Schedule::addScheduleEdit(ScheduleEdit* edit)
{
    std::cout << "Added Schedule Edit of type: " << edit->getType() << std::endl;
    removeFollowingEditHistory();
    m_editHistory.push_back(edit);
    m_editHistoryIndex = m_editHistory.size() - 1;
}

void Schedule::removeFollowingEditHistory()
{
    if (m_editHistory.size() == 0) { return; }

    for (int i = m_editHistory.size() - 1; i > m_editHistoryIndex; i--)
    {
        delete m_editHistory[i];
        m_editHistory.pop_back();
    }
}

void Schedule::undo()
{
    if (m_editHistory.size() == 0 || m_editHistoryIndex == 0 && m_editHistory[m_editHistoryIndex]->getIsReverted()) { return; }

    ScheduleEdit* edit = m_editHistory[m_editHistoryIndex];
    edit->revert(); 
    if (m_editHistoryIndex > 0) { m_editHistoryIndex--; }
}

void Schedule::redo()
{
    if (m_editHistory.size() == 0 || m_editHistoryIndex == m_editHistory.size() - 1 && m_editHistory[m_editHistoryIndex]->getIsReverted() == false) { return; }

    if ((m_editHistoryIndex > 0 || m_editHistory[0]->getIsReverted() == false) && m_editHistoryIndex < m_editHistory.size() - 1) { m_editHistoryIndex++; }
    ScheduleEdit* edit = m_editHistory[m_editHistoryIndex];
    edit->apply();
}