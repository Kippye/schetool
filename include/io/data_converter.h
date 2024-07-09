#pragma once
#include <iterator>
#include <map>
#include "blf/include/blf.hpp"
#include "element_base.h"
#include "element.h"
#include "time_container.h"
#include "date_container.h"
#include "select_container.h"
#include "schedule_column.h"
#include "element.h"
using namespace blf;

class BLF_Base;

class ObjectDefinitions
{
    private:
        std::map<std::string, const std::shared_ptr<ObjectDefinition>> m_localObjectDefinitions = {};
        LocalObjectTable m_objectTable;
    public:
        LocalObjectTable& getObjectTable();

        template <typename T>
        void add(LocalObjectDefinition<T>& definition)
        {
            static_assert(std::is_base_of_v<BLF_Base, T> == true);
            m_localObjectDefinitions.insert({definition.getName(), std::make_shared<LocalObjectDefinition<T>>(definition)});
        }
        template <typename T>
        const LocalObjectDefinition<T>& get()
        {
            static_assert(std::is_base_of_v<BLF_Base, T> == true);
            return *std::dynamic_pointer_cast<LocalObjectDefinition<T>>(m_localObjectDefinitions.at(T::objectName));
        }
};

class BLF_Base
{
    public:
        inline static const char* objectName = "BLF_Base";
        virtual void addDefinition(ObjectDefinitions& objectTable)
        {
            objectTable.add(objectTable.getObjectTable().define<BLF_Base>(objectName));
        }
};

// class BLF_FilterBase : public TemplateObject
// {
//     protected:
//         const char* objectName = "BLF_FilterBase";
//         std::vector<ObjectAttribute> attributeMap = 
//         {
//             {"ColumnIndex", &columnIndex, TYPE_INT},
//             {"FilterIndex", &filterIndex, TYPE_INT},
//             {"Type", &type, TYPE_INT},
//             {"Comparison", &comparison, TYPE_INT},
//         };
//     public:
//         int columnIndex;
//         int filterIndex;
//         int type;
//         int comparison;

//     BLF_FilterBase() 
//     {
//     }

//     BLF_FilterBase(Comparison comparison, SCHEDULE_TYPE type, size_t columnIndex = 0, size_t filterIndex = 0)
//     {
//         this->columnIndex = columnIndex;
//         this->filterIndex = filterIndex;
//         this->type = type;
//         this->comparison = (int)comparison;
//     }

//     const char* getObjectName() const override
//     {
//         return objectName;
//     }

//     std::vector<ObjectAttribute> getAttributeMap() override
//     {
//         return attributeMap;
//     }
// };

// template <typename T>
// class BLF_Filter : public BLF_FilterBase {};

// template <>
// class BLF_Filter<bool> : public BLF_FilterBase
// {
//     public:
//         bool passValue;

//         BLF_Filter()
//         {
//             attributeMap.push_back({"PassValue", &passValue, TYPE_BOOL});
//             objectName = "BLF_Filter_Bool";
//         }

//         BLF_Filter(const Filter<bool>* filter, SCHEDULE_TYPE type, size_t columnIndex = 0, size_t filterIndex = 0) : BLF_FilterBase(filter->getComparison(), type, columnIndex, filterIndex)
//         {
//             passValue = filter->getPassValue();
//             attributeMap.push_back({"PassValue", &passValue, TYPE_BOOL});
//             objectName = "BLF_Filter_Bool";
//         }
// };

// template <>
// class BLF_Filter<int> : public BLF_FilterBase
// {
//     public:
//         int passValue;

//         BLF_Filter()
//         {
//             attributeMap.push_back({"PassValue", &passValue, TYPE_INT});
//             objectName = "BLF_Filter_Number";
//         }

//         BLF_Filter(const Filter<int>* filter, SCHEDULE_TYPE type, size_t columnIndex = 0, size_t filterIndex = 0) : BLF_FilterBase(filter->getComparison(), type, columnIndex, filterIndex)
//         {
//             passValue = filter->getPassValue();
//             attributeMap.push_back({"PassValue", &passValue, TYPE_INT});
//             objectName = "BLF_Filter_Number";
//         }
// };

// template <>
// class BLF_Filter<double> : public BLF_FilterBase
// {
//     public:
//         double passValue;

