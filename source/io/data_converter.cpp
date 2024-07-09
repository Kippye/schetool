#include "data_converter.h"
#include "element_base.h"
#include "element.h"

using namespace blf::file;

LocalObjectTable& ObjectDefinitions::getObjectTable()
{
    return m_objectTable;
}

tm DataConverter::getElementCreationTime(BLF_ElementInfo* element)
{
    return tm {
        0,
        (int)element->creationMinutes,
        (int)element->creationHours,
        (int)element->creationMday,
        (int)element->creationMonth,
        (int)element->creationYear,
        0,
        0,
        0
    };
}

struct SimpleObject
{
    int a;
    int b;
    int c;
    std::string d;
};

void DataConverter::setupObjectTable()
{
    addObjectDefinition<BLF_Base>();
    addObjectDefinition<BLF_ElementInfo>();
    addObjectDefinition<BLF_Element<bool>>();
//     m_objects =
//     {
//         createDefinition<BLF_FilterBase>(),
//         createDefinition<BLF_Filter<bool>>(),
//         createDefinition<BLF_Filter<int>>(),
//         createDefinition<BLF_Filter<double>>(),
//         createDefinition<BLF_Filter<std::string>>(),
//         createDefinition<BLF_Filter<SelectContainer>>(),
//         createDefinition<BLF_Filter<WeekdayContainer>>(),
//         createDefinition<BLF_Filter<TimeContainer>>(),
//         createDefinition<BLF_Filter<DateContainer>>(),
//         createDefinition<BLF_Column>(),
//         createDefinition<BLF_ElementInfo>(),
//         createDefinition<BLF_Element<bool>>(),
//         createDefinition<BLF_Element<int>>(),
//         createDefinition<BLF_Element<double>>(),
//         createDefinition<BLF_Element<std::string>>(),
//         createDefinition<BLF_Element<SelectContainer>>(),
//         createDefinition<BLF_Element<WeekdayContainer>>(),
//         createDefinition<BLF_Element<TimeContainer>>(),
//         createDefinition<BLF_Element<DateContainer>>(),
//     };
}

int DataConverter::writeSchedule(const char* path, const std::vector<Column>& schedule)
{
    FileWriteStream stream(path);

    BLF_ElementInfo obj1 = { new ElementBase(SCH_NUMBER, DateContainer(), TimeContainer()), 69 };
    BLF_ElementInfo obj2 = { new ElementBase(SCH_TEXT, DateContainer(), TimeContainer()), 420 };
    BLF_Element<bool> elementBool = { new Element<bool>(SCH_BOOL, true, DateContainer(), TimeContainer()), 52 };

    DataTable data;

    data.insert(getObjectDefinition<BLF_ElementInfo>().serialize(obj1));
    data.insert(getObjectDefinition<BLF_ElementInfo>().serialize(obj2));
    data.insert(getObjectDefinition<BLF_Element<bool>>().serialize(elementBool));

    File file(data, m_definitions.getObjectTable(), {blf::CompressionType::None, blf::EncryptionType::None});

    file.serialize(stream);

    // DataTable data;

    // for (size_t c = 0; c < schedule.size(); c++)
    // {
    //     SCHEDULE_TYPE columnType = schedule[c].type;
    //     switch(columnType)
    //     {
    //         case(SCH_BOOL):
    //             addColumnToData<bool>(data, schedule[c], c);
    //             break;
    //         case(SCH_NUMBER):
    //             addColumnToData<int>(data, schedule[c], c);
    //             break;
    //         case(SCH_DECIMAL):
    //             addColumnToData<double>(data, schedule[c], c);
    //             break;
    //         case(SCH_TEXT):
    //             addColumnToData<std::string>(data, schedule[c], c);
    //             break;
    //         case(SCH_SELECT):
    //             addColumnToData<SelectContainer>(data, schedule[c], c);
    //             break;
    //         case(SCH_WEEKDAY):
    //             addColumnToData<WeekdayContainer>(data, schedule[c], c);
    //             break;
    //         case(SCH_TIME):
    //             addColumnToData<TimeContainer>(data, schedule[c], c);
    //             break;
    //         case(SCH_DATE):
    //             addColumnToData<DateContainer>(data, schedule[c], c);
    //             break;
    //         default:
    //             printf("DataConverter::writeSchedule(%s, schedule): Writing Columns of type %d has not been implemented\n", path, schedule[c].type);
    //             break;
    //     }
    // }

    // writeFile(path, m_objects, data);

    // // clean data to seal memory leak
    // std::vector<TemplateObject*> dataPointers = {};

    // for (auto d: data)
    // {
    //     dataPointers.push_back(d);
    // }

    // for (size_t i = dataPointers.size() - 1; i > 0; i--)
    // {
    //     delete dataPointers[i];
    // }

    return 0;
}

