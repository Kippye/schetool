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

class BLF_ElementInfo : BLF_Base
{
    public:
        inline static const char* objectName = "BLF_ElementInfo";
        int creationYear;
        int creationMonth;
        int creationMday;
        int creationHours;
        int creationMinutes;

    BLF_ElementInfo() {}
    BLF_ElementInfo(const ElementBase* element)
    {
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
        BLF_Element(const Element<bool>* element) : info(element)
        {
            value = element->getValue();
        }
         
        Element<bool> getElement() const
        {
            tm creationTime = info.getCreationTime();
            return Element<bool>(SCH_BOOL, value, DateContainer(creationTime), TimeContainer(creationTime));
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
        BLF_Element(const Element<int>* element) : info(element)
        {
            value = element->getValue();
        }

        Element<int> getElement() const
        {
            tm creationTime = info.getCreationTime();
            return Element<int>(SCH_NUMBER, value, DateContainer(creationTime), TimeContainer(creationTime));
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
        BLF_Element(const Element<double>* element) : info(element)
        {
            value = element->getValue();
        }

        Element<double> getElement() const
        {
            tm creationTime = info.getCreationTime();
            return Element<double>(SCH_DECIMAL, value, DateContainer(creationTime), TimeContainer(creationTime));
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
        BLF_Element(const Element<std::string>* element) : info(element)
        {
            value = element->getValue();
        }

        Element<std::string> getElement() const
        {
            tm creationTime = info.getCreationTime();
            return Element<std::string>(SCH_TEXT, value, DateContainer(creationTime), TimeContainer(creationTime));
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
        BLF_Element(const Element<SelectContainer>* element) : info(element)
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
            Element<SelectContainer> select = Element<SelectContainer>(SCH_SELECT, SelectContainer(), DateContainer(creationTime), TimeContainer(creationTime));
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
        BLF_Element(const Element<WeekdayContainer>* element) : info(element)
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
            Element<WeekdayContainer> weekday = Element<WeekdayContainer>(SCH_WEEKDAY, WeekdayContainer(), DateContainer(creationTime), TimeContainer(creationTime));
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
        BLF_Element(const Element<TimeContainer>* element) : info(element)
        {
            hours = element->getValue().getHours();
            minutes = element->getValue().getMinutes();
        }

        Element<TimeContainer> getElement() const
        {
            tm creationTime = info.getCreationTime(); 
            return Element<TimeContainer>(SCH_TIME, TimeContainer(hours, minutes), DateContainer(creationTime), TimeContainer(creationTime));
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
        BLF_Element(const Element<DateContainer>* element) : info(element)
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
            return Element<DateContainer>(SCH_DATE, DateContainer(dateTime), DateContainer(creationTime), TimeContainer(creationTime));
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
class BLF_Filter : BLF_Base
{
    private:
        static constexpr const char* getName()
        {
            return "BLF_Filter";
        }
    public:
        inline static const char* objectName = getName();
        BLF_Element<T> passValueElement;
        int comparison;

    BLF_Filter() {}

    BLF_Filter(SCHEDULE_TYPE type, const Filter<T>& filter)
    {
        Element<T> element = Element<T>(type, filter.getPassValue(), DateContainer(), TimeContainer());
        passValueElement = BLF_Element<T>(&element);

        comparison = (int)filter.getComparison();
    }

    Filter<T> getFilter() const
    {
        Filter<T> filter = Filter<T>(passValueElement.getElement().getValue());
        filter.setComparison((Comparison)comparison);
        return filter;
    }

    static void addDefinition(ObjectDefinitions& definitions)
    {
        definitions.add(definitions.getObjectTable().define<BLF_Filter<T>>(objectName,
            arg("passValueElement", &BLF_Filter<T>::passValueElement, definitions.get<BLF_Element<T>>()),
            arg("comparison", &BLF_Filter<T>::comparison)
        ));
    }
};

template <>
constexpr const char* BLF_Filter<bool>::getName()
{
    return "BLF_Filter_bool";
}

template <>
constexpr const char* BLF_Filter<int>::getName()
{
    return "BLF_Filter_int";
}

template <>
constexpr const char* BLF_Filter<double>::getName()
{
    return "BLF_Filter_double";
}

template <>
constexpr const char* BLF_Filter<std::string>::getName()
{
    return "BLF_Filter_string";
}

template <>
constexpr const char* BLF_Filter<SelectContainer>::getName()
{
    return "BLF_Filter_SelectContainer";
}

template <>
constexpr const char* BLF_Filter<WeekdayContainer>::getName()
{
    return "BLF_Filter_WeekdayContainer";
}

template <>
constexpr const char* BLF_Filter<TimeContainer>::getName()
{
    return "BLF_Filter_TimeContainer";
}

template <>
constexpr const char* BLF_Filter<DateContainer>::getName()
{
    return "BLF_Filter_DateContainer";
}

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
        std::vector<BLF_Filter<T>> filters = {};
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

        for (auto filterBase : column->getFiltersConst())
        {
            filters.push_back(BLF_Filter<T>(column->type, *std::dynamic_pointer_cast<Filter<T>>(filterBase)));
        }
    }

    Column getColumn() const
    {
        std::vector<ElementBase*> rows = {};

        // set up SelectOptions
        SelectOptions columnSelectOptions = SelectOptions(selectOptions);
        columnSelectOptions.setIsMutable(selectOptionsMutable);

        // add elements to rows
        for (size_t row = 0; row < elements.size(); row++)
        {
            Element<T>* element = new Element<T>(elements[row].getElement());
            rows.push_back(element);
        }

        Column col = Column(
            rows,
            (SCHEDULE_TYPE)type,
            name,
            permanent,
            (ScheduleColumnFlags)flags,
            (COLUMN_SORT)sort,
            columnSelectOptions
        );

        // add filters to the column
        for (const BLF_Filter<T>& filter: filters)
        {
            col.addFilter<T>(filter.getFilter());
        }

        return col;
    }

    static void addDefinition(ObjectDefinitions& definitions)
    {
        definitions.add(definitions.getObjectTable().define<BLF_Column<T>>(objectName,
            arg("index", &BLF_Column<T>::index),
            arg("type", &BLF_Column<T>::type),
            arg("name", &BLF_Column<T>::name),
            arg("permanent", &BLF_Column<T>::permanent),
            arg("flags", &BLF_Column<T>::flags),
            arg("sort", &BLF_Column<T>::sort),
            arg("selectOptionsMutable", &BLF_Column<T>::selectOptionsMutable),
            arg("selectOptions", &BLF_Column<T>::selectOptions),
            arg("elements", &BLF_Column<T>::elements, definitions.get<BLF_Element<T>>()),
            arg("filters", &BLF_Column<T>::filters, definitions.get<BLF_Filter<T>>())
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