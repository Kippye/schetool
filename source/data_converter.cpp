#include "datagroup.hpp"
#include "datatable.hpp"
#include "element.h"
#include "element_base.h"
#include "objectdefinition.hpp"
#include "schedule.h"
#include "templateobject.hpp"
#include <data_converter.h>

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
        createDefinition<BLF_Column>(),
        createDefinition<BLF_Element>(),
        createDefinition<BLF_Bool>(),
        createDefinition<BLF_Number>(),
        createDefinition<BLF_Decimal>(),
        createDefinition<BLF_Text>(),
        createDefinition<BLF_Select>(),
        createDefinition<BLF_Time>(),
        createDefinition<BLF_Date>(),
    };
}

int DataConverter::writeSchedule(const char* path, const std::vector<Column>& schedule)
{
    // TODO: check if path is valid

    DataTable data;

    for (size_t c = 0; c < schedule.size(); c++)
    {
        data.addObject(new BLF_Column(&schedule[c], c));

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
    // TODO: check if path is valid
    BLFFile file = readFile(path, m_objects);

    // clear the provided copy just in case
    schedule.clear();

    // load and create Columns first
    DataGroup<BLF_Column> loadedColumns = file.data.get<BLF_Column>();
    // sort in ASCENDING order 
    //std::sort(columns.begin(), columns.end(), [](BLF_Column* a, BLF_Column* b) {return a->index > b->index; });

    std::vector<TemplateObject*> dataPointers = {};

    // loop through the sorted BLF_Columns and add them to the schedule as Columns
    for (size_t c = 0; c < loadedColumns.getSize(); c++)
    {
        Column column = Column {
            std::vector<ElementBase*>{}, 
            (SCHEDULE_TYPE)loadedColumns[c]->type, 
            std::string(loadedColumns[c]->name.getBuffer()),
            loadedColumns[c]->permanent,
            (ScheduleElementFlags)loadedColumns[c]->flags,
            (COLUMN_SORT)loadedColumns[c]->sort,
            false,
            SelectOptions(loadedColumns[c]->getSelectOptions())
        };

        column.selectOptions.setIsMutable(loadedColumns[c]->selectOptionsMutable);

        schedule.push_back(column);

        dataPointers.push_back(loadedColumns[c]);
    }

    for (size_t t = 0; t <= (size_t)SCH_LAST; t++)
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
                    Element<SelectContainer>* select = new Element<SelectContainer>(type, SelectContainer(schedule[element->columnIndex].selectOptions), DateContainer(creationTime), TimeContainer(creationTime));
                    select->getValue().replaceSelection(element->getSelection());
                    schedule[element->columnIndex].rows.push_back(select);
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
        }
    }

    // clean loaded data pointers to seal a memory leak
    for (size_t i = dataPointers.size() - 1; i > 0; i--)
    {
        delete dataPointers[i];
    }

    return 0;
}