//         BLF_Filter()
//         {
//             attributeMap.push_back({"PassValue", &passValue, TYPE_DOUBLE});
//             objectName = "BLF_Filter_Decimal";
//         }

//         BLF_Filter(const Filter<double>* filter, SCHEDULE_TYPE type, size_t columnIndex = 0, size_t filterIndex = 0) : BLF_FilterBase(filter->getComparison(), type, columnIndex, filterIndex)
//         {
//             passValue = filter->getPassValue();
//             attributeMap.push_back({"PassValue", &passValue, TYPE_DOUBLE});
//             objectName = "BLF_Filter_Decimal";
//         }
// };

// template <>
// class BLF_Filter<std::string> : public BLF_FilterBase
// {
//     public:
//         std::string passValue;

//         BLF_Filter()
//         {
//             attributeMap.push_back({"PassValue", &passValue, TYPE_STRING});
//             objectName = "BLF_Filter_Text";
//         }

//         BLF_Filter(const Filter<std::string>* filter, SCHEDULE_TYPE type, size_t columnIndex = 0, size_t filterIndex = 0) : BLF_FilterBase(filter->getComparison(), type, columnIndex, filterIndex)
//         {
//             passValue = filter->getPassValue();
//             attributeMap.push_back({"PassValue", &passValue, TYPE_STRING});
//             objectName = "BLF_Filter_Text";
//         }
// };

// template <>
// class BLF_Filter<SelectContainer> : public BLF_FilterBase
// {
//     private:
//         std::vector<int*> m_selectionPointers = 
//         {
//             &selection_0,
//             &selection_1,
//             &selection_2,
//             &selection_3,
//             &selection_4,
//             &selection_5,
//             &selection_6,
//             &selection_7,
//             &selection_8,
//             &selection_9,
//             &selection_10,
//             &selection_11,
//             &selection_12,
//             &selection_13,
//             &selection_14,
//             &selection_15,
//             &selection_16,
//             &selection_17,
//             &selection_18,
//             &selection_19,
//         };
//     public:
//         int selection_0 = -1;
//         int selection_1 = -1;
//         int selection_2 = -1;
//         int selection_3 = -1;
//         int selection_4 = -1;
//         int selection_5 = -1;
//         int selection_6 = -1;
//         int selection_7 = -1;
//         int selection_8 = -1;
//         int selection_9 = -1;
//         int selection_10 = -1;
//         int selection_11 = -1;
//         int selection_12 = -1;
//         int selection_13 = -1;
//         int selection_14 = -1;
//         int selection_15 = -1;
//         int selection_16 = -1;
//         int selection_17 = -1;
//         int selection_18 = -1;
//         int selection_19 = -1;

//         BLF_Filter() 
//         {
//             for (size_t i = 0; i < m_selectionPointers.size(); i++)
//             {
//                 attributeMap.push_back({std::string("Selection").append(std::to_string(i)).c_str(), m_selectionPointers[i], TYPE_INT});
//             }

//             objectName = "BLF_Filter_Select";
//         }
//         BLF_Filter(const Filter<SelectContainer>* filter, SCHEDULE_TYPE type, size_t columnIndex = 0, size_t filterIndex = 0) : BLF_FilterBase(filter->getComparison(), type, columnIndex, filterIndex)
//         {
//             const std::set<size_t>& selection = filter->getPassValue().getSelection();
        
//             size_t i = 0;
//             for (size_t s: selection)
//             {
//                 *this->m_selectionPointers[i] = (int)s;
//                 i++;
//             }

//             for (i = 0; i < m_selectionPointers.size(); i++)
//             {
//                 attributeMap.push_back({std::string("Selection").append(std::to_string(i)).c_str(), m_selectionPointers[i], TYPE_INT});
//             }

//             objectName = "BLF_Filter_Select";
//         }
          
//         SelectContainer getValue()
//         {
//             std::set<size_t> selection = {};

//             for (size_t i = 0; i < std::size(m_selectionPointers); i++)
//             {
//                 if (*m_selectionPointers[i] != -1)
//                 {
//                     selection.insert(*m_selectionPointers[i]);
//                 }
//                 else
//                 {
//                     break;
//                 }
//             }

//             SelectContainer value;
//             value.replaceSelection(selection);
//             return value;
//         }
// };

