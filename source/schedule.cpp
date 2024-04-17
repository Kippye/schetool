#include "element.h"
#include "element_base.h"
#include "input.h"
#include "schedule_edit.h"
#include <algorithm>
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
    m_core = ScheduleCore();

    m_editHistory = ScheduleEditHistory(&m_core);
    input.addCallbackListener(INPUT_CALLBACK_SC_UNDO, undoCallback);
    input.addCallbackListener(INPUT_CALLBACK_SC_REDO, redoCallback);
}

void Schedule::createDefaultSchedule()
{
    clearSchedule();
    m_editHistory.clearEditHistory();

    m_core.addColumn(getColumnCount(), Column(std::vector<ElementBase*>{}, SCH_TEXT, std::string("Name"), true, ScheduleColumnFlags_Name));
    m_core.addColumn(getColumnCount(), Column(std::vector<ElementBase*>{}, SCH_BOOL, std::string("Finished"), true, ScheduleColumnFlags_Finished));
    m_core.addColumn(getColumnCount(), Column(std::vector<ElementBase*>{}, SCH_TIME, std::string("Start"), true, ScheduleColumnFlags_Start));
    m_core.addColumn(getColumnCount(), Column(std::vector<ElementBase*>{}, SCH_TIME, std::string("Duration"), true, ScheduleColumnFlags_Duration));
    m_core.addColumn(getColumnCount(), Column(std::vector<ElementBase*>{}, SCH_TIME, std::string("End"), true, ScheduleColumnFlags_End));
}

void Schedule::setScheduleName(const std::string& name)
{
    m_core.setScheduleName(name);
}

std::string Schedule::getScheduleName()
{
    return m_core.getScheduleName();
}

void Schedule::clearSchedule()
{
    m_core.clearSchedule();
}

void Schedule::replaceSchedule(std::vector<Column>& columns)
{
    m_editHistory.clearEditHistory();

    m_core.replaceSchedule(columns);
}

const ScheduleEditHistory& Schedule::getScheduleEditHistory()
{
    return getScheduleEditHistoryMut();
}

ScheduleEditHistory& Schedule::getScheduleEditHistoryMut()
{
    return m_editHistory;
}

const Column* Schedule::getColumn(size_t column)
{
    return m_core.getColumn(column);
}

const std::vector<Column>& Schedule::getColumns()
{
    return m_core.getColumns();
}

std::vector<Column>& Schedule::getMutableColumns()
{
    return m_core.getMutableColumns();
}

const SelectOptions& Schedule::getColumnSelectOptions(size_t column)
{
    return m_core.getColumnSelectOptions(column);
}

void Schedule::modifyColumnSelectOptions(size_t column, OPTION_MODIFICATION selectModification, size_t firstIndex, size_t secondIndex, const std::vector<std::string>& optionNames, bool addToHistory)
{
    Column previousData = Column(*m_core.getColumn(column));

    m_core.modifyColumnSelectOptions(column, selectModification, firstIndex, secondIndex, optionNames);

    if (addToHistory)
    {
        m_editHistory.addEdit(new ColumnPropertyEdit(column, COLUMN_PROPERTY_SELECT_OPTIONS, previousData, *m_core.getColumn(column)));
    }
}

void Schedule::setColumnType(size_t column, SCHEDULE_TYPE type, bool addToHistory)
{
    //if (type == m_schedule[column].type) { return; }

    // for adding to edit history
    Column previousData = Column(*m_core.getColumn(column));

    m_core.setColumnType(column, type);

    if (addToHistory)
    {
        m_editHistory.addEdit(new ColumnPropertyEdit(column, COLUMN_PROPERTY_TYPE, previousData, *m_core.getColumn(column)));
    }
    
    m_editHistory.setEditedSinceWrite(true);
}

