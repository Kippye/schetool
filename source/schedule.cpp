#include <vector>
#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <string.h>
#include <schedule.h>
#include <main_menu_bar_gui.h>
#include <edit_history_gui.h>
#include <time.h>

#include <schedule_gui.h>

// TEMP
#include <iostream>

void Schedule::init(Input& input, Interface& interface)
{
    if (auto scheduleGui = interface.getGuiByID<ScheduleGui>("ScheduleGui"))
    {
        scheduleGui->setScheduleCore(m_core);
        if (auto elementEditorSubGui = scheduleGui->getSubGui<ElementEditorSubGui>("ElementEditorSubGui"))
        {
            elementEditorSubGui->modifyColumnSelectOptions.addListener(modifyColumnSelectOptionsListener); 
        }
        
        scheduleGui->setElementValueBool.addListener(setElementValueListenerBool);
        scheduleGui->setElementValueNumber.addListener(setElementValueListenerNumber);
        scheduleGui->setElementValueDecimal.addListener(setElementValueListenerDecimal);
        scheduleGui->setElementValueText.addListener(setElementValueListenerText);
        scheduleGui->setElementValueSelect.addListener(setElementValueListenerSelect);
        scheduleGui->setElementValueTime.addListener(setElementValueListenerTime);
        scheduleGui->setElementValueDate.addListener(setElementValueListenerDate);

        scheduleGui->addDefaultColumn.addListener(addDefaultColumnListener);
        scheduleGui->removeColumn.addListener(removeColumnListener);

        scheduleGui->setColumnType.addListener(setColumnTypeListener);
        scheduleGui->setColumnSort.addListener(setColumnSortListener);
        scheduleGui->setColumnName.addListener(setColumnNameListener);

        scheduleGui->addRow.addListener(addRowListener);
        scheduleGui->removeRow.addListener(removeRowListener);
    }
    if (auto mainMenuBarGui = interface.getGuiByID<MainMenuBarGui>("MainMenuBarGui"))
    {
        mainMenuBarGui->undoEvent.addListener(undoListener);
        mainMenuBarGui->redoEvent.addListener(redoListener);
    }
    if (auto editHistoryGui = interface.getGuiByID<EditHistoryGui>("EditHistoryGui"))
    {
        editHistoryGui->passScheduleEditHistory(&m_editHistory);
        editHistoryGui->undoEvent.addListener(undoListener);
        editHistoryGui->redoEvent.addListener(redoListener);
    }
    m_core = ScheduleCore();

    m_editHistory = ScheduleEditHistory(&m_core);
    input.addEventListener(INPUT_EVENT_SC_UNDO, undoListener);
    input.addEventListener(INPUT_EVENT_SC_REDO, redoListener);
}

void Schedule::setName(const std::string& name)
{
    if (name.size() > SCHEDULE_NAME_MAX_LENGTH)
    {
        m_scheduleName = name.substr(0, SCHEDULE_NAME_MAX_LENGTH);
    }
    else
    {
        m_scheduleName = name;
    }
}

std::string Schedule::getName()
{
    return m_scheduleName;
}

const ScheduleEditHistory& Schedule::getEditHistory()
{
    return getEditHistoryMutable();
}

ScheduleEditHistory& Schedule::getEditHistoryMutable()
{
    return m_editHistory;
}

void Schedule::undo()
{
    m_editHistory.undo();
}