// template <>
// class BLF_Filter<WeekdayContainer> : public BLF_FilterBase
// {
//     private:
//         std::vector<int*> m_selectionPointers = 
//         {
//             &selection_0,
//             &selection_1,
//             &selection_2,
//             &selection_3,
//             &selection_4,
//             &selection_5,
//             &selection_6,
//         };
//     public:
//         BLF_Filter() 
//         {
//             for (size_t i = 0; i < m_selectionPointers.size(); i++)
//             {
//                 attributeMap.push_back({std::string("Selection").append(std::to_string(i)).c_str(), m_selectionPointers[i], TYPE_INT});
//             }

//             objectName = "BLF_Filter_Weekday";
//         }
//         BLF_Filter(const Filter<WeekdayContainer>* filter, SCHEDULE_TYPE type, size_t columnIndex = 0, size_t filterIndex = 0 ) : BLF_FilterBase(filter->getComparison(), type, columnIndex, filterIndex)
//         {
//             const std::set<size_t>& selection = filter->getPassValue().getSelection();
        
//             size_t i = 0;
//             for (size_t s: selection)
//             {
//                 *this->m_selectionPointers[i] = (int)s;
//                 i++;
//             }

//             for (i = 0; i < m_selectionPointers.size(); i++)
//             {
//                 attributeMap.push_back({std::string("Selection").append(std::to_string(i)).c_str(), m_selectionPointers[i], TYPE_INT});
//             }

//             objectName = "BLF_Filter_Weekday";
//         }

//     public:
//         int selection_0 = -1;
//         int selection_1 = -1;
//         int selection_2 = -1;
//         int selection_3 = -1;
//         int selection_4 = -1;
//         int selection_5 = -1;
//         int selection_6 = -1;
          
//         WeekdayContainer getValue()
//         {
//             std::set<size_t> selection = {};

//             for (size_t i = 0; i < std::size(m_selectionPointers); i++)
//             {
//                 if (*m_selectionPointers[i] != -1)
//                 {
//                     selection.insert(*m_selectionPointers[i]);
//                 }
//                 else
//                 {
//                     break;
//                 }
//             }

//             WeekdayContainer value;
//             value.replaceSelection(selection);
//             return value;
//         }
// };

// template <>
// class BLF_Filter<TimeContainer> : public BLF_FilterBase
// {
//     public:
//         int hours;
//         int minutes;
//         BLF_Filter() 
//         {
//             attributeMap.push_back({"Hours", &hours, TYPE_INT});
//             attributeMap.push_back({"Minutes", &minutes, TYPE_INT});
//             objectName = "BLF_Filter_Time";
//         }
//         BLF_Filter(const Filter<TimeContainer>* filter, SCHEDULE_TYPE type, size_t columnIndex = 0, size_t filterIndex = 0) : BLF_FilterBase(filter->getComparison(), type, columnIndex, filterIndex)
//         {
//             hours = filter->getPassValue().getHours();
//             minutes = filter->getPassValue().getMinutes();
//             attributeMap.push_back({"Hours", &hours, TYPE_INT});
//             attributeMap.push_back({"Minutes", &minutes, TYPE_INT});
//             objectName = "BLF_Filter_Time";
//         }

//         TimeContainer getValue()
//         {
//             return TimeContainer(hours, minutes);
//         }
// };

// template <>
// class BLF_Filter<DateContainer> : public BLF_FilterBase
// {
//     public:
//         int year;
//         int month;
//         int mday;
//         BLF_Filter() 
//         {
//             attributeMap.push_back({"Year", &year, TYPE_INT});
//             attributeMap.push_back({"Month", &month, TYPE_INT});
//             attributeMap.push_back({"Mday", &mday, TYPE_INT});
//             objectName = "BLF_Filter_Date";
//         }
//         BLF_Filter(const Filter<DateContainer>* filter, SCHEDULE_TYPE type, size_t columnIndex = 0, size_t filterIndex = 0) : BLF_FilterBase(filter->getComparison(), type, columnIndex, filterIndex)
//         {
//             tm dateTime = filter->getPassValue().getTime();
//             year = dateTime.tm_year;
//             month = dateTime.tm_mon;
//             mday = dateTime.tm_mday;
//             attributeMap.push_back({"Year", &year, TYPE_INT});
//             attributeMap.push_back({"Month", &month, TYPE_INT});
//             attributeMap.push_back({"Mday", &mday, TYPE_INT});
//             objectName = "BLF_Filter_Date";
//         }

