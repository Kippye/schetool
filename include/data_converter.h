#pragma once
#include "datatable.hpp"
#include "enums.hpp"
#include "objectattribute.hpp"
#include "string.hpp"
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

class BLF_Element : public TemplateObject
{
    protected:
        const char* objectName = "BLF_Element";
        std::vector<ObjectAttribute> attributeMap = 
        {
            {"Type", &type, TYPE_INT},
            {"CreationYear", &creationYear, TYPE_INT},
            {"CreationMonth", &creationMonth, TYPE_INT},
            {"CreationMday", &creationMday, TYPE_INT},
            {"CreationHours", &creationHours, TYPE_INT},
            {"CreationMinutes", &creationMinutes, TYPE_INT},
        };
    public:
        int type;
        int creationYear;
        int creationMonth;
        int creationMday;
        int creationHours;
        int creationMinutes;

    BLF_Element() {}

    BLF_Element(const Element* element)
    {
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
        BLF_Bool(const Bool* element) : BLF_Element(element)
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
        BLF_Number(const Number* element) : BLF_Element(element)
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
        BLF_Decimal(const Decimal* element) : BLF_Element(element)
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
        BLF_Text(const Text* element) : BLF_Element(element)
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
        BLF_Select(const Select* element) : BLF_Element(element)
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
        BLF_Time(const Time* element) : BLF_Element(element)
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
        BLF_Date(const Date* element) : BLF_Element(element)
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
        void write(const char* path, const Element* element);
        Element* read(const char* path);
};