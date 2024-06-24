#include <data_converter.h>
#include <element_base.h>
#include <element.h>

tm DataConverter::getElementCreationTime(BLF_Element* element)
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

void DataConverter::setupObjectTable()
{
    m_objects =
    {
        createDefinition<BLF_Filter<bool>>(),
        createDefinition<BLF_Filter<int>>(),
        createDefinition<BLF_Filter<double>>(),
        createDefinition<BLF_Filter<std::string>>(),
        createDefinition<BLF_Filter<SelectContainer>>(),
        createDefinition<BLF_Filter<WeekdayContainer>>(),
        createDefinition<BLF_Filter<TimeContainer>>(),
        createDefinition<BLF_Filter<DateContainer>>(),
        createDefinition<BLF_Column>(),
        createDefinition<BLF_Element>(),
        createDefinition<BLF_Bool>(),
        createDefinition<BLF_Number>(),
        createDefinition<BLF_Decimal>(),
        createDefinition<BLF_Text>(),
        createDefinition<BLF_Select>(),
        createDefinition<BLF_Weekday>(),
        createDefinition<BLF_Time>(),
        createDefinition<BLF_Date>(),
    };
}

int DataConverter::writeSchedule(const char* path, const std::vector<Column>& schedule)
{
    DataTable data;

    for (size_t c = 0; c < schedule.size(); c++)
    {
        SCHEDULE_TYPE columnType = schedule[c].type;
        data.addObject(new BLF_Column(&schedule[c], c));

        auto filters = schedule[c].getFiltersConst();
        for (size_t f = 0; f < schedule[c].getFilterCount(); f++)
        {
            auto filter = filters.at(f).get();

            switch(columnType)
            {
                case(SCH_BOOL):
                {
                    data.addObject(new BLF_Filter<bool>((Filter<bool>*)filter, columnType, c, f));
                    break;
                }
                case(SCH_NUMBER):
                {
                    data.addObject(new BLF_Filter<int>((Filter<int>*)filter, columnType, c, f));
                    break;
                }
                case(SCH_DECIMAL):
                {
                    data.addObject(new BLF_Filter<double>((Filter<double>*)filter, columnType, c, f));
                    break;
                }
                case(SCH_TEXT):
                {
                    data.addObject(new BLF_Filter<std::string>((Filter<std::string>*)filter, columnType, c, f));
                    break;
                }
                case(SCH_SELECT):
                { 
                    data.addObject(new BLF_Filter<SelectContainer>((Filter<SelectContainer>*)filter, columnType, c, f));
                    break;
                }
                case(SCH_WEEKDAY):
                { 
                    data.addObject(new BLF_Filter<WeekdayContainer>((Filter<WeekdayContainer>*)filter, columnType, c, f));
                    break;
                }
                case(SCH_TIME):
                { 
                    data.addObject(new BLF_Filter<TimeContainer>((Filter<TimeContainer>*)filter, columnType, c, f));
                    break;
                }
                case(SCH_DATE):
                { 
                    data.addObject(new BLF_Filter<DateContainer>((Filter<DateContainer>*)filter, columnType, c, f));
                    break;
                }
                default:
                {
                    printf("DataConverter::writeSchedule(%s, schedule): Tried to write filter with invalid type %d\n", path, schedule[c].type);
                }
            }
        }

        for (size_t r = 0; r < schedule[c].rows.size(); r++)
        {
            const ElementBase* element = schedule[c].rows[r];
            switch(element->getType())
            {
                case(SCH_BOOL):
                {
                    data.addObject(new BLF_Bool((Element<bool>*)element, c));
                    break;
                }
                case(SCH_NUMBER):
                {
                    data.addObject(new BLF_Number((Element<int>*)element, c));
                    break;
                }
                case(SCH_DECIMAL):
                {
                    data.addObject(new BLF_Decimal((Element<double>*)element, c));
                    break;
                }
                case(SCH_TEXT):
                {
                    data.addObject(new BLF_Text((Element<std::string>*)element, c));
                    break;
                }
                case(SCH_SELECT):
                { 
                    data.addObject(new BLF_Select((Element<SelectContainer>*)element, c));
                    break;
                }
                case(SCH_WEEKDAY):
                { 
                    data.addObject(new BLF_Weekday((Element<WeekdayContainer>*)element, c));
                    break;
                }
                case(SCH_TIME):
                { 
                    data.addObject(new BLF_Time((Element<TimeContainer>*)element, c));
                    break;
                }
                case(SCH_DATE):
                { 
                    data.addObject(new BLF_Date((Element<DateContainer>*)element, c));
                    break;
                }
                default:
                {
                    printf("DataConverter::writeSchedule(%s, schedule): Tried to write element with invalid type %d\n", path, element->getType());
                }
            }
        }
    }

    writeFile(path, m_objects, data);

    // clean data to seal memory leak
    std::vector<TemplateObject*> dataPointers = {};

    for (auto d: data)
    {
        dataPointers.push_back(d);
    }

    for (size_t i = dataPointers.size() - 1; i > 0; i--)
    {
        delete dataPointers[i];
    }

    return 0;
}