//         DateContainer getValue()
//         {
//             return DateContainer(tm{0, 0, 0, mday, month, year});
//         }
// };


// class BLF_Column : public TemplateObject
// {
//     private:
//     protected:
//         const char* objectName = "BLF_Column";
//         std::vector<ObjectAttribute> attributeMap = 
//         {
//             {"Index", &index, TYPE_INT},
//             {"Type", &type, TYPE_INT},
//             {"Name", &name, TYPE_STRING},
//             {"Permanent", &permanent, TYPE_BOOL},
//             {"Flags", &flags, TYPE_INT},
//             {"Sort", &sort, TYPE_INT},
//             {"SelectOptionsMutable", &selectOptionsMutable, TYPE_BOOL},
//             {"Option0", &option_0, TYPE_STRING},
//             {"Option1", &option_1, TYPE_STRING},
//             {"Option2", &option_2, TYPE_STRING},
//             {"Option3", &option_3, TYPE_STRING},
//             {"Option4", &option_4, TYPE_STRING},
//             {"Option5", &option_5, TYPE_STRING},
//             {"Option6", &option_6, TYPE_STRING},
//             {"Option7", &option_7, TYPE_STRING},
//             {"Option8", &option_8, TYPE_STRING},
//             {"Option9", &option_9, TYPE_STRING},
//             {"Option10", &option_10, TYPE_STRING},
//             {"Option11", &option_11, TYPE_STRING},
//             {"Option12", &option_12, TYPE_STRING},
//             {"Option13", &option_13, TYPE_STRING},
//             {"Option14", &option_14, TYPE_STRING},
//             {"Option15", &option_15, TYPE_STRING},
//             {"Option16", &option_16, TYPE_STRING},
//             {"Option17", &option_17, TYPE_STRING},
//             {"Option18", &option_18, TYPE_STRING},
//             {"Option19", &option_19, TYPE_STRING},
//         };
//     public:
//         int index;
//         int type;
//         blf::String name;
//         bool permanent;
//         int flags;
//         int sort;
//         bool selectOptionsMutable;
//         blf::String option_0;
//         blf::String option_1;
//         blf::String option_2;
//         blf::String option_3;
//         blf::String option_4;
//         blf::String option_5;
//         blf::String option_6;
//         blf::String option_7;
//         blf::String option_8;
//         blf::String option_9;
//         blf::String option_10;
//         blf::String option_11;
//         blf::String option_12;
//         blf::String option_13;
//         blf::String option_14;
//         blf::String option_15;
//         blf::String option_16;
//         blf::String option_17;
//         blf::String option_18;
//         blf::String option_19;

//     private:
//         std::vector<blf::String*> m_optionPointers = 
//         {
//             &option_0,
//             &option_1,
//             &option_2,
//             &option_3,
//             &option_4,
//             &option_5,
//             &option_6,
//             &option_7,
//             &option_8,
//             &option_9,
//             &option_10,
//             &option_11,
//             &option_12,
//             &option_13,
//             &option_14,
//             &option_15,
//             &option_16,
//             &option_17,
//             &option_18,
//             &option_19,
//         };

//     public:
//     BLF_Column() {}

//     BLF_Column(const Column* column, size_t index)
//     {
//         this->index = (int)index;
//         this->type = (int)column->type;
//         this->name = column->name;
//         this->permanent = column->permanent;
//         this->flags = column->flags;
//         this->sort = (int)column->sort;
//         this->selectOptionsMutable = column->selectOptions.getIsMutable();

//         const std::vector<std::string>& selectOptions = column->selectOptions.getOptions();
        
//         for (size_t i = 0; i < selectOptions.size(); i++)
//         {
//             *this->m_optionPointers[i] = blf::String(selectOptions[i]);
//         }
//     }

//     std::vector<std::string> getSelectOptions()
//     {
//         std::vector<std::string> options = {};

//         for (size_t i = 0; i < std::size(m_optionPointers); i++)
//         {
//             if (m_optionPointers[i]->getLength() > 0)
//             {
//                 options.push_back(std::string(m_optionPointers[i]->getBuffer()));
//             }
//             else
//             {
//                 break;
//             }
//         }

