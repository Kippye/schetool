#include "data_converter.h"
#include "element_base.h"
#include "element.h"

LocalObjectTable& ObjectDefinitions::getObjectTable()
{
    return m_objectTable;
}

tm BLF_ElementInfo::getCreationTime() const
{
    return tm {
        0,
        (int)creationMinutes,
        (int)creationHours,
        (int)creationMday,
        (int)creationMonth,
        (int)creationYear,
        0,
        0,
        0
    };
}

void DataConverter::setupObjectTable()
{
    addObjectDefinition<BLF_Base>();
    addObjectDefinition<BLF_ElementInfo>();
    addObjectDefinition<BLF_Element<bool>>();
    addObjectDefinition<BLF_Element<int>>();
    addObjectDefinition<BLF_Element<double>>();
    addObjectDefinition<BLF_Element<std::string>>();
    addObjectDefinition<BLF_Element<SelectContainer>>();
    addObjectDefinition<BLF_Element<WeekdayContainer>>();
    addObjectDefinition<BLF_Element<TimeContainer>>();
    addObjectDefinition<BLF_Element<DateContainer>>();
    addObjectDefinition<BLF_Filter<bool>>();
    addObjectDefinition<BLF_Filter<int>>();
    addObjectDefinition<BLF_Filter<double>>();
    addObjectDefinition<BLF_Filter<std::string>>();
    addObjectDefinition<BLF_Filter<SelectContainer>>();
    addObjectDefinition<BLF_Filter<WeekdayContainer>>();
    addObjectDefinition<BLF_Filter<TimeContainer>>();
    addObjectDefinition<BLF_Filter<DateContainer>>();
    addObjectDefinition<BLF_Column<bool>>();
    addObjectDefinition<BLF_Column<int>>();
    addObjectDefinition<BLF_Column<double>>();
    addObjectDefinition<BLF_Column<std::string>>();
    addObjectDefinition<BLF_Column<SelectContainer>>();
    addObjectDefinition<BLF_Column<WeekdayContainer>>();
    addObjectDefinition<BLF_Column<TimeContainer>>();
    addObjectDefinition<BLF_Column<DateContainer>>();
}

int DataConverter::writeSchedule(const char* path, const std::vector<Column>& schedule)
{
    FileWriteStream stream(path);

    DataTable data;

    for (size_t c = 0; c < schedule.size(); c++)
    {
        SCHEDULE_TYPE columnType = schedule[c].type;
        switch(columnType)
        {
            case(SCH_BOOL):
                addColumnToData<bool>(data, schedule[c], c);
                break;
            case(SCH_NUMBER):
                addColumnToData<int>(data, schedule[c], c);
                break;
            case(SCH_DECIMAL):
                addColumnToData<double>(data, schedule[c], c);
                break;
            case(SCH_TEXT):
                addColumnToData<std::string>(data, schedule[c], c);
                break;
            case(SCH_SELECT):
                addColumnToData<SelectContainer>(data, schedule[c], c);
                break;
            case(SCH_WEEKDAY):
                addColumnToData<WeekdayContainer>(data, schedule[c], c);
                break;
            case(SCH_TIME):
                addColumnToData<TimeContainer>(data, schedule[c], c);
                break;
            case(SCH_DATE):
                addColumnToData<DateContainer>(data, schedule[c], c);
                break;
            default:
                printf("DataConverter::writeSchedule(%s, schedule): Writing Columns of type %d has not been implemented\n", path, schedule[c].type);
                break;
        }
    }

    File file(data, m_definitions.getObjectTable(), {blf::CompressionType::None, blf::EncryptionType::None});

    file.serialize(stream);

    return 0;
}

