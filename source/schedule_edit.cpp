#include <schedule_edit.h>

ScheduleEdit::ScheduleEdit(Schedule* schedule, SCHEDULE_EDIT_TYPE type) 
{ 
    m_schedule = schedule;
    m_type = type;
}

ScheduleEdit::~ScheduleEdit()
{

}

void ScheduleEdit::revert()
{
    m_isReverted = true;
}

void ScheduleEdit::apply()
{
    m_isReverted = false;
}


// ElementEditBase
ElementEditBase::ElementEditBase(Schedule* schedule, size_t column, size_t row, SCHEDULE_TYPE elementType) : ScheduleEdit(schedule, SCHEDULE_EDIT_ELEMENT) 
{
    m_column = column;
    m_row = row;
    m_elementType = elementType;
}


// RowEdit
RowEdit::RowEdit(Schedule* schedule, bool isRemove, size_t row, const std::vector<ElementBase*>& elementDataCopy) : ScheduleEdit(schedule, SCHEDULE_EDIT_ROW) 
{
    m_isRemove = isRemove;
    m_row = row;
    m_elementData = elementDataCopy;
}

RowEdit::~RowEdit()
{
    if (m_elementData.size() == 0) { return; }

    for (size_t i = m_elementData.size() - 1; i > 0; i--)
    {
        delete m_elementData[i];
    }
}

void RowEdit::revert()
{
    // reverting a removal means adding the row back
    if (m_isRemove)
    {
        m_schedule->addRow(m_row, false);
        m_schedule->setRow(m_row, m_elementData);
    }
    // reverting an addition means removing the row again
    else
    {
        m_schedule->removeRow(m_row, false);
    }

    m_isReverted = true;
} 

void RowEdit::apply()
{
    // applying a removal means removing the row
    if (m_isRemove)
    {
        m_schedule->removeRow(m_row, false);
    }
    // applying an addition means adding the row
    else
    {
        m_schedule->addRow(m_row, false);
        m_schedule->setRow(m_row, m_elementData);
    }

    m_isReverted = false;
}


// ColumnEdit
ColumnEdit::ColumnEdit(Schedule* schedule, bool isRemove, size_t column, const Column& columnData) : ScheduleEdit(schedule, SCHEDULE_EDIT_COLUMN) 
{
    m_isRemove = isRemove;
    m_column = column;
    m_columnData = new Column(columnData);
}

ColumnEdit::~ColumnEdit()
{
    delete m_columnData;
}

void ColumnEdit::revert()
{
    // reverting a removal means adding the column
    if (m_isRemove)
    {
        m_schedule->addColumn(m_column, *m_columnData, false);
    }
    // reverting an addition means removing the column
    else
    {
        m_schedule->removeColumn(m_column, false);
    }

    m_isReverted = true;
} 

void ColumnEdit::apply()
{
    // applying a removal means removing the column
    if (m_isRemove)
    {
        m_schedule->removeColumn(m_column, false);
    }
    // applying an addition means adding the column
    else
    {
        m_schedule->addColumn(m_column, *m_columnData, false);
    }

    m_isReverted = false;
}


// ColumnPropertyEdit
ColumnPropertyEdit::ColumnPropertyEdit(Schedule* schedule, size_t column, COLUMN_PROPERTY editedProperty, const Column& previousData, const Column& newData) : ScheduleEdit(schedule, SCHEDULE_EDIT_COLUMN_PROPERTY) 
{
    m_column = column;
    m_editedProperty = editedProperty;
    m_columnData = new Column();
    m_previousColumnData = new Column();

    switch (editedProperty)
    {
        case(COLUMN_PROPERTY_NAME):
        {
            m_previousColumnData->name = previousData.name;
            m_columnData->name = newData.name;
            break;
        }
        case(COLUMN_PROPERTY_TYPE):
        {
            m_previousColumnData->type = previousData.type;
            m_columnData->type = newData.type;
            break;
        }
        case(COLUMN_PROPERTY_SELECT_OPTIONS):
        {
            m_previousColumnData->selectOptions = previousData.selectOptions;
            m_columnData->selectOptions = newData.selectOptions;
            break;
        }
        case(COLUMN_PROPERTY_SORT):
        {
            m_previousColumnData->sort = previousData.sort;
            m_columnData->sort = newData.sort;
            break;
        }
    }
}

ColumnPropertyEdit::~ColumnPropertyEdit()
{
    delete m_previousColumnData;
    delete m_columnData;
}

void ColumnPropertyEdit::revert()
{
    switch(m_editedProperty)
    {
        case(COLUMN_PROPERTY_NAME):
        {
            m_schedule->setColumnName(m_column, m_previousColumnData->name.c_str(), false);
            break;
        }
        case(COLUMN_PROPERTY_TYPE):
        {
            m_schedule->setColumnType(m_column, m_previousColumnData->type, false); // NOTE: TODO: will probably cause unrecoverable data loss with the resets involved
            break;
        }
        case(COLUMN_PROPERTY_SELECT_OPTIONS):
        {
            m_schedule->modifyColumnSelectOptions(m_column, OPTION_MODIFICATION_REPLACE, 0, 0, m_previousColumnData->selectOptions.getOptions(), false);
            break;
        }
        case(COLUMN_PROPERTY_SORT):
        {
            m_schedule->setColumnSort(m_column, m_previousColumnData->sort, false);
            break;
        }
    }

    m_isReverted = true;
} 

void ColumnPropertyEdit::apply()
{
    switch(m_editedProperty)
    {
        case(COLUMN_PROPERTY_NAME):
        {
            m_schedule->setColumnName(m_column, m_columnData->name.c_str(), false);
            break;
        }
        case(COLUMN_PROPERTY_TYPE):
        {
            m_schedule->setColumnType(m_column, m_columnData->type, false); // NOTE: TODO: will probably cause unrecoverable data loss with the resets involved
            break;
        }
        case(COLUMN_PROPERTY_SELECT_OPTIONS):
        {
            m_schedule->modifyColumnSelectOptions(m_column, OPTION_MODIFICATION_REPLACE, 0, 0, m_columnData->selectOptions.getOptions(), false);
            break;
        }
        case(COLUMN_PROPERTY_SORT):
        {
            m_schedule->setColumnSort(m_column, m_columnData->sort, false);
            break;
        }
    }

    m_isReverted = false;
}

std::string ColumnPropertyEdit::getColumnName() const
{
    return m_previousColumnData->name;
}