//         return options;
//     }

//     const char* getObjectName() const override
//     {
//         return objectName;
//     }

//     std::vector<ObjectAttribute> getAttributeMap() override
//     {
//         return attributeMap;
//     }
// };

class BLF_ElementInfo : public BLF_Base
{
    public:
        inline static const char* objectName = "BLF_ElementInfo";
        int columnIndex;
        int type;
        int creationYear;
        int creationMonth;
        int creationMday;
        int creationHours;
        int creationMinutes;

    BLF_ElementInfo() {}
    BLF_ElementInfo(const ElementBase* element, size_t columnIndex = 0)
    {
        this->columnIndex = columnIndex;
        this->type = element->getType();
        this->creationYear = element->getCreationDate().getTime().tm_year;
        this->creationMonth = element->getCreationDate().getTime().tm_mon;
        this->creationMday = element->getCreationDate().getTime().tm_mday;
        this->creationHours = element->getCreationTime().hours;
        this->creationMinutes = element->getCreationTime().minutes;
    }

    void addDefinition(ObjectDefinitions& definitions) override
    {
        definitions.add(definitions.getObjectTable().define<BLF_ElementInfo>(objectName, 
            arg("columnIndex", &BLF_ElementInfo::columnIndex),
            arg("type", &BLF_ElementInfo::type),
            arg("creationYear", &BLF_ElementInfo::creationYear),
            arg("creationMonth", &BLF_ElementInfo::creationMonth),
            arg("creationMday", &BLF_ElementInfo::creationMday),
            arg("creationHours", &BLF_ElementInfo::creationHours),
            arg("creationMinutes", &BLF_ElementInfo::creationMinutes)
        ));
    }
};

template <typename T>
class BLF_Element : BLF_Base {};

template <>
class BLF_Element<bool> : BLF_Base
{
    public:
        inline static const char* objectName = "BLF_Element_bool";
        BLF_ElementInfo base;
        bool value;

        BLF_Element() {}
        BLF_Element(const Element<bool>* element, size_t columnIndex = 0) : base(element, columnIndex)
        {
            value = element->getValue();
        }

        void addDefinition(ObjectDefinitions& definitions) override
        {
            definitions.add(definitions.getObjectTable().define<BLF_Element<bool>>(objectName,
                arg("base", &BLF_Element<bool>::base, definitions.get<BLF_ElementInfo>()),
                arg("value", &BLF_Element<bool>::value)
            ));
        }
};

// template <>
// class BLF_Element<int> : public BLF_ElementInfo
// {
//     public:
//         int value;
//         BLF_Element() 
//         {
//             attributeMap.push_back({"Value", &value, TYPE_INT});
//             objectName = "BLF_Element_int";
//         }
//         BLF_Element(const Element<int>* element, size_t columnIndex = 0) : BLF_ElementInfo(element, columnIndex)
//         {
//             value = element->getValue();
//             attributeMap.push_back({"Value", &value, TYPE_INT});
//             objectName = "BLF_Element_int";
//         }
// };

// template <>
// class BLF_Element<double> : public BLF_ElementInfo
// {
//     public:
//         double value;
//         BLF_Element() 
//         {
//             attributeMap.push_back({"Value", &value, TYPE_DOUBLE});
//             objectName = "BLF_Element_double";
//         }
//         BLF_Element(const Element<double>* element, size_t columnIndex = 0) : BLF_ElementInfo(element, columnIndex)
//         {
//             value = element->getValue();
//             attributeMap.push_back({"Value", &value, TYPE_DOUBLE});
//             objectName = "BLF_Element_double";
//         }
// };

// template <>
// class BLF_Element<std::string> : public BLF_ElementInfo
// {
//     public:
//         blf::String value;
//         BLF_Element() 
//         {
//             attributeMap.push_back({"Value", &value, TYPE_STRING});
//             objectName = "BLF_Element_string";
//         }
//         BLF_Element(const Element<std::string>* element, size_t columnIndex = 0) : BLF_ElementInfo(element, columnIndex)
//         {
//             value = blf::String(element->getValue());
//             attributeMap.push_back({"Value", &value, TYPE_STRING});
//             objectName = "BLF_Element_string";
//         }
// };