int DataConverter::readSchedule(const char* path, std::vector<Column>& schedule)
{
    FileReadStream stream(path);

    auto file = File::fromData(stream);

    auto fileBody = file.deserializeBody(m_definitions.getObjectTable());

    for (const BLF_ElementInfo& obj : fileBody.data.groupby(getObjectDefinition<BLF_ElementInfo>()))
    {
        std::cout << obj.objectName << " " << obj.type << " " << obj.columnIndex << std::endl;
    }

    for (const BLF_Element<bool>& obj : fileBody.data.groupby(getObjectDefinition<BLF_Element<bool>>()))
    {
        std::cout << obj.objectName << " " << obj.value << " " << obj.base.type << " " << obj.base.columnIndex << std::endl;
    }

    // std::vector<Column> scheduleCopy = schedule;

    // BLFFile file = readFile(path, m_objects);

    // // clear the provided copy just in case
    // schedule.clear();

    // // load and create Columns first
    // DataGroup<BLF_Column> loadedColumns = file.data.get<BLF_Column>();
    // // sort in ASCENDING order 
    // //std::sort(columns.begin(), columns.end(), [](BLF_Column* a, BLF_Column* b) {return a->index > b->index; });

    // std::vector<TemplateObject*> dataPointers = {};

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

    //     schedule.push_back(column);

    //     dataPointers.push_back(loadedColumns[c]);
    // }


    // for (size_t t = 0; t < (size_t)SCH_LAST; t++)
    // {
    //     SCHEDULE_TYPE type = (SCHEDULE_TYPE)t;

    //     switch(type)
    //     {
    //         case(SCH_BOOL):
    //         {
    //             for (BLF_Element<bool>* element: file.data.get<BLF_Element<bool>>())
    //             {
    //                 tm creationTime = getElementCreationTime(element);
    //                 schedule[element->columnIndex].rows.push_back(new Element<bool>(type, element->value, DateContainer(creationTime), TimeContainer(creationTime)));
    //                 dataPointers.push_back(element);
    //             }
    //             break;
    //         }
    //         case(SCH_NUMBER):
    //         {
    //             for (BLF_Element<int>* element: file.data.get<BLF_Element<int>>())
    //             {
    //                 tm creationTime = getElementCreationTime(element);
    //                 schedule[element->columnIndex].rows.push_back(new Element<int>(type, element->value, DateContainer(creationTime), TimeContainer(creationTime)));
    //                 dataPointers.push_back(element);
    //             }                
    //             break;
    //         }
    //         case(SCH_DECIMAL):
    //         {
    //             for (BLF_Element<double>* element: file.data.get<BLF_Element<double>>())
    //             {
    //                 tm creationTime = getElementCreationTime(element);
    //                 schedule[element->columnIndex].rows.push_back(new Element<double>(type, element->value, DateContainer(creationTime), TimeContainer(creationTime)));
    //                 dataPointers.push_back(element);
    //             }        
    //             break;
    //         }
    //         case(SCH_TEXT):
    //         {
    //             for (BLF_Element<std::string>* element: file.data.get<BLF_Element<std::string>>())
    //             {
    //                 tm creationTime = getElementCreationTime(element);
    //                 schedule[element->columnIndex].rows.push_back(new Element<std::string>(type, element->value.getBuffer(), DateContainer(creationTime), TimeContainer(creationTime)));
    //                 dataPointers.push_back(element);
    //             }                      
    //             break;
    //         }
    //         case(SCH_SELECT):
    //         { 
    //             for (BLF_Element<SelectContainer>* element: file.data.get<BLF_Element<SelectContainer>>())
    //             {
    //                 tm creationTime = getElementCreationTime(element);
    //                 Element<SelectContainer>* select = new Element<SelectContainer>(type, SelectContainer(), DateContainer(creationTime), TimeContainer(creationTime));
                    
    //                 select->getValueReference().replaceSelection(element->getSelection());
    //                 schedule[element->columnIndex].rows.push_back(select);
    //                 schedule[element->columnIndex].selectOptions.addListener(schedule[element->columnIndex].rows.size() - 1, select->getValueReference());
    //                 dataPointers.push_back(element);
    //             }        
    //             break;
    //         }
    //         case(SCH_WEEKDAY):
    //         { 
    //             for (BLF_Element<WeekdayContainer>* element: file.data.get<BLF_Element<WeekdayContainer>>())
    //             {
    //                 tm creationTime = getElementCreationTime(element);
    //                 Element<WeekdayContainer>* weekday = new Element<WeekdayContainer>(type, WeekdayContainer(), DateContainer(creationTime), TimeContainer(creationTime));
                    
    //                 weekday->getValueReference().replaceSelection(element->getSelection());
    //                 schedule[element->columnIndex].rows.push_back(weekday);
    //                 dataPointers.push_back(element);
    //             }        
    //             break;
    //         }
    //         case(SCH_TIME):
    //         { 
    //             for (BLF_Element<TimeContainer>* element: file.data.get<BLF_Element<TimeContainer>>())
    //             {
    //                 tm creationTime = getElementCreationTime(element);
    //                 schedule[element->columnIndex].rows.push_back(new Element<TimeContainer>(type, TimeContainer(element->hours, element->minutes), DateContainer(creationTime), TimeContainer(creationTime)));
    //                 dataPointers.push_back(element);
    //             }                      
    //             break;
    //         }
    //         case(SCH_DATE):
    //         { 
    //             for (BLF_Element<DateContainer>* element: file.data.get<BLF_Element<DateContainer>>())
    //             {
    //                 tm creationTime = getElementCreationTime(element);
    //                 tm dateTime;
    //                 dateTime.tm_year = element->year;
    //                 dateTime.tm_mon = element->month;
    //                 dateTime.tm_mday = element->mday;
    //                 schedule[element->columnIndex].rows.push_back(new Element<DateContainer>(type, DateContainer(dateTime), DateContainer(creationTime), TimeContainer(creationTime)));
    //                 dataPointers.push_back(element);
    //             }                      
    //             break;
    //         }
    //         default:
    //         {
    //             printf("DataConverter::readSchedule(%s, schedule): Reading elements with type %d has not been implemented\n", path, type);
    //         }
    //     }
    // }

    // // clean loaded data pointers to seal a memory leak
    // for (size_t i = dataPointers.size() - 1; i > 0; i--)
    // {
    //     delete dataPointers[i];
    // }

    return 0;
}