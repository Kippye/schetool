#include <schedule_edit.h>

ScheduleEdit::ScheduleEdit(ScheduleEditType type) 
{ 
    m_type = type;
}

ScheduleEdit::~ScheduleEdit()
{

}

void ScheduleEdit::revert(ScheduleCore* const scheduleCore)
{
    m_isReverted = true;
}

void ScheduleEdit::apply(ScheduleCore* const scheduleCore)
{
    m_isReverted = false;
}


// ElementEditBase
ElementEditBase::ElementEditBase(size_t column, size_t row, SCHEDULE_TYPE elementType) : ScheduleEdit(ScheduleEditType::ElementChange) 
{
    m_column = column;
    m_row = row;
    m_elementType = elementType;
}


// RowEdit
RowEdit::RowEdit(bool isRemove, size_t row, const std::vector<ElementBase*>& elementDataToCopy) : ScheduleEdit(ScheduleEditType::RowAddOrRemove) 
{
    m_isRemove = isRemove;
    m_row = row;
    for (ElementBase* element: elementDataToCopy)
    {
        m_elementData.push_back(element->getCopy());
    }
}

RowEdit::~RowEdit()
{
    if (m_elementData.size() == 0) { return; }

    for (size_t i = m_elementData.size() - 1; i > 0; i--)
    {
        delete m_elementData[i];
    }
}

void RowEdit::revert(ScheduleCore* const scheduleCore)
{
    // reverting a removal means adding the row back
    if (m_isRemove)
    {
        scheduleCore->addRow(m_row);
        scheduleCore->setRow(m_row, m_elementData);
    }
    // reverting an addition means removing the row again
    else
    {
        scheduleCore->removeRow(m_row);
    }

    m_isReverted = true;
} 

void RowEdit::apply(ScheduleCore* const scheduleCore)
{
    // applying a removal means removing the row
    if (m_isRemove)
    {
        scheduleCore->removeRow(m_row);
    }
    // applying an addition means adding the row
    else
    {
        scheduleCore->addRow(m_row);
        scheduleCore->setRow(m_row, m_elementData);
    }

    m_isReverted = false;
}


// ColumnEdit
ColumnEdit::ColumnEdit(bool isRemove, size_t column, const Column& columnData) : ScheduleEdit(ScheduleEditType::ColumnAddOrRemove) 
{
    m_isRemove = isRemove;
    m_column = column;
    m_columnData = Column(columnData);
}

void ColumnEdit::revert(ScheduleCore* const scheduleCore)
{
    // reverting a removal means adding the column
    if (m_isRemove)
    {
        scheduleCore->addColumn(m_column, m_columnData);
    }
    // reverting an addition means removing the column
    else
    {
        scheduleCore->removeColumn(m_column);
    }

    m_isReverted = true;
} 

void ColumnEdit::apply(ScheduleCore* const scheduleCore)
{
    // applying a removal means removing the column
    if (m_isRemove)
    {
        scheduleCore->removeColumn(m_column);
    }
    // applying an addition means adding the column
    else
    {
        scheduleCore->addColumn(m_column, m_columnData);
    }

    m_isReverted = false;
}


// ColumnPropertyEdit
ColumnPropertyEdit::ColumnPropertyEdit(size_t column, COLUMN_PROPERTY editedProperty, const Column& previousData, const Column& newData) : ScheduleEdit(ScheduleEditType::ColumnPropertyChange) 
{
    m_column = column;
    m_editedProperty = editedProperty;
    m_columnData = Column();
    m_previousColumnData = Column();

    switch (editedProperty)
    {
        case(COLUMN_PROPERTY_NAME):
        {
            m_previousColumnData.name = previousData.name;
            m_columnData.name = newData.name;
            break;
        }
        case(COLUMN_PROPERTY_TYPE):
        {
            m_previousColumnData.type = previousData.type;
            m_columnData.type = newData.type;
            // copy ROWS for previous data too.. >:(
            for (size_t i = 0; i < previousData.rows.size(); i++)
            {
                m_previousColumnData.rows.push_back(previousData.rows[i]->getCopy());
            }
            break;
        }
        case(COLUMN_PROPERTY_SELECT_OPTIONS):
        {
            m_previousColumnData.selectOptions = previousData.selectOptions;
            m_columnData.selectOptions = newData.selectOptions;
            break;
        }
        case(COLUMN_PROPERTY_SORT):
        {
            m_previousColumnData.sort = previousData.sort;
            m_columnData.sort = newData.sort;
            break;
        }
    }
}

void ColumnPropertyEdit::revert(ScheduleCore* const scheduleCore)
{
    switch(m_editedProperty)
    {
        case(COLUMN_PROPERTY_NAME):
        {
            scheduleCore->setColumnName(m_column, m_previousColumnData.name.c_str());
            break;
        }
        case(COLUMN_PROPERTY_TYPE):
        {
            scheduleCore->setColumnType(m_column, m_previousColumnData.type); // NOTE: TODO: will probably cause unrecoverable data loss with the resets involved
            scheduleCore->setColumnElements(m_column, m_previousColumnData);
            break;
        }
        case(COLUMN_PROPERTY_SELECT_OPTIONS):
        {
            scheduleCore->modifyColumnSelectOptions(m_column, SelectOptionsModification(OPTION_MODIFICATION_REPLACE).optionNames(m_previousColumnData.selectOptions.getOptions()));
            break;
        }
        case(COLUMN_PROPERTY_SORT):
        {
            scheduleCore->setColumnSort(m_column, m_previousColumnData.sort);
            break;
        }
    }

    m_isReverted = true;
} 

void ColumnPropertyEdit::apply(ScheduleCore* const scheduleCore)
{
    switch(m_editedProperty)
    {
        case(COLUMN_PROPERTY_NAME):
        {
            scheduleCore->setColumnName(m_column, m_columnData.name.c_str());
            break;
        }
        case(COLUMN_PROPERTY_TYPE):
        {
            scheduleCore->setColumnType(m_column, m_columnData.type); // NOTE: TODO: will probably cause unrecoverable data loss with the resets involved
            break;
        }
        case(COLUMN_PROPERTY_SELECT_OPTIONS):
        {
            scheduleCore->modifyColumnSelectOptions(m_column, SelectOptionsModification(OPTION_MODIFICATION_REPLACE).optionNames(m_columnData.selectOptions.getOptions()));
            break;
        }
        case(COLUMN_PROPERTY_SORT):
        {
            scheduleCore->setColumnSort(m_column, m_columnData.sort);
            break;
        }
    }

    m_isReverted = false;
}

std::string ColumnPropertyEdit::getColumnName() const
{
    return m_previousColumnData.name;
}


// FilterEditBase
FilterEditBase::FilterEditBase(size_t column, size_t filterIndex, ScheduleEditType editType) : ScheduleEdit(editType)
{
    m_column = column;
    m_filterIndex = filterIndex;
}

// FilterAddOrRemoveEditBase
FilterAddOrRemoveEditBase::FilterAddOrRemoveEditBase(bool isRemove, size_t column, size_t filterIndex) : FilterEditBase(column, filterIndex, ScheduleEditType::FilterAddOrRemove) 
{
    m_isRemove = isRemove;
}