// template <>
// class BLF_Element<SelectContainer> : public BLF_ElementInfo
// {
//     private:
//         std::vector<int*> m_selectionPointers = 
//         {
//             &selection_0,
//             &selection_1,
//             &selection_2,
//             &selection_3,
//             &selection_4,
//             &selection_5,
//             &selection_6,
//             &selection_7,
//             &selection_8,
//             &selection_9,
//             &selection_10,
//             &selection_11,
//             &selection_12,
//             &selection_13,
//             &selection_14,
//             &selection_15,
//             &selection_16,
//             &selection_17,
//             &selection_18,
//             &selection_19,
//         };
//     public:
//         int selection_0 = -1;
//         int selection_1 = -1;
//         int selection_2 = -1;
//         int selection_3 = -1;
//         int selection_4 = -1;
//         int selection_5 = -1;
//         int selection_6 = -1;
//         int selection_7 = -1;
//         int selection_8 = -1;
//         int selection_9 = -1;
//         int selection_10 = -1;
//         int selection_11 = -1;
//         int selection_12 = -1;
//         int selection_13 = -1;
//         int selection_14 = -1;
//         int selection_15 = -1;
//         int selection_16 = -1;
//         int selection_17 = -1;
//         int selection_18 = -1;
//         int selection_19 = -1;

//         BLF_Element() 
//         {
//             for (size_t i = 0; i < m_selectionPointers.size(); i++)
//             {
//                 attributeMap.push_back({std::string("Selection").append(std::to_string(i)).c_str(), m_selectionPointers[i], TYPE_INT});
//             }

//             objectName = "BLF_Element_SelectContainer";
//         }
//         BLF_Element(const Element<SelectContainer>* element, size_t columnIndex = 0) : BLF_ElementInfo(element, columnIndex)
//         {
//             const std::set<size_t>& selection = element->getValue().getSelection();
        
//             size_t i = 0;
//             for (size_t s: selection)
//             {
//                 *this->m_selectionPointers[i] = (int)s;
//                 i++;
//             }

//             for (i = 0; i < m_selectionPointers.size(); i++)
//             {
//                 attributeMap.push_back({std::string("Selection").append(std::to_string(i)).c_str(), m_selectionPointers[i], TYPE_INT});
//             }

//             objectName = "BLF_Element_SelectContainer";
//         }
          
//         std::set<size_t> getSelection()
//         {
//             std::set<size_t> selection = {};

//             for (size_t i = 0; i < std::size(m_selectionPointers); i++)
//             {
//                 if (*m_selectionPointers[i] != -1)
//                 {
//                     selection.insert(*m_selectionPointers[i]);
//                 }
//                 else
//                 {
//                     break;
//                 }
//             }

//             return selection;
//         }
// };

// template <>
// class BLF_Element<WeekdayContainer> : public BLF_ElementInfo
// {
//     private:
//         std::vector<int*> m_selectionPointers = 
//         {
//             &selection_0,
//             &selection_1,
//             &selection_2,
//             &selection_3,
//             &selection_4,
//             &selection_5,
//             &selection_6,
//         };
//     public:
//         BLF_Element() 
//         {
//             for (size_t i = 0; i < m_selectionPointers.size(); i++)
//             {
//                 attributeMap.push_back({std::string("Selection").append(std::to_string(i)).c_str(), m_selectionPointers[i], TYPE_INT});
//             }

//             objectName = "BLF_Element_WeekdayContainer";
//         }
//         BLF_Element(const Element<WeekdayContainer>* element, size_t columnIndex = 0) : BLF_ElementInfo(element, columnIndex)
//         {
//             const std::set<size_t>& selection = element->getValue().getSelection();
        
//             size_t i = 0;
//             for (size_t s: selection)
//             {
//                 *this->m_selectionPointers[i] = (int)s;
//                 i++;
//             }

//             for (i = 0; i < m_selectionPointers.size(); i++)
//             {
//                 attributeMap.push_back({std::string("Selection").append(std::to_string(i)).c_str(), m_selectionPointers[i], TYPE_INT});
//             }

//             objectName = "BLF_Element_WeekdayContainer";
//         }

//     public:
//         int selection_0 = -1;
//         int selection_1 = -1;
//         int selection_2 = -1;
//         int selection_3 = -1;
//         int selection_4 = -1;
//         int selection_5 = -1;
//         int selection_6 = -1;
          
