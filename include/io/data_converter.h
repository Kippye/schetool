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
using namespace blf::file;

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
        static void addDefinition(ObjectDefinitions& objectTable)
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

class BLF_ElementInfo : BLF_Base
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
        type = element->getType();
        creationYear = element->getCreationDate().getTime().tm_year;
        creationMonth = element->getCreationDate().getTime().tm_mon;
        creationMday = element->getCreationDate().getTime().tm_mday;
        creationHours = element->getCreationTime().hours;
        creationMinutes = element->getCreationTime().minutes;
    }

    tm getCreationTime() const;

    static void addDefinition(ObjectDefinitions& definitions)
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
        BLF_ElementInfo info;
        bool value;

        BLF_Element() {}
        BLF_Element(const Element<bool>* element, size_t columnIndex = 0) : info(element, columnIndex)
        {
            value = element->getValue();
        }
         
        Element<bool> getElement() const
        {
            tm creationTime = info.getCreationTime();
            return Element<bool>((SCHEDULE_TYPE)info.type, value, DateContainer(creationTime), TimeContainer(creationTime));
        }

        static void addDefinition(ObjectDefinitions& definitions)
        {
            definitions.add(definitions.getObjectTable().define<BLF_Element<bool>>(objectName,
                arg("info", &BLF_Element<bool>::info, definitions.get<BLF_ElementInfo>()),
                arg("value", &BLF_Element<bool>::value)
            ));
        }
};

template <>
class BLF_Element<int> : BLF_Base
{
    public:
        inline static const char* objectName = "BLF_Element_int";
        BLF_ElementInfo info;
        int value;

        BLF_Element() {}
        BLF_Element(const Element<int>* element, size_t columnIndex = 0) : info(element, columnIndex)
        {
            value = element->getValue();
        }

        Element<int> getElement() const
        {
            tm creationTime = info.getCreationTime();
            return Element<int>((SCHEDULE_TYPE)info.type, value, DateContainer(creationTime), TimeContainer(creationTime));
        }

        static void addDefinition(ObjectDefinitions& definitions)
        {
            definitions.add(definitions.getObjectTable().define<BLF_Element<int>>(objectName,
                arg("info", &BLF_Element<int>::info, definitions.get<BLF_ElementInfo>()),
                arg("value", &BLF_Element<int>::value)
            ));
        }
};

template <>
class BLF_Element<double> : BLF_Base
{
    public:
        inline static const char* objectName = "BLF_Element_double";
        BLF_ElementInfo info;
        double value;

        BLF_Element() {}
        BLF_Element(const Element<double>* element, size_t columnIndex = 0) : info(element, columnIndex)
        {
            value = element->getValue();
        }

        Element<double> getElement() const
        {
            tm creationTime = info.getCreationTime();
            return Element<double>((SCHEDULE_TYPE)info.type, value, DateContainer(creationTime), TimeContainer(creationTime));
        }

        static void addDefinition(ObjectDefinitions& definitions)
        {
            definitions.add(definitions.getObjectTable().define<BLF_Element<double>>(objectName,
                arg("info", &BLF_Element<double>::info, definitions.get<BLF_ElementInfo>()),
                arg("value", &BLF_Element<double>::value)
            ));
        }
};

template <>
class BLF_Element<std::string> : BLF_Base
{
    public:
        inline static const char* objectName = "BLF_Element_string";
        BLF_ElementInfo info;
        std::string value;

        BLF_Element() {}
        BLF_Element(const Element<std::string>* element, size_t columnIndex = 0) : info(element, columnIndex)
        {
            value = element->getValue();
        }

        Element<std::string> getElement() const
        {
            tm creationTime = info.getCreationTime();
            return Element<std::string>((SCHEDULE_TYPE)info.type, value, DateContainer(creationTime), TimeContainer(creationTime));
        }
        
        static void addDefinition(ObjectDefinitions& definitions)
        {
            definitions.add(definitions.getObjectTable().define<BLF_Element<std::string>>(objectName,
                arg("info", &BLF_Element<std::string>::info, definitions.get<BLF_ElementInfo>()),
                arg("value", &BLF_Element<std::string>::value)
            ));
        }
};

template <>
class BLF_Element<SelectContainer> : BLF_Base
{
    public:
        inline static const char* objectName = "BLF_Element_SelectContainer";
        BLF_ElementInfo info;
        std::vector<size_t> selectionIndices = {};

        BLF_Element() {}
        BLF_Element(const Element<SelectContainer>* element, size_t columnIndex = 0) : info(element, columnIndex)
        {
            const std::set<size_t>& selection = element->getValue().getSelection();
        
            for (size_t s: selection)
            {
                selectionIndices.push_back(s);
            }
        }
          
        std::set<size_t> getSelection() const
        {
            std::set<size_t> selection = {};

            for (size_t i: selectionIndices)
            {
                selection.insert(i);
            }

            return selection;
        }