void Schedule::setColumnName(size_t column, const char* name, bool addToHistory)
{
    Column previousData = Column(*m_core.getColumn(column));

    m_core.setColumnName(column, name);

    if (addToHistory)
    {
        m_editHistory.addEdit(new ColumnPropertyEdit(column, COLUMN_PROPERTY_NAME, previousData, *m_core.getColumn(column)));
    }

    m_editHistory.setEditedSinceWrite(true);
}

void Schedule::setColumnSort(size_t column, COLUMN_SORT sortDirection, bool addToHistory)
{
    Column previousData = Column(*m_core.getColumn(column));

    m_core.setColumnSort(column, sortDirection);

    if (addToHistory)
    {
        m_editHistory.addEdit(new ColumnPropertyEdit(column, COLUMN_PROPERTY_SORT, previousData, *m_core.getColumn(column)));
    }

    m_editHistory.setEditedSinceWrite(true);
}

size_t Schedule::getColumnCount()
{
    return m_core.getColumnCount();
}

// Return the number of rows in the schedule or 0 if there are no columns or no rows (which probably won't happen?) 
size_t Schedule::getRowCount()
{
    return m_core.getRowCount();
}

// Sorts every column's rows based on "sorter" columns
void Schedule::sortColumns()
{
    m_core.sortColumns();
}

void Schedule::resetColumn(size_t index, SCHEDULE_TYPE type)
{
    m_core.resetColumn(index, type);
}

void Schedule::addRow(size_t index, bool addToHistory)
{
    std::vector<ElementBase*> elementCopies = m_core.getRow(index);

    m_core.addRow(index);

    if (addToHistory)
    {
        m_editHistory.addEdit(new RowEdit(false, index, elementCopies));
    }

    m_editHistory.setEditedSinceWrite(true);
}

void Schedule::removeRow(size_t index, bool addToHistory)
{
    std::vector<ElementBase*> elementCopies = m_core.getRow(index);

    m_core.removeRow(index);

    // add a remove RowEdit to the edit history with copies of the removed Elements
    if (addToHistory)
    {
        m_editHistory.addEdit(new RowEdit(true, index, elementCopies));
    }

    m_editHistory.setEditedSinceWrite(true);
}

void Schedule::setRow(size_t index, std::vector<ElementBase*> elementData)
{
    m_core.setRow(index, elementData);
}

std::vector<ElementBase*> Schedule::getRow(size_t index)
{
    return m_core.getRow(index);
}

void Schedule::addDefaultColumn(size_t index, bool addToHistory)
{
    m_core.addDefaultColumn(index);

    // kiiinda HACK y since i'm just hoping that the core actually added a Column at the index
    if (addToHistory)
    {
        if (index < m_core.getColumnCount())
        {
            m_editHistory.addEdit(new ColumnEdit(false, index, *m_core.getColumn(index)));
        }
    }

    m_editHistory.setEditedSinceWrite(true);
}

// Add a column from previous data. NOTE: Creates copies of all passed values, because this will probably mostly be used for duplicating columns
void Schedule::addColumn(size_t index, const Column& column, bool addToHistory)
{
    if (addToHistory)
    {
        m_editHistory.addEdit(new ColumnEdit(false, index, column));
    }

    m_core.addColumn(index, column);

    m_editHistory.setEditedSinceWrite(true);
}

void Schedule::removeColumn(size_t column, bool addToHistory)
{
    // i know it's kinda dumb to have this here too, but..
    // a permanent column can't be removed
    if (m_core.getColumn(column)->permanent == true) { return; }

    Column columnCopy = *m_core.getColumn(column);
    
    m_core.removeColumn(column);

    // oh cheese.
    if (addToHistory)
    {
        m_editHistory.addEdit(new ColumnEdit(true, column, columnCopy));
    }

    m_editHistory.setEditedSinceWrite(true);
}

void Schedule::undo()
{
    m_editHistory.undo();
}

void Schedule::redo()
{
    m_editHistory.redo();
}