void Schedule::redo()
{
    m_editHistory.redo();
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
    m_core.sortColumns();
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

const std::vector<Column>& Schedule::getAllColumns()
{
    return m_core.getAllColumns();
}

std::vector<Column>& Schedule::getAllColumnsMutable()
{
    return m_core.getAllColumnsMutable();
}

// Sorts every column's rows based on "sorter" columns
void Schedule::sortColumns()
{
    m_core.sortColumns();
}


size_t Schedule::getColumnCount()
{
    return m_core.getColumnCount();
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

void Schedule::removeColumn(size_t column, bool addToHistory)
{
    // i know it's kinda dumb to have this here too, but..
    // a permanent column can't be removed
    if (m_core.getColumn(column)->permanent == true) { return; }

    Column columnCopy = *m_core.getColumn(column);
    
    // oh cheese.
    if (m_core.removeColumn(column))
    {
        if (addToHistory)
        {
            m_editHistory.addEdit(new ColumnEdit(true, column, columnCopy));
        }

        m_editHistory.setEditedSinceWrite(true);
    }
}

const Column* Schedule::getColumn(size_t column)
{
    return m_core.getColumn(column);
}


void Schedule::setColumnType(size_t column, SCHEDULE_TYPE type, bool addToHistory)
{
    // for adding to edit history
    Column previousData = Column(*m_core.getColumn(column));

    if (m_core.setColumnType(column, type))
    {
        if (addToHistory)
        {
            m_editHistory.addEdit(new ColumnPropertyEdit(column, COLUMN_PROPERTY_TYPE, previousData, *m_core.getColumn(column)));
        }

        m_editHistory.setEditedSinceWrite(true);
    }
}

void Schedule::setColumnName(size_t column, const std::string& name, bool addToHistory)
{
    Column previousData = Column(*m_core.getColumn(column));

    if (m_core.setColumnName(column, name))
    {
        if (addToHistory)
        {
            m_editHistory.addEdit(new ColumnPropertyEdit(column, COLUMN_PROPERTY_NAME, previousData, *m_core.getColumn(column)));
        }

        m_editHistory.setEditedSinceWrite(true);
    }
}

void Schedule::setColumnSort(size_t column, COLUMN_SORT sortDirection, bool addToHistory)
{
    Column previousData = Column(*m_core.getColumn(column));

    if (m_core.setColumnSort(column, sortDirection))
    {
        if (addToHistory)
        {
            m_editHistory.addEdit(new ColumnPropertyEdit(column, COLUMN_PROPERTY_SORT, previousData, *m_core.getColumn(column)));
        }

        m_editHistory.setEditedSinceWrite(true);
    }
}

const SelectOptions& Schedule::getColumnSelectOptions(size_t column)
{
    return m_core.getColumnSelectOptions(column);
}

void Schedule::modifyColumnSelectOptions(size_t column, const SelectOptionsModification& selectOptionsModification, bool addToHistory)
{
    Column previousData = Column(*m_core.getColumn(column));

    if (m_core.modifyColumnSelectOptions(column, selectOptionsModification))
    {
        if (addToHistory)
        {
            m_editHistory.addEdit(new ColumnPropertyEdit(column, COLUMN_PROPERTY_SELECT_OPTIONS, previousData, *m_core.getColumn(column)));
        }
    
        m_editHistory.setEditedSinceWrite(true);
    }
}

void Schedule::resetColumn(size_t index, SCHEDULE_TYPE type)
{
    m_core.resetColumn(index, type);
}


// Return the number of rows in the schedule or 0 if there are no columns or no rows (which probably won't happen?) 
size_t Schedule::getRowCount()
{
    return m_core.getRowCount();
}

void Schedule::addRow(size_t index, bool addToHistory)
{
    m_core.addRow(index);

    if (addToHistory)
    {
        m_editHistory.addEdit(new RowEdit(false, index, m_core.getRow(index)));
    }

    m_editHistory.setEditedSinceWrite(true);
}

void Schedule::removeRow(size_t index, bool addToHistory)
{
    std::vector<ElementBase*> originalRow = m_core.getRow(index);
    // temporary vector of copies
    std::vector<ElementBase*> originalRowCopies = {};

    for (size_t i = 0; i < originalRow.size(); i++)
    {
        originalRowCopies.push_back(originalRow[i]->getCopy());
    }

    if (m_core.removeRow(index))
    {
        if (addToHistory)
        {
            // add a remove RowEdit to the edit history with copies of the removed Elements
            m_editHistory.addEdit(new RowEdit(true, index, originalRowCopies));
        }

        m_editHistory.setEditedSinceWrite(true);
    }

    for (size_t i = 0; i < originalRowCopies.size(); i++)
    {
        delete originalRowCopies[i];
    }
}

std::vector<ElementBase*> Schedule::getRow(size_t index)
{
    return m_core.getRow(index);
}

void Schedule::setRow(size_t index, std::vector<ElementBase*> elementData)
{
    m_core.setRow(index, elementData);
}

std::vector<size_t> Schedule::getSortedRowIndices()
{
    return m_core.getSortedRowIndices();
}