//         std::set<size_t> getSelection()
//         {
//             std::set<size_t> selection = {};

//             for (size_t i = 0; i < std::size(m_selectionPointers); i++)
//             {
//                 if (*m_selectionPointers[i] != -1)
//                 {
//                     selection.insert(*m_selectionPointers[i]);
//                 }
//                 else
//                 {
//                     break;
//                 }
//             }

//             return selection;
//         }
// };

// template <>
// class BLF_Element<TimeContainer> : public BLF_ElementInfo
// {
//     public:
//         int hours;
//         int minutes;
//         BLF_Element() 
//         {
//             attributeMap.push_back({"Hours", &hours, TYPE_INT});
//             attributeMap.push_back({"Minutes", &minutes, TYPE_INT});
//             objectName = "BLF_Element_TimeContainer";
//         }
//         BLF_Element(const Element<TimeContainer>* element, size_t columnIndex = 0) : BLF_ElementInfo(element, columnIndex)
//         {
//             hours = element->getValue().getHours();
//             minutes = element->getValue().getMinutes();
//             attributeMap.push_back({"Hours", &hours, TYPE_INT});
//             attributeMap.push_back({"Minutes", &minutes, TYPE_INT});
//             objectName = "BLF_Element_TimeContainer";
//         }
// };

// template <>
// class BLF_Element<DateContainer> : public BLF_ElementInfo
// {
//     public:
//         int year;
//         int month;
//         int mday;
//         BLF_Element() 
//         {
//             attributeMap.push_back({"Year", &year, TYPE_INT});
//             attributeMap.push_back({"Month", &month, TYPE_INT});
//             attributeMap.push_back({"Mday", &mday, TYPE_INT});
//             objectName = "BLF_Element_DateContainer";
//         }
//         BLF_Element(const Element<DateContainer>* element, size_t columnIndex = 0) : BLF_ElementInfo(element, columnIndex)
//         {
//             tm dateTime = element->getValue().getTime();
//             year = dateTime.tm_year;
//             month = dateTime.tm_mon;
//             mday = dateTime.tm_mday;
//             attributeMap.push_back({"Year", &year, TYPE_INT});
//             attributeMap.push_back({"Month", &month, TYPE_INT});
//             attributeMap.push_back({"Mday", &mday, TYPE_INT});
//             objectName = "BLF_Element_DateContainer";
//         }
// };

template <typename T>
concept DerivedBlfBase = std::is_base_of<BLF_Base, T>::value;

class DataConverter
{
    private:
        ObjectDefinitions m_definitions;
        
        template <DerivedBlfBase BlfClass>
        void addObjectDefinition()
        {
            BlfClass instance;
            instance.addDefinition(m_definitions);
        }
        template <typename T>
        const LocalObjectDefinition<T>& getObjectDefinition()
        {
            return m_definitions.get<T>();
        }
        tm getElementCreationTime(BLF_ElementInfo* element);
    public:
        void setupObjectTable();

        // Adds the Column (and its elements, filters, etc to the provided DataTable), assuming that the Columns (and their elements, filters) are of the provided type.
        // template <typename T>
        // void addColumnToData(DataTable& data, const Column& column, size_t columnIndex)
        // {
        //     data.addObject(new BLF_Column(&column, columnIndex));

        //     auto filters = column.getFiltersConst();
        //     for (size_t f = 0; f < column.getFilterCount(); f++)
        //     {
        //         Filter<T>* filter = (Filter<T>*)filters.at(f).get();
        //         data.addObject(new BLF_Filter<T>(filter, column.type, columnIndex, f));
        //     }
            
        //     for (size_t r = 0; r < column.rows.size(); r++)
        //     {
        //         const Element<T>* element = (Element<T>*)column.rows[r];
        //         data.addObject(new BLF_Element<T>(element, columnIndex));
        //     }
        // }
        // Write the Columns of a Schedule to a file at the given path.
        int writeSchedule(const char* path, const std::vector<Column>&);
        // Read a Schedule from path and return the Columns containing the correct Elements. NOTE: The function creates a copy of the provided vector, but modifies the argument directly. If the function fails at any point, it will be reset to the copy created at the start.
        int readSchedule(const char* path, std::vector<Column>&);
};