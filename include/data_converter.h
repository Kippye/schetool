#pragma once
#include "datatable.hpp"
#include "enums.hpp"
#include "objectattribute.hpp"
#include "string.hpp"
#include "templateobject.hpp"
#include <schedule.h>
#include <element.h>
#include <bool_container.h>
#include <number_container.h>
#include <decimal_container.h>
#include <text_container.h>
#include <select_container.h>
#include <time_container.h>
#include <date_container.h>
#include <blf.hpp>
using namespace blf;

class BLF_Column : public TemplateObject
{
    protected:
        const char* objectName = "BLF_Column";
        std::vector<ObjectAttribute> attributeMap = 
        {
            {"Index", &index, TYPE_INT},
            {"Type", &type, TYPE_INT},
            {"Name", &name, TYPE_STRING},
            {"Permanent", &permanent, TYPE_BOOL},
            {"Flags", &flags, TYPE_INT},
            {"Sort", &sort, TYPE_INT},
        };
    public:
        int index;
        int type;
        blf::String name;
        bool permanent;
        // TODO: select options
        int flags;
        int sort;

    BLF_Column() {}

    BLF_Column(const Column* column, size_t index)
    {
        this->index = (int)index;
        this->type = (int)column->type;
        this->name = column->name;
        this->permanent = column->permanent;
        this->flags = column->flags;
        this->sort = (int)column->sort;
    }

    const char* getObjectName() const override
    {
        return objectName;
    }

    std::vector<ObjectAttribute> getAttributeMap() override
    {
        return attributeMap;
    }
};

class BLF_Element : public TemplateObject
{
    protected:
        const char* objectName = "BLF_Element";
        std::vector<ObjectAttribute> attributeMap = 
        {
            {"ColumnIndex", &columnIndex, TYPE_INT},
            {"Type", &type, TYPE_INT},
            {"CreationYear", &creationYear, TYPE_INT},
            {"CreationMonth", &creationMonth, TYPE_INT},
            {"CreationMday", &creationMday, TYPE_INT},
            {"CreationHours", &creationHours, TYPE_INT},
            {"CreationMinutes", &creationMinutes, TYPE_INT},
        };
    public:
        int columnIndex;
        int type;
        int creationYear;
        int creationMonth;
        int creationMday;
        int creationHours;
        int creationMinutes;

    BLF_Element() {}

    BLF_Element(const Element* element, size_t columnIndex = 0)
    {
        this->columnIndex = columnIndex;
        this->type = element->getType();
        this->creationYear = element->getCreationDate().time.tm_year;
        this->creationMonth = element->getCreationDate().time.tm_mon;
        this->creationMday = element->getCreationDate().time.tm_mday;
        this->creationHours = element->getCreationTime().hours;
        this->creationMinutes = element->getCreationTime().minutes;
    }

    const char* getObjectName() const override
    {
        return objectName;
    }

    std::vector<ObjectAttribute> getAttributeMap() override
    {
        return attributeMap;
    }
};

class BLF_Bool : public BLF_Element
{
    public:
        bool value;
        BLF_Bool() 
        {
            attributeMap.push_back({"Value", &value, TYPE_BOOL});
            objectName = "BLF_Bool";
        }
        BLF_Bool(const Bool* element, size_t columnIndex = 0) : BLF_Element(element, columnIndex)
        {
            value = element->getValue();
            attributeMap.push_back({"Value", &value, TYPE_BOOL});
            objectName = "BLF_Bool";
        }
};

class BLF_Number : public BLF_Element
{
    public:
        int value;
        BLF_Number() 
        {
            attributeMap.push_back({"Value", &value, TYPE_INT});
            objectName = "BLF_Number";
        }
        BLF_Number(const Number* element, size_t columnIndex = 0) : BLF_Element(element, columnIndex)
        {
            value = element->getValue();
            attributeMap.push_back({"Value", &value, TYPE_INT});
            objectName = "BLF_Number";
        }
};

class BLF_Decimal : public BLF_Element
{
    public:
        double value;
        BLF_Decimal() 
        {
            attributeMap.push_back({"Value", &value, TYPE_DOUBLE});
            objectName = "BLF_Decimal";
        }
        BLF_Decimal(const Decimal* element, size_t columnIndex = 0) : BLF_Element(element, columnIndex)
        {
            value = element->getValue();
            attributeMap.push_back({"Value", &value, TYPE_DOUBLE});
            objectName = "BLF_Decimal";
        }
};

class BLF_Text : public BLF_Element
{
    public:
        blf::String value;
        BLF_Text() 
        {
            attributeMap.push_back({"Value", &value, TYPE_STRING});
            objectName = "BLF_Text";
        }
        BLF_Text(const Text* element, size_t columnIndex = 0) : BLF_Element(element, columnIndex)
        {
            value = blf::String(element->getValue());
            attributeMap.push_back({"Value", &value, TYPE_STRING});
            objectName = "BLF_Text";
        }
};

class BLF_Select : public BLF_Element
{
    public:
        // TODO: finger out
        int value;
        BLF_Select() 
        {
            attributeMap.push_back({"Value", &value, TYPE_INT});
            objectName = "BLF_Select";
        }
        BLF_Select(const Select* element, size_t columnIndex = 0) : BLF_Element(element, columnIndex)
        {
            // TODO value = element->();
            attributeMap.push_back({"Value", &value, TYPE_INT});
            objectName = "BLF_Select";
        }
};

class BLF_Time : public BLF_Element
{
    public:
        int hours;
        int minutes;
        BLF_Time() 
        {
            attributeMap.push_back({"Hours", &hours, TYPE_INT});
            attributeMap.push_back({"Minutes", &minutes, TYPE_INT});
            objectName = "BLF_Time";
        }
        BLF_Time(const Time* element, size_t columnIndex = 0) : BLF_Element(element, columnIndex)
        {
            hours = element->getHours();
            minutes = element->getMinutes();
            attributeMap.push_back({"Hours", &hours, TYPE_INT});
            attributeMap.push_back({"Minutes", &minutes, TYPE_INT});
            objectName = "BLF_Time";
        }
};

class BLF_Date : public BLF_Element
{
    public:
        int year;
        int month;
        int mday;
        BLF_Date() 
        {
            attributeMap.push_back({"Year", &year, TYPE_INT});
            attributeMap.push_back({"Month", &month, TYPE_INT});
            attributeMap.push_back({"Mday", &mday, TYPE_INT});
            objectName = "BLF_Date";
        }
        BLF_Date(const Date* element, size_t columnIndex = 0) : BLF_Element(element, columnIndex)
        {
            const tm* dateTime = element->getTime();
            std::cout << dateTime->tm_mday << std::endl;
            year = dateTime->tm_year;
            month = dateTime->tm_mon;
            mday = dateTime->tm_mday;
            attributeMap.push_back({"Year", &year, TYPE_INT});
            attributeMap.push_back({"Month", &month, TYPE_INT});
            attributeMap.push_back({"Mday", &mday, TYPE_INT});
            objectName = "BLF_Date";
        }
};

class DataConverter
{
    private:
        ObjectTable m_objects;
        tm getElementCreationTime(BLF_Element* element);
    public:
        void setupObjectTable();

        int writeSchedule(const char* path, const std::vector<Column>&);
        int readSchedule(const char* path, std::vector<Column>&);
        
        void write(const char* path, const Element* element);
        Element* read(const char* path);
};