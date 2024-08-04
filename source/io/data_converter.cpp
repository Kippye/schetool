#include "data_converter.h"

LocalObjectTable& ObjectDefinitions::getObjectTable()
{
    return m_objectTable;
}

const LocalObjectTable& ObjectDefinitions::getObjectTableConst() const
{
    return m_objectTable;
}

TimeWrapper BLF_ElementInfo::getCreationTime() const
{
    return TimeWrapper(
        ClockTimeWrapper(creationHours, creationMinutes),
        creationYear, 
        creationMonth,
        creationMday
    );
}

void DataConverter::setupObjectTable()
{
    addObjectDefinition<BLF_Base>();
    addObjectDefinition<BLF_Date>();
    addObjectDefinition<BLF_FileInfo>();
    addObjectDefinition<BLF_ElementInfo>();
    
    addTypeObjectDefinitions<bool>();
    addTypeObjectDefinitions<int>();
    addTypeObjectDefinitions<double>();
    addTypeObjectDefinitions<std::string>();
    addTypeObjectDefinitions<SelectContainer>();
    addTypeObjectDefinitions<WeekdayContainer>();
    addTypeObjectDefinitions<TimeContainer>();
    addTypeObjectDefinitions<DateContainer>();
}

bool DataConverter::isValidScheduleFile(const char* path) const
{
    try
    {
        // Try to load the file
        FileReadStream stream(path);
        File file = File::fromData(stream);
        // Try to deserialize the file
        file.deserializeBody(m_definitions.getObjectTableConst());
        return true;
    }
    // The file is not valid BLF or some other I/O error occurred, catch error and return false
    // OR Some error occurred, probably due to a mismatch in the object tables, return false
    catch(std::exception& e)
    {
        return false;
    }
}

int DataConverter::writeSchedule(const char* path, const std::vector<Column>& schedule)
{
    FileWriteStream stream(path);

    DataTable data;

    BLF_FileInfo fileInfo = BLF_FileInfo(DateContainer::getCurrentSystemDate().getTime());
    data.insert(getObjectDefinition<BLF_FileInfo>().serialize(fileInfo));

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

std::optional<FileInfo> DataConverter::readSchedule(const char* path, std::vector<Column>& schedule)
{
    std::vector<Column> scheduleCopy = schedule;
    // clear the provided copy just in case
    schedule.clear();
    
    FileReadStream stream(path);

    auto file = File::fromData(stream);

    auto fileBody = file.deserializeBody(m_definitions.getObjectTableConst());

    BLF_FileInfo fileInfo = *fileBody.data.groupby(m_definitions.get<BLF_FileInfo>()).begin();
    FileInfo returnFileInfo = FileInfo(path, TimeWrapper(), fileInfo.editDate.getDate());

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
                printf("DataConverter::readSchedule(%s, schedule): Inserting type of BLF_Column<T> with type %d has not been implemented\n", path, type);
            }
        }
    }

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

                        // Add SelectContainers as listeners to the Column's SelectOptions!!
                        Column& scheduleColumn = schedule.back();
                        for (size_t row = 0; row < scheduleColumn.rows.size(); row++)
                        {
                            scheduleColumn.selectOptions.addListener(row, ((Element<SelectContainer>*)scheduleColumn.getElement(row))->getValueReference());
                        }
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
                printf("DataConverter::readSchedule(%s, schedule): Converting from BLF_Column<T> with type %d has not been implemented\n", path, type);
            }
        }
    }

    return returnFileInfo;
}