int DataConverter::readSchedule(const char* path, std::vector<Column>& schedule)
{
    std::vector<Column> scheduleCopy = schedule;

    BLFFile file = readFile(path, m_objects);

    // clear the provided copy just in case
    schedule.clear();

    // load and create Columns first
    DataGroup<BLF_Column> loadedColumns = file.data.get<BLF_Column>();
    // sort in ASCENDING order 
    //std::sort(columns.begin(), columns.end(), [](BLF_Column* a, BLF_Column* b) {return a->index > b->index; });

    std::vector<TemplateObject*> dataPointers = {};

    // loop through the BLF_Columns and add them to the schedule as Columns
    for (size_t c = 0; c < loadedColumns.getSize(); c++)
    {
        Column column = Column(
            std::vector<ElementBase*>{}, 
            (SCHEDULE_TYPE)loadedColumns[c]->type, 
            std::string(loadedColumns[c]->name.getBuffer()),
            loadedColumns[c]->permanent,
            (ScheduleColumnFlags)loadedColumns[c]->flags,
            (COLUMN_SORT)loadedColumns[c]->sort,
            SelectOptions(loadedColumns[c]->getSelectOptions())
        );

        column.selectOptions.setIsMutable(loadedColumns[c]->selectOptionsMutable);

        SCHEDULE_TYPE columnType = (SCHEDULE_TYPE)loadedColumns[c]->type;

        // TEMP get filters
        switch(columnType)
        {
            case(SCH_BOOL):
            {
                for (BLF_Filter<bool>* filter: file.data.get<BLF_Filter<bool>>())
                {
                    if (filter->columnIndex == c)
                    {
                        auto specialFilter = Filter<bool>(filter->value);
                        specialFilter.setComparison((Comparison)filter->comparison);
                        column.addFilter(specialFilter);
                        dataPointers.push_back(filter);
                    }
                }
                break;
            }
            case(SCH_NUMBER):
            {
                for (BLF_Filter<int>* filter: file.data.get<BLF_Filter<int>>())
                {
                    if (filter->columnIndex == c)
                    {
                        auto specialFilter = Filter<int>(filter->value);
                        specialFilter.setComparison((Comparison)filter->comparison);
                        column.addFilter(specialFilter);
                        dataPointers.push_back(filter);
                    }
                }
                break;
            }
            case(SCH_DECIMAL):
            {
                for (BLF_Filter<double>* filter: file.data.get<BLF_Filter<double>>())
                {
                    if (filter->columnIndex == c)
                    {
                        auto specialFilter = Filter<double>(filter->value);
                        specialFilter.setComparison((Comparison)filter->comparison);
                        column.addFilter(specialFilter);
                        dataPointers.push_back(filter);
                    }
                }    
                break;
            }
            case(SCH_TEXT):
            {
                for (BLF_Filter<std::string>* filter: file.data.get<BLF_Filter<std::string>>())
                {
                    if (filter->columnIndex == c)
                    {
                        auto specialFilter = Filter<std::string>(filter->value);
                        specialFilter.setComparison((Comparison)filter->comparison);
                        column.addFilter(specialFilter);
                        dataPointers.push_back(filter);
                    }
                }                 
                break;
            }
            case(SCH_SELECT):
            { 
                for (BLF_Filter<SelectContainer>* filter: file.data.get<BLF_Filter<SelectContainer>>())
                {
                    if (filter->columnIndex == c)
                    {
                        auto specialFilter = Filter<SelectContainer>(filter->getValue());
                        specialFilter.setComparison((Comparison)filter->comparison);
                        column.addFilter(specialFilter);
                        dataPointers.push_back(filter);
                    }
                }        
                break;
            }
            case(SCH_WEEKDAY):
            { 
                for (BLF_Filter<WeekdayContainer>* filter: file.data.get<BLF_Filter<WeekdayContainer>>())
                {
                    if (filter->columnIndex == c)
                    {
                        auto specialFilter = Filter<WeekdayContainer>(filter->getValue());
                        specialFilter.setComparison((Comparison)filter->comparison);
                        column.addFilter(specialFilter);
                        dataPointers.push_back(filter);
                    }
                }     
                break;
            }
            case(SCH_TIME):
            { 
                for (BLF_Filter<TimeContainer>* filter: file.data.get<BLF_Filter<TimeContainer>>())
                {
                    if (filter->columnIndex == c)
                    {
                        auto specialFilter = Filter<TimeContainer>(filter->getValue());
                        specialFilter.setComparison((Comparison)filter->comparison);
                        column.addFilter(specialFilter);
                        dataPointers.push_back(filter);
                    }
                }                     
                break;
            }
            case(SCH_DATE):
            { 
                for (BLF_Filter<DateContainer>* filter: file.data.get<BLF_Filter<DateContainer>>())
                {
                    if (filter->columnIndex == c)
                    {
                        auto specialFilter = Filter<DateContainer>(filter->getValue());
                        specialFilter.setComparison((Comparison)filter->comparison);
                        column.addFilter(specialFilter);
                        dataPointers.push_back(filter);
                    }
                }                   
                break;
            }
            default:
            {
                printf("DataConverter::readSchedule(%s, schedule): Reading filter of type %d has not been implemented\n", path, columnType);
            }
        }

        schedule.push_back(column);

        dataPointers.push_back(loadedColumns[c]);
    }


    for (size_t t = 0; t < (size_t)SCH_LAST; t++)
    {
        SCHEDULE_TYPE type = (SCHEDULE_TYPE)t;

        switch(type)
        {
            case(SCH_BOOL):
            {
                for (BLF_Bool* element: file.data.get<BLF_Bool>())
                {
                    tm creationTime = getElementCreationTime(element);
                    schedule[element->columnIndex].rows.push_back(new Element<bool>(type, element->value, DateContainer(creationTime), TimeContainer(creationTime)));
                    dataPointers.push_back(element);
                }
                break;
            }
            case(SCH_NUMBER):
            {
                for (BLF_Number* element: file.data.get<BLF_Number>())
                {
                    tm creationTime = getElementCreationTime(element);
                    schedule[element->columnIndex].rows.push_back(new Element<int>(type, element->value, DateContainer(creationTime), TimeContainer(creationTime)));
                    dataPointers.push_back(element);
                }                
                break;
            }
            case(SCH_DECIMAL):
            {
                for (BLF_Decimal* element: file.data.get<BLF_Decimal>())
                {
                    tm creationTime = getElementCreationTime(element);
                    schedule[element->columnIndex].rows.push_back(new Element<double>(type, element->value, DateContainer(creationTime), TimeContainer(creationTime)));
                    dataPointers.push_back(element);
                }        
                break;
            }
            case(SCH_TEXT):
            {
                for (BLF_Text* element: file.data.get<BLF_Text>())
                {
                    tm creationTime = getElementCreationTime(element);
                    schedule[element->columnIndex].rows.push_back(new Element<std::string>(type, element->value.getBuffer(), DateContainer(creationTime), TimeContainer(creationTime)));
                    dataPointers.push_back(element);
                }                      
                break;
            }
            case(SCH_SELECT):
            { 
                for (BLF_Select* element: file.data.get<BLF_Select>())
                {
                    tm creationTime = getElementCreationTime(element);
                    Element<SelectContainer>* select = new Element<SelectContainer>(type, SelectContainer(), DateContainer(creationTime), TimeContainer(creationTime));
                    
                    select->getValueReference().replaceSelection(element->getSelection());
                    schedule[element->columnIndex].rows.push_back(select);
                    schedule[element->columnIndex].selectOptions.addListener(schedule[element->columnIndex].rows.size() - 1, select->getValueReference());
                    dataPointers.push_back(element);
                }        
                break;
            }
            case(SCH_WEEKDAY):
            { 
                for (BLF_Weekday* element: file.data.get<BLF_Weekday>())
                {
                    tm creationTime = getElementCreationTime(element);
                    Element<WeekdayContainer>* weekday = new Element<WeekdayContainer>(type, WeekdayContainer(), DateContainer(creationTime), TimeContainer(creationTime));
                    
                    weekday->getValueReference().replaceSelection(element->getSelection());
                    schedule[element->columnIndex].rows.push_back(weekday);
                    dataPointers.push_back(element);
                }        
                break;
            }
            case(SCH_TIME):
            { 
                for (BLF_Time* element: file.data.get<BLF_Time>())
                {
                    tm creationTime = getElementCreationTime(element);
                    schedule[element->columnIndex].rows.push_back(new Element<TimeContainer>(type, TimeContainer(element->hours, element->minutes), DateContainer(creationTime), TimeContainer(creationTime)));
                    dataPointers.push_back(element);
                }                      
                break;
            }
            case(SCH_DATE):
            { 
                for (BLF_Date* element: file.data.get<BLF_Date>())
                {
                    tm creationTime = getElementCreationTime(element);
                    tm dateTime;
                    dateTime.tm_year = element->year;
                    dateTime.tm_mon = element->month;
                    dateTime.tm_mday = element->mday;
                    schedule[element->columnIndex].rows.push_back(new Element<DateContainer>(type, DateContainer(dateTime), DateContainer(creationTime), TimeContainer(creationTime)));
                    dataPointers.push_back(element);
                }                      
                break;
            }
            default:
            {
                printf("DataConverter::readSchedule(%s, schedule): Reading elements with type %d has not been implemented\n", path, type);
            }
        }
    }

    // clean loaded data pointers to seal a memory leak
    for (size_t i = dataPointers.size() - 1; i > 0; i--)
    {
        delete dataPointers[i];
    }

    return 0;
}