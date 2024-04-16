#include <schedule_edit.h>

ScheduleEdit::ScheduleEdit(SCHEDULE_EDIT_TYPE type) 
{ 
    m_type = type;
}

ScheduleEdit::~ScheduleEdit()
{

}

void ScheduleEdit::revert(const ScheduleEditFunctions& editFunctions)
{
    m_isReverted = true;
}

void ScheduleEdit::apply(const ScheduleEditFunctions& editFunctions)
{
    m_isReverted = false;
}


// ElementEditBase
ElementEditBase::ElementEditBase(size_t column, size_t row, SCHEDULE_TYPE elementType) : ScheduleEdit(SCHEDULE_EDIT_ELEMENT) 
{
    m_column = column;
    m_row = row;
    m_elementType = elementType;
}


// RowEdit
RowEdit::RowEdit(bool isRemove, size_t row, const std::vector<ElementBase*>& elementDataCopy) : ScheduleEdit(SCHEDULE_EDIT_ROW) 
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

void RowEdit::revert(const ScheduleEditFunctions& editFunctions)
{
    // reverting a removal means adding the row back
    if (m_isRemove)
    {
        editFunctions.addRow(m_row, false);
        editFunctions.setRow(m_row, m_elementData);
    }
    // reverting an addition means removing the row again
    else
    {
        editFunctions.removeRow(m_row, false);
    }

    m_isReverted = true;
} 

void RowEdit::apply(const ScheduleEditFunctions& editFunctions)
{
    // applying a removal means removing the row
    if (m_isRemove)
    {
        editFunctions.removeRow(m_row, false);
    }
    // applying an addition means adding the row
    else
    {
        editFunctions.addRow(m_row, false);
        editFunctions.setRow(m_row, m_elementData);
    }

    m_isReverted = false;
}


// ColumnEdit
ColumnEdit::ColumnEdit(bool isRemove, size_t column, const Column& columnData) : ScheduleEdit(SCHEDULE_EDIT_COLUMN) 
{
    m_isRemove = isRemove;
    m_column = column;
    m_columnData = new Column(columnData);
}

ColumnEdit::~ColumnEdit()
{
    delete m_columnData;
}

void ColumnEdit::revert(const ScheduleEditFunctions& editFunctions)
{
    // reverting a removal means adding the column
    if (m_isRemove)
    {
        editFunctions.addColumn(m_column, *m_columnData, false);
    }
    // reverting an addition means removing the column
    else
    {
        editFunctions.removeColumn(m_column, false);
    }

    m_isReverted = true;
} 

void ColumnEdit::apply(const ScheduleEditFunctions& editFunctions)
{
    // applying a removal means removing the column
    if (m_isRemove)
    {
        editFunctions.removeColumn(m_column, false);
    }
    // applying an addition means adding the column
    else
    {
        editFunctions.addColumn(m_column, *m_columnData, false);
    }

    m_isReverted = false;
}


// ColumnPropertyEdit
ColumnPropertyEdit::ColumnPropertyEdit(size_t column, COLUMN_PROPERTY editedProperty, const Column& previousData, const Column& newData) : ScheduleEdit(SCHEDULE_EDIT_COLUMN_PROPERTY) 
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

void ColumnPropertyEdit::revert(const ScheduleEditFunctions& editFunctions)
{
    switch(m_editedProperty)
    {
        case(COLUMN_PROPERTY_NAME):
        {
            editFunctions.setColumnName(m_column, m_previousColumnData->name.c_str(), false);
            break;
        }
        case(COLUMN_PROPERTY_TYPE):
        {
            editFunctions.setColumnType(m_column, m_previousColumnData->type, false); // NOTE: TODO: will probably cause unrecoverable data loss with the resets involved
            break;
        }
        case(COLUMN_PROPERTY_SELECT_OPTIONS):
        {
            editFunctions.modifyColumnSelectOptions(m_column, OPTION_MODIFICATION_REPLACE, 0, 0, m_previousColumnData->selectOptions.getOptions(), false);
            break;
        }
        case(COLUMN_PROPERTY_SORT):
        {
            editFunctions.setColumnSort(m_column, m_previousColumnData->sort, false);
            break;
        }
    }

    m_isReverted = true;
} 

void ColumnPropertyEdit::apply(const ScheduleEditFunctions& editFunctions)
{
    switch(m_editedProperty)
    {
        case(COLUMN_PROPERTY_NAME):
        {
            editFunctions.setColumnName(m_column, m_columnData->name.c_str(), false);
            break;
        }
        case(COLUMN_PROPERTY_TYPE):
        {
            editFunctions.setColumnType(m_column, m_columnData->type, false); // NOTE: TODO: will probably cause unrecoverable data loss with the resets involved
            break;
        }
        case(COLUMN_PROPERTY_SELECT_OPTIONS):
        {
            editFunctions.modifyColumnSelectOptions(m_column, OPTION_MODIFICATION_REPLACE, 0, 0, m_columnData->selectOptions.getOptions(), false);
            break;
        }
        case(COLUMN_PROPERTY_SORT):
        {
            editFunctions.setColumnSort(m_column, m_columnData->sort, false);
            break;
        }
    }

    m_isReverted = false;
}

std::string ColumnPropertyEdit::getColumnName() const
{
    return m_previousColumnData->name;
}