        Element<SelectContainer> getElement() const
        {
            tm creationTime = info.getCreationTime();
            Element<SelectContainer> select = Element<SelectContainer>((SCHEDULE_TYPE)info.type, SelectContainer(), DateContainer(creationTime), TimeContainer(creationTime));
            select.getValueReference().replaceSelection(getSelection());
            return select;
        }

        static void addDefinition(ObjectDefinitions& definitions)
        {
            definitions.add(definitions.getObjectTable().define<BLF_Element<SelectContainer>>(objectName,
                arg("info", &BLF_Element<SelectContainer>::info, definitions.get<BLF_ElementInfo>()),
                arg("selectionIndices", &BLF_Element<SelectContainer>::selectionIndices)
            ));
        }
};

template <>
class BLF_Element<WeekdayContainer> : BLF_Base
{
    public:
        inline static const char* objectName = "BLF_Element_WeekdayContainer";
        BLF_ElementInfo info;
        std::vector<size_t> selectionIndices = {};

        BLF_Element() {}
        BLF_Element(const Element<WeekdayContainer>* element, size_t columnIndex = 0) : info(element, columnIndex)
        {
            const std::set<size_t>& selection = element->getValue().getSelection();
        
            for (size_t s: selection)
            {
                selectionIndices.push_back(s);
            }
        }
          
        std::set<size_t> getSelection() const
        {
            std::set<size_t> selection = {};

            for (size_t i: selectionIndices)
            {
                // only insert valid weekday indices
                if (i < 7)
                {
                    selection.insert(i);
                }
            }

            return selection;
        }

        Element<WeekdayContainer> getElement() const
        {
            tm creationTime = info.getCreationTime();
            Element<WeekdayContainer> weekday = Element<WeekdayContainer>((SCHEDULE_TYPE)info.type, WeekdayContainer(), DateContainer(creationTime), TimeContainer(creationTime));
            weekday.getValueReference().replaceSelection(getSelection());
            return weekday;
        }

        static void addDefinition(ObjectDefinitions& definitions)
        {
            definitions.add(definitions.getObjectTable().define<BLF_Element<WeekdayContainer>>(objectName,
                arg("info", &BLF_Element<WeekdayContainer>::info, definitions.get<BLF_ElementInfo>()),
                arg("selectionIndices", &BLF_Element<WeekdayContainer>::selectionIndices)
            ));
        }
};

template <>
class BLF_Element<TimeContainer> : BLF_Base
{
    public:
        inline static const char* objectName = "BLF_Element_TimeContainer";
        BLF_ElementInfo info;
        int hours;
        int minutes;

        BLF_Element() {}
        BLF_Element(const Element<TimeContainer>* element, size_t columnIndex = 0) : info(element, columnIndex)
        {
            hours = element->getValue().getHours();
            minutes = element->getValue().getMinutes();
        }

        Element<TimeContainer> getElement() const
        {
            tm creationTime = info.getCreationTime(); 
            return Element<TimeContainer>((SCHEDULE_TYPE)info.type, TimeContainer(hours, minutes), DateContainer(creationTime), TimeContainer(creationTime));
        }

        static void addDefinition(ObjectDefinitions& definitions)
        {
            definitions.add(definitions.getObjectTable().define<BLF_Element<TimeContainer>>(objectName,
                arg("info", &BLF_Element<TimeContainer>::info, definitions.get<BLF_ElementInfo>()),
                arg("hours", &BLF_Element<TimeContainer>::hours),
                arg("minutes", &BLF_Element<TimeContainer>::minutes)
            ));
        }
};

template <>
class BLF_Element<DateContainer> : BLF_Base
{
    public:
        inline static const char* objectName = "BLF_Element_DateContainer";
        BLF_ElementInfo info;
        int year;
        int month;
        int mday;

        BLF_Element() {}
        BLF_Element(const Element<DateContainer>* element, size_t columnIndex = 0) : info(element, columnIndex)
        {
            tm dateTime = element->getValue().getTime();
            year = dateTime.tm_year;
            month = dateTime.tm_mon;
            mday = dateTime.tm_mday;
        }

        Element<DateContainer> getElement() const
        {
            tm creationTime = info.getCreationTime();
            tm dateTime;
            dateTime.tm_year = year;
            dateTime.tm_mon = month;
            dateTime.tm_mday = mday;
            return Element<DateContainer>((SCHEDULE_TYPE)info.type, DateContainer(dateTime), DateContainer(creationTime), TimeContainer(creationTime));
        }

        static void addDefinition(ObjectDefinitions& definitions)
        {
            definitions.add(definitions.getObjectTable().define<BLF_Element<DateContainer>>(objectName,
                arg("info", &BLF_Element<DateContainer>::info, definitions.get<BLF_ElementInfo>()),
                arg("year", &BLF_Element<DateContainer>::year),
                arg("month", &BLF_Element<DateContainer>::month),
                arg("mday", &BLF_Element<DateContainer>::mday)
            ));
        }
};

