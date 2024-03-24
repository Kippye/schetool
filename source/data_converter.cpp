#include "datatable.hpp"
#include "decimal_container.h"
#include "element.h"
#include "objectdefinition.hpp"
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
    InformationHeader header = {SIGNATURE, VERSION_MAJOR, VERSION_MINOR, VERSION_FIX};
    CommonTable common;

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