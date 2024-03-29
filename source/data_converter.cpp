#include "datagroup.hpp"
#include "datatable.hpp"
#include "decimal_container.h"
#include "element.h"
#include "objectdefinition.hpp"
#include "schedule.h"
#include "select_container.h"
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
            const Element* element = schedule[c].rows[r];
            switch(element->getType())
            {
                case(SCH_BOOL):
                {
                    data.addObject(new BLF_Bool((Bool*)element, c));
                    break;
                }
                case(SCH_NUMBER):
                {
                    data.addObject(new BLF_Number((Number*)element, c));
                    break;
                }
                case(SCH_DECIMAL):
                {
                    data.addObject(new BLF_Decimal((Decimal*)element, c));
                    break;
                }
                case(SCH_TEXT):
                {
                    data.addObject(new BLF_Text((Text*)element, c));
                    break;
                }
                case(SCH_SELECT):
                { 
                    data.addObject(new BLF_Select((Select*)element, c));
                    break;
                }
                case(SCH_TIME):
                { 
                    data.addObject(new BLF_Time((Time*)element, c));
                    break;
                }
                case(SCH_DATE):
                { 
                    data.addObject(new BLF_Date((Date*)element, c));
                    break;
                }
            }
        }
    }

    writeFile(path, m_objects, data);

    // TODO: clean data?
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

    // loop through the sorted BLF_Columns and add them to the schedule as Columns
    for (size_t c = 0; c < loadedColumns.getSize(); c++)
    {
        Column column = Column {
            std::vector<Element*>{}, 
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
                    schedule[element->columnIndex].rows.push_back(new Bool(element->value, type, DateContainer(creationTime), TimeContainer(creationTime)));
                }
                break;
            }
            case(SCH_NUMBER):
            {
                for (BLF_Number* element: file.data.get<BLF_Number>())
                {
                    tm creationTime = getElementCreationTime(element);
                    schedule[element->columnIndex].rows.push_back(new Number(element->value, type, DateContainer(creationTime), TimeContainer(creationTime)));
                }                
                break;
            }
            case(SCH_DECIMAL):
            {
                for (BLF_Decimal* element: file.data.get<BLF_Decimal>())
                {
                    tm creationTime = getElementCreationTime(element);
                    schedule[element->columnIndex].rows.push_back(new Decimal(element->value, type, DateContainer(creationTime), TimeContainer(creationTime)));
                }        
                break;
            }
            case(SCH_TEXT):
            {
                for (BLF_Text* element: file.data.get<BLF_Text>())
                {
                    tm creationTime = getElementCreationTime(element);
                    schedule[element->columnIndex].rows.push_back(new Text(element->value.getBuffer(), type, DateContainer(creationTime), TimeContainer(creationTime)));
                }                      
                break;
            }
            case(SCH_SELECT):
            { 
                for (BLF_Select* element: file.data.get<BLF_Select>())
                {
                    tm creationTime = getElementCreationTime(element);
                    Select* select = new Select(schedule[element->columnIndex].selectOptions, type, DateContainer(creationTime), TimeContainer(creationTime));
                    select->replaceSelection(element->getSelection());
                    schedule[element->columnIndex].rows.push_back(select);
                }        
                break;
            }
            case(SCH_TIME):
            { 
                for (BLF_Time* element: file.data.get<BLF_Time>())
                {
                    tm creationTime = getElementCreationTime(element);
                    schedule[element->columnIndex].rows.push_back(new Time(element->hours, element->minutes, type, DateContainer(creationTime), TimeContainer(creationTime)));
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
                    schedule[element->columnIndex].rows.push_back(new Date(dateTime, type, DateContainer(creationTime), TimeContainer(creationTime)));
                }                      
                break;
            }
        }
    }

    return 0;
}

void DataConverter::write(const char* path, const Element* element)
{
    DataTable data;

    std::vector<TemplateObject*> elements = {};
    switch(element->getType())
    {
        case(SCH_BOOL):
        {
            elements.push_back(new BLF_Bool((Bool*)element));
            break;
        }
        case(SCH_NUMBER):
        {
            elements.push_back(new BLF_Number((Number*)element));
            break;
        }
        case(SCH_DECIMAL):
        {
            elements.push_back(new BLF_Decimal((Decimal*)element));
            break;
        }
        case(SCH_TEXT):
        {
            elements.push_back(new BLF_Text((Text*)element));
            break;
        }
        case(SCH_SELECT):
        { 
            elements.push_back(new BLF_Select((Select*)element));
            break;
        }
        case(SCH_TIME):
        { 
            elements.push_back(new BLF_Time((Time*)element));
            break;
        }
        case(SCH_DATE):
        { 
            elements.push_back(new BLF_Date((Date*)element));
            break;
        }
    }
    std::cout << "write: objName: " << elements[0]->getObjectName() << std::endl;
    std::cout << "write: attribute map: " << std::endl ;
    for (auto c: elements[0]->getAttributeMap())
    {
        std::cout << c.name << std::endl;
        std::cout << c.offset << std::endl;
    }
    data.addObject(elements[0]);

    writeFile("C:\\Users\\Remoa\\Documents\\Devil\\schetool\\test.blf", m_objects, data);
    std::cout << "File successfully written to: " << path << std::endl;
}

Element* DataConverter::read(const char* path)
{
    BLFFile file = readFile(path, m_objects);

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
                    return new Bool(element->value, type, DateContainer(creationTime), TimeContainer(creationTime));
                }
                break;
            }
            case(SCH_NUMBER):
            {
                for (BLF_Number* element: file.data.get<BLF_Number>())
                {
                    tm creationTime = getElementCreationTime(element);
                    return new Number(element->value, type, DateContainer(creationTime), TimeContainer(creationTime));
                }                
                break;
            }
            case(SCH_DECIMAL):
            {
                for (BLF_Decimal* element: file.data.get<BLF_Decimal>())
                {
                    tm creationTime = getElementCreationTime(element);
                    return new Decimal(element->value, type, DateContainer(creationTime), TimeContainer(creationTime));
                }        
                break;
            }
            case(SCH_TEXT):
            {
                for (BLF_Text* element: file.data.get<BLF_Text>())
                {
                    tm creationTime = getElementCreationTime(element);
                    return new Text(element->value.getBuffer(), type, DateContainer(creationTime), TimeContainer(creationTime));
                }                      
                break;
            }
            case(SCH_SELECT):
            { 
                for (BLF_Select* element: file.data.get<BLF_Select>())
                {
                    tm creationTime = getElementCreationTime(element);
                    return nullptr; // TODO! //new Select(element->value, type, DateContainer(creationTime), TimeContainer(creationTime));
                }        
                break;
            }
            case(SCH_TIME):
            { 
                for (BLF_Time* element: file.data.get<BLF_Time>())
                {
                    tm creationTime = getElementCreationTime(element);
                    return new Time(element->hours, element->minutes, type, DateContainer(creationTime), TimeContainer(creationTime));
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
                    return new Date(dateTime, type, DateContainer(creationTime), TimeContainer(creationTime));
                }                      
                break;
            }
        }
    }
    // TEMP
    return nullptr;
}