template <typename T>
class BLF_Column : BLF_Base
{
    private:
        static constexpr const char* getName()
        {
            return "BLF_Column";
        }
    public:
        inline static const char* objectName = getName();
        int index;
        int type;
        std::string name;
        bool permanent;
        int flags;
        int sort;
        bool selectOptionsMutable;
        std::vector<std::string> selectOptions = {};
        std::vector<BLF_Element<T>> elements = {};
    BLF_Column() {}

    BLF_Column(const Column* column, size_t index)
    {
        this->index = (int)index;
        this->type = (int)column->type;
        this->name = column->name;
        this->permanent = column->permanent;
        this->flags = column->flags;
        this->sort = (int)column->sort;
        this->selectOptionsMutable = column->selectOptions.getIsMutable();
 
        selectOptions = column->selectOptions.getOptions();

        for (ElementBase* elementBase: column->rows)
        {
            elements.push_back(BLF_Element<T>((Element<T>*)elementBase));
        }
    }

    Column getColumn() const
    {
        std::vector<ElementBase*> rows = {};

        for (const BLF_Element<T>& blfElement : elements)
        {
            Element<T> element = blfElement.getElement();
            // TODO: add SelectContainer to column's selectoptions listeners
            rows.push_back(new Element<T>(element));
        }

        Column col = Column(
            rows,
            (SCHEDULE_TYPE)type,
            name,
            permanent,
            (ScheduleColumnFlags)flags,
            (COLUMN_SORT)sort,
            selectOptions
        );

        return col;
    }

    static void addDefinition(ObjectDefinitions& definitions)
    {
        std::cout << objectName << std::endl;
        definitions.add(definitions.getObjectTable().define<BLF_Column<T>>(objectName,
            arg("index", &BLF_Column<T>::index),
            arg("type", &BLF_Column<T>::type),
            arg("name", &BLF_Column<T>::name),
            arg("permanent", &BLF_Column<T>::permanent),
            arg("flags", &BLF_Column<T>::flags),
            arg("sort", &BLF_Column<T>::sort),
            arg("selectOptionsMutable", &BLF_Column<T>::selectOptionsMutable),
            arg("selectOptions", &BLF_Column<T>::selectOptions),
            arg("elements", &BLF_Column<T>::elements, definitions.get<BLF_Element<T>>())
        ));
    }
};

template <>
constexpr const char* BLF_Column<bool>::getName()
{
    return "BLF_Column_bool";
}

template <>
constexpr const char* BLF_Column<int>::getName()
{
    return "BLF_Column_int";
}

template <>
constexpr const char* BLF_Column<double>::getName()
{
    return "BLF_Column_double";
}

template <>
constexpr const char* BLF_Column<std::string>::getName()
{
    return "BLF_Column_string";
}

template <>
constexpr const char* BLF_Column<SelectContainer>::getName()
{
    return "BLF_Column_SelectContainer";
}

template <>
constexpr const char* BLF_Column<WeekdayContainer>::getName()
{
    return "BLF_Column_WeekdayContainer";
}

template <>
constexpr const char* BLF_Column<TimeContainer>::getName()
{
    return "BLF_Column_TimeContainer";
}

template <>
constexpr const char* BLF_Column<DateContainer>::getName()
{
    return "BLF_Column_DateContainer";
}

template <typename T>
concept DerivedBlfBase = std::is_base_of<BLF_Base, T>::value;

class DataConverter
{
    private:
        ObjectDefinitions m_definitions;
        
        template <DerivedBlfBase BlfClass>
        void addObjectDefinition()
        {
            BlfClass::addDefinition(m_definitions);
        }
        template <typename T>
        const LocalObjectDefinition<T>& getObjectDefinition()
        {
            return m_definitions.get<T>();
        }
    public:
        void setupObjectTable();

        // Adds the Column (and its elements, filters, etc to the provided DataTable), assuming that the Columns (and their elements, filters) are of the provided type.
        template <typename T>
        void addColumnToData(DataTable& data, const Column& column, size_t columnIndex)
        {
            BLF_Column<T> blfColumn = BLF_Column<T>(&column, columnIndex);
            data.insert(getObjectDefinition<BLF_Column<T>>().serialize(blfColumn));
        }
        // Write the Columns of a Schedule to a file at the given path.
        int writeSchedule(const char* path, const std::vector<Column>&);
        // Read a Schedule from path and return the Columns containing the correct Elements. NOTE: The function creates a copy of the provided vector, but modifies the argument directly. If the function fails at any point, it will be reset to the copy created at the start.
        int readSchedule(const char* path, std::vector<Column>&);
};