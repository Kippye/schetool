#pragma once
#include "datatable.hpp"
#include "enums.hpp"
#include "objectattribute.hpp"
#include "string.hpp"
#include "templateobject.hpp"
#include <iterator>
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
    private:

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
            {"Option0", &option_0, TYPE_STRING},
            {"Option1", &option_1, TYPE_STRING},
            {"Option2", &option_2, TYPE_STRING},
            {"Option3", &option_3, TYPE_STRING},
            {"Option4", &option_4, TYPE_STRING},
            {"Option5", &option_5, TYPE_STRING},
            {"Option6", &option_6, TYPE_STRING},
            {"Option7", &option_7, TYPE_STRING},
            {"Option8", &option_8, TYPE_STRING},
            {"Option9", &option_9, TYPE_STRING},
            {"Option10", &option_10, TYPE_STRING},
            {"Option11", &option_11, TYPE_STRING},
            {"Option12", &option_12, TYPE_STRING},
            {"Option13", &option_13, TYPE_STRING},
            {"Option14", &option_14, TYPE_STRING},
            {"Option15", &option_15, TYPE_STRING},
            {"Option16", &option_16, TYPE_STRING},
            {"Option17", &option_17, TYPE_STRING},
            {"Option18", &option_18, TYPE_STRING},
            {"Option19", &option_19, TYPE_STRING},
        };
    public:
        int index;
        int type;
        blf::String name;
        bool permanent;
        int flags;
        int sort;
        blf::String option_0;
        blf::String option_1;
        blf::String option_2;
        blf::String option_3;
        blf::String option_4;
        blf::String option_5;
        blf::String option_6;
        blf::String option_7;
        blf::String option_8;
        blf::String option_9;
        blf::String option_10;
        blf::String option_11;
        blf::String option_12;
        blf::String option_13;
        blf::String option_14;
        blf::String option_15;
        blf::String option_16;
        blf::String option_17;
        blf::String option_18;
        blf::String option_19;

    private:
        std::vector<blf::String*> m_optionPointers = {};

    public:
    BLF_Column() 
    {
        m_optionPointers =
        {
            &option_0,
            &option_1,
            &option_2,
            &option_3,
            &option_4,
            &option_5,
            &option_6,
            &option_7,
            &option_8,
            &option_9,
            &option_10,
            &option_11,
            &option_12,
            &option_13,
            &option_14,
            &option_15,
            &option_16,
            &option_17,
            &option_18,
            &option_19,
        };
    }

    BLF_Column(const Column* column, size_t index)
    {
        this->index = (int)index;
        this->type = (int)column->type;
        this->name = column->name;
        this->permanent = column->permanent;
        this->flags = column->flags;
        this->sort = (int)column->sort;

        const std::vector<std::string>& selectOptions = column->selectOptions.getOptions();

        m_optionPointers =
        {
            &option_0,
            &option_1,
            &option_2,
            &option_3,
            &option_4,
            &option_5,
            &option_6,
            &option_7,
            &option_8,
            &option_9,
            &option_10,
            &option_11,
            &option_12,
            &option_13,
            &option_14,
            &option_15,
            &option_16,
            &option_17,
            &option_18,
            &option_19,
        };
        
        for (size_t i = 0; i < selectOptions.size(); i++)
        {
            *this->m_optionPointers[i] = blf::String(selectOptions[i]);
        }
    }

    std::vector<std::string> getSelectOptions()
    {
        std::vector<std::string> options = {};

        for (size_t i = 0; i < std::size(m_optionPointers); i++)
        {
            std::cout << i << std::endl;
            std::cout << option_0 << std::endl;
            std::cout << *m_optionPointers[i] << std::endl;
            std::cout << "..hi?" << std::endl;
            std::cout << m_optionPointers[i]->getLength() << std::endl;
            std::cout << "..hi?" << std::endl;
            if (m_optionPointers[i]->getLength() > 0)
            {
                options.push_back(std::string(m_optionPointers[i]->getBuffer()));
            }
            else
            {
                std::cout << "broke (in a good way)" << std::endl;
                break;
            }
        }

        return options;
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

        // Write the Columns of a Schedule to a file at the given path.
        int writeSchedule(const char* path, const std::vector<Column>&);
        // Read a Schedule from path and return the Columns containing the correct Elements. NOTE: The provided vector will be modified. Provide a copy.
        int readSchedule(const char* path, std::vector<Column>&);
        
        void write(const char* path, const Element* element);
        Element* read(const char* path);
};