int DataConverter::readSchedule(const char* path, std::vector<Column>& schedule)
{
    std::vector<Column> scheduleCopy = schedule;
    // clear the provided copy just in case
    schedule.clear();
    
    FileReadStream stream(path);

    auto file = File::fromData(stream);

    auto fileBody = file.deserializeBody(m_definitions.getObjectTable());

    std::cout << "Deserialized" << std::endl;

    std::map<size_t, SCHEDULE_TYPE> columnTypes = {};

    // go through columns by type (they contain all their data so dont need to get anything else)
    for (size_t t = 0; t < (size_t)SCH_LAST; t++)
    {
        SCHEDULE_TYPE type = (SCHEDULE_TYPE)t;

        switch(type)
        {
            case(SCH_BOOL):
            {
                for (const BLF_Column<bool>& column: fileBody.data.groupby(m_definitions.get<BLF_Column<bool>>()))
                {
                    columnTypes.insert({column.index, type});
                }
                break;
            }
            case(SCH_NUMBER):
            {
                for (const BLF_Column<int>& column: fileBody.data.groupby(m_definitions.get<BLF_Column<int>>()))
                {
                    columnTypes.insert({column.index, type});
                }              
                break;
            }
            case(SCH_DECIMAL):
            {
                for (const BLF_Column<double>& column: fileBody.data.groupby(m_definitions.get<BLF_Column<double>>()))
                {
                    columnTypes.insert({column.index, type});
                }  
                break;
            }
            case(SCH_TEXT):
            {
                for (const BLF_Column<std::string>& column: fileBody.data.groupby(m_definitions.get<BLF_Column<std::string>>()))
                {
                    columnTypes.insert({column.index, type});
                }                
                break;
            }
            case(SCH_SELECT):
            { 
                for (const BLF_Column<SelectContainer>& column: fileBody.data.groupby(m_definitions.get<BLF_Column<SelectContainer>>()))
                {
                    columnTypes.insert({column.index, type});
                }  
                break;
            }
            case(SCH_WEEKDAY):
            { 
                for (const BLF_Column<WeekdayContainer>& column: fileBody.data.groupby(m_definitions.get<BLF_Column<WeekdayContainer>>()))
                {
                    columnTypes.insert({column.index, type});
                }
                break;
            }
            case(SCH_TIME):
            { 
                 for (const BLF_Column<TimeContainer>& column: fileBody.data.groupby(m_definitions.get<BLF_Column<TimeContainer>>()))
                {
                    columnTypes.insert({column.index, type});
                }                 
                break;
            }
            case(SCH_DATE):
            { 
                for (const BLF_Column<DateContainer>& column: fileBody.data.groupby(m_definitions.get<BLF_Column<DateContainer>>()))
                {
                    columnTypes.insert({column.index, type});
                }                  
                break;
            }
            default:
            {
                printf("DataConverter::readSchedule(%s, schedule): Reading elements with type %d has not been implemented\n", path, type);
            }
        }
    }

    std::cout << "Mapped indices to types" << std::endl;

    for (size_t columnIndex = 0; columnIndex < columnTypes.size(); columnIndex++)
    {
        if (columnTypes.find(columnIndex) == columnTypes.end())
        {
            continue;
        }
        SCHEDULE_TYPE type = columnTypes.at(columnIndex);

        switch(type)
        {
            case(SCH_BOOL):
            {
                for (const BLF_Column<bool>& column: fileBody.data.groupby(m_definitions.get<BLF_Column<bool>>()))
                {
                    if (column.index == columnIndex) 
                    {
                        schedule.push_back(column.getColumn());
                    }
                }
                break;
            }
            case(SCH_NUMBER):
            {
                for (const BLF_Column<int>& column: fileBody.data.groupby(m_definitions.get<BLF_Column<int>>()))
                {
                    if (column.index == columnIndex) 
                    {
                        schedule.push_back(column.getColumn()); 
                    }
                }              
                break;
            }
            case(SCH_DECIMAL):
            {
                for (const BLF_Column<double>& column: fileBody.data.groupby(m_definitions.get<BLF_Column<double>>()))
                {
                    if (column.index == columnIndex) 
                    {
                        schedule.push_back(column.getColumn());
                    }
                }  
                break;
            }
            case(SCH_TEXT):
            {
                for (const BLF_Column<std::string>& column: fileBody.data.groupby(m_definitions.get<BLF_Column<std::string>>()))
                {
                    if (column.index == columnIndex) 
                    {
                        schedule.push_back(column.getColumn());
                    }
                }                
                break;
            }
            case(SCH_SELECT):
            { 
                for (const BLF_Column<SelectContainer>& column: fileBody.data.groupby(m_definitions.get<BLF_Column<SelectContainer>>()))
                {
                    if (column.index == columnIndex) 
                    {
                        schedule.push_back(column.getColumn());
                    }
                }  
                break;
            }
            case(SCH_WEEKDAY):
            { 
                for (const BLF_Column<WeekdayContainer>& column: fileBody.data.groupby(m_definitions.get<BLF_Column<WeekdayContainer>>()))
                {
                    if (column.index == columnIndex) 
                    {
                        schedule.push_back(column.getColumn());
                    }
                }
                break;
            }
            case(SCH_TIME):
            { 
                 for (const BLF_Column<TimeContainer>& column: fileBody.data.groupby(m_definitions.get<BLF_Column<TimeContainer>>()))
                {
                    if (column.index == columnIndex) 
                    {
                        schedule.push_back(column.getColumn());
                    }
                }                 
                break;
            }
            case(SCH_DATE):
            { 
                for (const BLF_Column<DateContainer>& column: fileBody.data.groupby(m_definitions.get<BLF_Column<DateContainer>>()))
                {
                    if (column.index == columnIndex) 
                    {
                        schedule.push_back(column.getColumn());
                    }
                }                  
                break;
            }
            default:
            {
                printf("DataConverter::readSchedule(%s, schedule): Reading elements with type %d has not been implemented\n", path, type);
            }
        }
    }

    // // loop through the BLF_Columns and add them to the schedule as Columns
    // for (size_t c = 0; c < loadedColumns.getSize(); c++)
    // {
    //     Column column = Column(
    //         std::vector<ElementBase*>{}, 
    //         (SCHEDULE_TYPE)loadedColumns[c]->type, 
    //         std::string(loadedColumns[c]->name.getBuffer()),
    //         loadedColumns[c]->permanent,
    //         (ScheduleColumnFlags)loadedColumns[c]->flags,
    //         (COLUMN_SORT)loadedColumns[c]->sort,
    //         SelectOptions(loadedColumns[c]->getSelectOptions())
    //     );

    //     column.selectOptions.setIsMutable(loadedColumns[c]->selectOptionsMutable);

    //     SCHEDULE_TYPE columnType = (SCHEDULE_TYPE)loadedColumns[c]->type;

    //     // TEMP get filters
    //     switch(columnType)
    //     {
    //         case(SCH_BOOL):
    //         {
    //             for (BLF_Filter<bool>* filter: file.data.get<BLF_Filter<bool>>())
    //             {
    //                 if (filter->columnIndex == c)
    //                 {
    //                     auto specialFilter = Filter<bool>(filter->passValue);
    //                     specialFilter.setComparison((Comparison)filter->comparison);
    //                     column.addFilter(specialFilter);
    //                     dataPointers.push_back(filter);
    //                 }
    //             }
    //             break;
    //         }
    //         case(SCH_NUMBER):
    //         {
    //             for (BLF_Filter<int>* filter: file.data.get<BLF_Filter<int>>())
    //             {
    //                 if (filter->columnIndex == c)
    //                 {
    //                     auto specialFilter = Filter<int>(filter->passValue);
    //                     specialFilter.setComparison((Comparison)filter->comparison);
    //                     column.addFilter(specialFilter);
    //                     dataPointers.push_back(filter);
    //                 }
    //             }
    //             break;
    //         }
    //         case(SCH_DECIMAL):
    //         {
    //             for (BLF_Filter<double>* filter: file.data.get<BLF_Filter<double>>())
    //             {
    //                 if (filter->columnIndex == c)
    //                 {
    //                     auto specialFilter = Filter<double>(filter->passValue);
    //                     specialFilter.setComparison((Comparison)filter->comparison);
    //                     column.addFilter(specialFilter);
    //                     dataPointers.push_back(filter);
    //                 }
    //             }    
    //             break;
    //         }
    //         case(SCH_TEXT):
    //         {
    //             for (BLF_Filter<std::string>* filter: file.data.get<BLF_Filter<std::string>>())
    //             {
    //                 if (filter->columnIndex == c)
    //                 {
    //                     auto specialFilter = Filter<std::string>(filter->passValue);
    //                     specialFilter.setComparison((Comparison)filter->comparison);
    //                     column.addFilter(specialFilter);
    //                     dataPointers.push_back(filter);
    //                 }
    //             }                 
    //             break;
    //         }
    //         case(SCH_SELECT):
    //         { 
    //             for (BLF_Filter<SelectContainer>* filter: file.data.get<BLF_Filter<SelectContainer>>())
    //             {
    //                 if (filter->columnIndex == c)
    //                 {
    //                     auto specialFilter = Filter<SelectContainer>(filter->getValue());
    //                     specialFilter.setComparison((Comparison)filter->comparison);
    //                     column.addFilter(specialFilter);
    //                     dataPointers.push_back(filter);
    //                 }
    //             }        
    //             break;
    //         }
    //         case(SCH_WEEKDAY):
    //         { 
    //             for (BLF_Filter<WeekdayContainer>* filter: file.data.get<BLF_Filter<WeekdayContainer>>())
    //             {
    //                 if (filter->columnIndex == c)
    //                 {
    //                     auto specialFilter = Filter<WeekdayContainer>(filter->getValue());
    //                     specialFilter.setComparison((Comparison)filter->comparison);
    //                     column.addFilter(specialFilter);
    //                     dataPointers.push_back(filter);
    //                 }
    //             }     
    //             break;
    //         }
    //         case(SCH_TIME):
    //         { 
    //             for (BLF_Filter<TimeContainer>* filter: file.data.get<BLF_Filter<TimeContainer>>())
    //             {
    //                 if (filter->columnIndex == c)
    //                 {
    //                     auto specialFilter = Filter<TimeContainer>(filter->getValue());
    //                     specialFilter.setComparison((Comparison)filter->comparison);
    //                     column.addFilter(specialFilter);
    //                     dataPointers.push_back(filter);
    //                 }
    //             }                     
    //             break;
    //         }
    //         case(SCH_DATE):
    //         { 
    //             for (BLF_Filter<DateContainer>* filter: file.data.get<BLF_Filter<DateContainer>>())
    //             {
    //                 if (filter->columnIndex == c)
    //                 {
    //                     auto specialFilter = Filter<DateContainer>(filter->getValue());
    //                     specialFilter.setComparison((Comparison)filter->comparison);
    //                     column.addFilter(specialFilter);
    //                     dataPointers.push_back(filter);
    //                 }
    //             }                   
    //             break;
    //         }
    //         default:
    //         {
    //             printf("DataConverter::readSchedule(%s, schedule): Reading filter of type %d has not been implemented\n", path, columnType);
    //         }
    //     }

    return 0;
}