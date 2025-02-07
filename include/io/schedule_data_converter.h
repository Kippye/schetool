#pragma once
#include <iterator>
#include <optional>
#include <map>
#include "blf/include/blf.hpp"
#include "blf_base_types.h"
#include "file_info.h"
#include "element_base.h"
#include "time_container.h"
#include "date_container.h"
#include "select_container.h"
#include "single_select_container.h"
#include "weekday_container.h"
#include "schedule_column.h"
#include "element.h"

struct BLF_ClockTime : BLF_Base
{
	static constexpr std::string getName()
	{
		return "BLF_ClockTime";
	}

	unsigned int hours;
	unsigned int minutes;

	BLF_ClockTime() : hours(0), minutes(0) 
	{}
	BLF_ClockTime(const ClockTimeWrapper& clockTime) : hours(clockTime.getHours()), minutes(clockTime.getMinutes())
	{}

	ClockTimeWrapper getClockTime() const
	{
		return ClockTimeWrapper(hours, minutes);
	}

	static void addDefinition(ObjectDefinitions& definitions)
	{
		definitions.add(definitions.getObjectTable().define<BLF_ClockTime>(getName(),
			blf::arg("hours", &BLF_ClockTime::hours),
			blf::arg("minutes", &BLF_ClockTime::minutes)
		));
	}
};

struct BLF_Date : BLF_Base
{
	static constexpr std::string getName()
	{
		return "BLF_Date";
	}

	unsigned int year;
	unsigned int month;
	unsigned int monthDay;

	BLF_Date() : year(1900), month(1), monthDay(1) 
	{}
	BLF_Date(const DateWrapper& date) : year(date.getYear()), month(date.getMonth()), monthDay(date.getMonthDay())
	{}

	DateWrapper getDate() const
	{
		return DateWrapper(year, month, monthDay);
	}

	static void addDefinition(ObjectDefinitions& definitions)
	{
		definitions.add(definitions.getObjectTable().define<BLF_Date>(getName(),
			blf::arg("year", &BLF_Date::year),
			blf::arg("month", &BLF_Date::month),
			blf::arg("monthDay", &BLF_Date::monthDay)
		));
	}
};

struct BLF_FileInfo : BLF_Base
{
	static constexpr std::string getName()
	{
		return "BLF_FileInfo";
	}

	BLF_Date editDate;
    BLF_ClockTime editTime;

	BLF_FileInfo() {}
	BLF_FileInfo(const TimeWrapper& fileEditTime) : editDate(fileEditTime.getDateUTC()), editTime(fileEditTime.getClockTimeUTC())
	{}

    TimeWrapper getEditTime() const
    {
        return TimeWrapper(
            editDate.getDate(),
            editTime.getClockTime()
        );
    }

	static void addDefinition(ObjectDefinitions& definitions)
	{
		definitions.add(definitions.getObjectTable().define<BLF_FileInfo>(getName(),
			blf::arg("editDate", &BLF_FileInfo::editDate, definitions.get<BLF_Date>()),
			blf::arg("editTime", &BLF_FileInfo::editTime, definitions.get<BLF_ClockTime>())
		));
	}
};

struct BLF_ElementInfo : BLF_Base
{
    static constexpr std::string getName()
    {
        return "BLF_ElementInfo";
    }
    BLF_Date creationDate;
    BLF_ClockTime creationTime;

    BLF_ElementInfo() {}
    BLF_ElementInfo(const ElementBase* element) : creationDate(element->getCreationTime().getDateUTC()), creationTime(element->getCreationTime().getClockTimeUTC())
    {}

    TimeWrapper getCreationTime() const
    {
        return TimeWrapper(
            creationDate.getDate(),
            creationTime.getClockTime()
        );         
    }

    static void addDefinition(ObjectDefinitions& definitions)
    {
        definitions.add(definitions.getObjectTable().define<BLF_ElementInfo>(getName(), 
            blf::arg("creationDate", &BLF_ElementInfo::creationDate, definitions.get<BLF_Date>()),
            blf::arg("creationTime", &BLF_ElementInfo::creationTime, definitions.get<BLF_ClockTime>())
        ));
    }
};

template <typename T>
struct BLF_Element : BLF_Base {};

template <>
struct BLF_Element<bool> : BLF_Base
{
    static constexpr std::string getName()
    {
        return "BLF_Element_bool";
    }
    BLF_ElementInfo info;
    bool value;

	BLF_Element() {}
	BLF_Element(const Element<bool>* element) : info(element)
	{
		value = element->getValue();
	}
	 
	Element<bool> getElement() const
	{
		return Element<bool>(SCH_BOOL, value, info.getCreationTime());
	}

	static void addDefinition(ObjectDefinitions& definitions)
	{
		definitions.add(definitions.getObjectTable().define<BLF_Element<bool>>(getName(),
			blf::arg("info", &BLF_Element<bool>::info, definitions.get<BLF_ElementInfo>()),
			blf::arg("value", &BLF_Element<bool>::value)
		));
	}
};

template <>
struct BLF_Element<int> : BLF_Base
{
	static constexpr std::string getName()
	{
		return "BLF_Element_int";
	}        
	BLF_ElementInfo info;
	int value;

	BLF_Element() {}
	BLF_Element(const Element<int>* element) : info(element)
	{
		value = element->getValue();
	}

	Element<int> getElement() const
	{
		return Element<int>(SCH_NUMBER, value, info.getCreationTime());
	}

	static void addDefinition(ObjectDefinitions& definitions)
	{
		definitions.add(definitions.getObjectTable().define<BLF_Element<int>>(getName(),
			blf::arg("info", &BLF_Element<int>::info, definitions.get<BLF_ElementInfo>()),
			blf::arg("value", &BLF_Element<int>::value)
		));
	}
};

template <>
struct BLF_Element<double> : BLF_Base
{
	static constexpr std::string getName()
	{
		return "BLF_Element_double";
	}        
	BLF_ElementInfo info;
	double value;

	BLF_Element() {}
	BLF_Element(const Element<double>* element) : info(element)
	{
		value = element->getValue();
	}

	Element<double> getElement() const
	{
		return Element<double>(SCH_DECIMAL, value, info.getCreationTime());
	}

	static void addDefinition(ObjectDefinitions& definitions)
	{
		definitions.add(definitions.getObjectTable().define<BLF_Element<double>>(getName(),
			blf::arg("info", &BLF_Element<double>::info, definitions.get<BLF_ElementInfo>()),
			blf::arg("value", &BLF_Element<double>::value)
		));
	}
};

template <>
struct BLF_Element<std::string> : BLF_Base
{
	static constexpr std::string getName()
	{
		return "BLF_Element_string";
	}        
	BLF_ElementInfo info;
	std::string value;

	BLF_Element() {}
	BLF_Element(const Element<std::string>* element) : info(element)
	{
		value = element->getValue();
	}

	Element<std::string> getElement() const
	{
		return Element<std::string>(SCH_TEXT, value, info.getCreationTime());
	}
    
    static void addDefinition(ObjectDefinitions& definitions)
    {
        definitions.add(definitions.getObjectTable().define<BLF_Element<std::string>>(getName(),
            blf::arg("info", &BLF_Element<std::string>::info, definitions.get<BLF_ElementInfo>()),
            blf::arg("value", &BLF_Element<std::string>::value)
        ));
    }
};

template <>
struct BLF_Element<SingleSelectContainer> : BLF_Base
{
    static constexpr std::string getName()
    {
        return "BLF_Element_SingleSelectContainer";
    }
    BLF_ElementInfo info;
    std::vector<size_t> selectionIndices = {};

    BLF_Element() {}
    BLF_Element(const Element<SingleSelectContainer>* element) : info(element)
    {
        const std::optional<size_t> selection = element->getValue().getSelection();
        if (selection.has_value())
        {
            selectionIndices.push_back(selection.value());
        }
    }

    std::optional<size_t> getSelection() const
    {
        if (selectionIndices.size() == 0)
        {
            return std::nullopt;
        }

        return selectionIndices[0];
    }

	Element<SingleSelectContainer> getElement() const
	{
		Element<SingleSelectContainer> singleSelect = Element<SingleSelectContainer>(SCH_SELECT, SingleSelectContainer(), info.getCreationTime());
		singleSelect.getValueReference().replaceSelection(getSelection());
		return singleSelect;
	}

    static void addDefinition(ObjectDefinitions& definitions)
    {
        definitions.add(definitions.getObjectTable().define<BLF_Element<SingleSelectContainer>>(getName(),
            blf::arg("info", &BLF_Element<SingleSelectContainer>::info, definitions.get<BLF_ElementInfo>()),
            blf::arg("selectionIndices", &BLF_Element<SingleSelectContainer>::selectionIndices)
        ));
    }
};

template <>
struct BLF_Element<SelectContainer> : BLF_Base
{
    static constexpr std::string getName()
    {
        return "BLF_Element_SelectContainer";
    }        
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
		Element<SelectContainer> select = Element<SelectContainer>(SCH_MULTISELECT, SelectContainer(), info.getCreationTime());
		select.getValueReference().replaceSelection(getSelection());
		return select;
	}

    static void addDefinition(ObjectDefinitions& definitions)
    {
        definitions.add(definitions.getObjectTable().define<BLF_Element<SelectContainer>>(getName(),
            blf::arg("info", &BLF_Element<SelectContainer>::info, definitions.get<BLF_ElementInfo>()),
            blf::arg("selectionIndices", &BLF_Element<SelectContainer>::selectionIndices)
        ));
    }
};

template <>
struct BLF_Element<WeekdayContainer> : BLF_Base
{
    static constexpr std::string getName()
    {
        return "BLF_Element_WeekdayContainer";
    }
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
		Element<WeekdayContainer> weekday = Element<WeekdayContainer>(SCH_WEEKDAY, WeekdayContainer(), info.getCreationTime());
		weekday.getValueReference().replaceSelection(getSelection());
		return weekday;
	}

    static void addDefinition(ObjectDefinitions& definitions)
    {
        definitions.add(definitions.getObjectTable().define<BLF_Element<WeekdayContainer>>(getName(),
            blf::arg("info", &BLF_Element<WeekdayContainer>::info, definitions.get<BLF_ElementInfo>()),
            blf::arg("selectionIndices", &BLF_Element<WeekdayContainer>::selectionIndices)
        ));
    }
};

template <>
struct BLF_Element<TimeContainer> : BLF_Base
{
    static constexpr std::string getName()
    {
        return "BLF_Element_TimeContainer";
    }
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
		return Element<TimeContainer>(SCH_TIME, TimeContainer(hours, minutes), info.getCreationTime());
    }

    static void addDefinition(ObjectDefinitions& definitions)
    {
        definitions.add(definitions.getObjectTable().define<BLF_Element<TimeContainer>>(getName(),
            blf::arg("info", &BLF_Element<TimeContainer>::info, definitions.get<BLF_ElementInfo>()),
            blf::arg("hours", &BLF_Element<TimeContainer>::hours),
            blf::arg("minutes", &BLF_Element<TimeContainer>::minutes)
        ));
    }
};

template <>
struct BLF_Element<DateContainer> : BLF_Base
{
    static constexpr std::string getName()
    {
        return "BLF_Element_DateContainer";
    }
    BLF_ElementInfo info;
    bool empty;
    int year;
    int month;
    int mday;

    BLF_Element() {}
    BLF_Element(const Element<DateContainer>* element) : info(element)
    {
        TimeWrapper dateTime = element->getValue().getTime();
        empty = element->getValue().getIsEmpty();
        year = dateTime.getYearUTC();
        month = dateTime.getMonthUTC();
        mday = dateTime.getMonthDayUTC();
    }

	Element<DateContainer> getElement() const
	{
		DateContainer dateContainer = DateContainer(TimeWrapper(DateWrapper(year, month, mday)));
		if (empty)
		{
			dateContainer.clear();
		}
		return Element<DateContainer>(SCH_DATE, dateContainer, info.getCreationTime());
	}

    static void addDefinition(ObjectDefinitions& definitions)
    {
        definitions.add(definitions.getObjectTable().define<BLF_Element<DateContainer>>(getName(),
            blf::arg("info", &BLF_Element<DateContainer>::info, definitions.get<BLF_ElementInfo>()),
            blf::arg("empty", &BLF_Element<DateContainer>::empty),
            blf::arg("year", &BLF_Element<DateContainer>::year),
            blf::arg("month", &BLF_Element<DateContainer>::month),
            blf::arg("mday", &BLF_Element<DateContainer>::mday)
        ));
    }
};

template <typename T>
struct BLF_FilterRule : BLF_Base
{
    static constexpr std::string getName()
    {
        return std::string("BLF_FilterRule_").append(Element<T>::getTypeName());
    }
    BLF_Element<T> passValueElement;
    int comparison = 0;
    bool dateCompareToCurrent = false;

    BLF_FilterRule() {}
    BLF_FilterRule(SCHEDULE_TYPE type, const FilterRule<T>& filterRule)
    {
        Element<T> element = Element<T>(type, filterRule.getPassValue(), TimeWrapper());
        passValueElement = BLF_Element<T>(&element);

        comparison = (int)filterRule.getComparison();
        dateCompareToCurrent = filterRule.getDateCompareCurrent();
    }

    FilterRule<T> getFilterRule() const
    {
        FilterRule<T> filter = FilterRule<T>(passValueElement.getElement().getValue());
        filter.setComparison((Comparison)comparison);
        filter.setDatePassCompareCurrent(dateCompareToCurrent);
        return filter;
    }

    static void addDefinition(ObjectDefinitions& definitions)
    {
        definitions.add(definitions.getObjectTable().define<BLF_FilterRule<T>>(getName(),
            blf::arg("passValueElement", &BLF_FilterRule<T>::passValueElement, definitions.get<BLF_Element<T>>()),
            blf::arg("comparison", &BLF_FilterRule<T>::comparison),
            blf::arg("dateCompareToCurrent", &BLF_FilterRule<T>::dateCompareToCurrent)
        ));
    }
};

template <typename T>
struct BLF_Filter : BLF_Base
{
    static constexpr std::string getName()
    {
        return std::string("BLF_Filter_").append(Element<T>::getTypeName());
    }
    int logicalOperator;
    std::vector<BLF_FilterRule<T>> rules = {};
    BLF_Filter() {}

    BLF_Filter(SCHEDULE_TYPE type, const Filter& filter)
    {
        this->logicalOperator = (int)filter.getOperatorType();

        for (size_t i = 0; i < filter.getRuleCount(); i++)
        {
            rules.emplace_back(type, filter.getRuleConst(i).getAsType<T>());
        }
    }

    Filter getFilter() const
    {
        Filter filter = Filter({}, (LogicalOperatorEnum)logicalOperator);

        for (const BLF_FilterRule<T>& blfRule: rules)
        {
            filter.addRule(blfRule.getFilterRule());
        }

        return filter;
    }

    static void addDefinition(ObjectDefinitions& definitions)
    {
        definitions.add(definitions.getObjectTable().define<BLF_Filter<T>>(getName(),
            blf::arg("logicalOperator", &BLF_Filter<T>::logicalOperator),
            blf::arg("rules", &BLF_Filter<T>::rules, definitions.get<BLF_FilterRule<T>>())
        ));
    }
};

template <typename T>
struct BLF_FilterGroup : BLF_Base
{
    static constexpr std::string getName()
    {
        return std::string("BLF_FilterGroup_").append(Element<T>::getTypeName());
    }
    int logicalOperator;
    std::string name;
    bool enabled;
    std::vector<BLF_Filter<T>> filters = {};
    BLF_FilterGroup() {}

    BLF_FilterGroup(SCHEDULE_TYPE type, const FilterGroup& filterGroup)
    {
        this->logicalOperator = (int)filterGroup.getOperatorType();
        this->name = filterGroup.getName();
        this->enabled = filterGroup.getIsEnabled();

        for (size_t i = 0; i < filterGroup.getFilterCount(); i++)
        {
            filters.emplace_back(type, filterGroup.getFilterConst(i));
        }
    }

    FilterGroup getFilterGroup() const
    {
        FilterGroup filterGroup = FilterGroup({}, name, (LogicalOperatorEnum)logicalOperator, enabled);

        for (const BLF_Filter<T>& blfFilter: filters)
        {
            filterGroup.addFilter(blfFilter.getFilter());
        }

        return filterGroup;
    }

    static void addDefinition(ObjectDefinitions& definitions)
    {
        definitions.add(definitions.getObjectTable().define<BLF_FilterGroup<T>>(getName(),
            blf::arg("logicalOperator", &BLF_FilterGroup<T>::logicalOperator),
            blf::arg("name", &BLF_FilterGroup<T>::name),
            blf::arg("enabled", &BLF_FilterGroup<T>::enabled),
            blf::arg("filters", &BLF_FilterGroup<T>::filters, definitions.get<BLF_Filter<T>>())
        ));
    }
};

struct BLF_SelectOption : BLF_Base
{
	static constexpr std::string getName()
	{
		return "BLF_SelectOption";
	}

    std::string name = "";
    int colorEnum = 0;

    BLF_SelectOption() {}
    BLF_SelectOption(const SelectOption& selectOption)
    {
        name = selectOption.name;
        colorEnum = selectOption.color;
    }

    SelectOption getOption() const
    {
        return SelectOption(name, colorEnum);
    }

    static void addDefinition(ObjectDefinitions& definitions)
	{
		definitions.add(definitions.getObjectTable().define<BLF_SelectOption>(getName(),
			blf::arg("name", &BLF_SelectOption::name),
			blf::arg("colorEnum", &BLF_SelectOption::colorEnum)
		));
	}
};

struct BLF_SelectOptions : BLF_Base
{
	static constexpr std::string getName()
	{
		return "BLF_SelectOptions";
	}

    std::vector<BLF_SelectOption> options = {};
    bool isMutable = true;

    BLF_SelectOptions() {}
    BLF_SelectOptions(const SelectOptions& selectOptions)
    {
        isMutable = selectOptions.getIsMutable();
        for (size_t i = 0; i < selectOptions.getOptions().size(); i++)
        {
            options.push_back(BLF_SelectOption(selectOptions.getOptions()[i]));
        }
    }

    SelectOptions getOptions() const
    {
        std::vector<SelectOption> normalOptions = {};
        for (size_t i = 0; i < options.size(); i++)
        {
            normalOptions.push_back(options[i].getOption());
        }
        SelectOptions selectOptions = SelectOptions(normalOptions);
        selectOptions.setIsMutable(isMutable);
        return selectOptions;
    }

    static void addDefinition(ObjectDefinitions& definitions)
	{
		definitions.add(definitions.getObjectTable().define<BLF_SelectOptions>(getName(),
			blf::arg("options", &BLF_SelectOptions::options, definitions.get<BLF_SelectOption>())
		));
	}
};

template <typename T>
struct BLF_Column : BLF_Base
{
    static constexpr std::string getName()
    {
        return std::string("BLF_Column_").append(Element<T>::getTypeName());
    }
    int index;
    int type;
    std::string name;
    bool permanent;
    int flags;
    int sort;
    int resetOption;
    BLF_SelectOptions selectOptions;
    std::vector<BLF_Element<T>> elements = {};
    std::vector<BLF_FilterGroup<T>> filterGroups = {};
    BLF_Column() {}

    BLF_Column(const Column* column, size_t index)
    {
        this->index = (int)index;
        this->type = (int)column->type;
        this->name = column->name;
        this->permanent = column->permanent;
        this->flags = column->flags;
        this->sort = (int)column->sort;
        this->resetOption = (int)column->resetOption;

        selectOptions = BLF_SelectOptions(column->selectOptions);

        for (ElementBase* elementBase: column->rows)
        {
            elements.push_back(BLF_Element<T>((Element<T>*)elementBase));
        }

        for (auto filterGroup : column->getFilterGroupsConst())
        {
            filterGroups.emplace_back(column->type, filterGroup);
        }
    }

    Column getColumn() const
    {
        // set up SelectOptions
        SelectOptions columnSelectOptions = selectOptions.getOptions();

        Column col = Column(
            {},
            (SCHEDULE_TYPE)type,
            name,
            permanent,
            (ScheduleColumnFlags)flags,
            (COLUMN_SORT)sort,
            columnSelectOptions,
            (ColumnResetOption)resetOption
        );
        // add elements to the column
        for (size_t row = 0; row < elements.size(); row++)
        {
            col.addElement(col.rows.size(), new Element<T>(elements[row].getElement()));
        }

        // add filter groups to the column
        for (const BLF_FilterGroup<T>& filterGroup: filterGroups)
        {
            col.addFilterGroup(col.getFilterGroupCount(), filterGroup.getFilterGroup());
        }

        return col;
    }

    static void addDefinition(ObjectDefinitions& definitions)
    {
        definitions.add(definitions.getObjectTable().define<BLF_Column<T>>(getName(),
            blf::arg("index", &BLF_Column<T>::index),
            blf::arg("type", &BLF_Column<T>::type),
            blf::arg("name", &BLF_Column<T>::name),
            blf::arg("permanent", &BLF_Column<T>::permanent),
            blf::arg("flags", &BLF_Column<T>::flags),
            blf::arg("sort", &BLF_Column<T>::sort),
            blf::arg("resetOption", &BLF_Column<T>::resetOption),
            blf::arg("selectOptions", &BLF_Column<T>::selectOptions, definitions.get<BLF_SelectOptions>()),
            blf::arg("elements", &BLF_Column<T>::elements, definitions.get<BLF_Element<T>>()),
            blf::arg("filterGroups", &BLF_Column<T>::filterGroups, definitions.get<BLF_FilterGroup<T>>())
        ));
    }
};

class ScheduleDataConverter
{
    private:
        std::string m_extension = ".blf";
        ObjectDefinitions m_definitions;
        
        template <DerivedBlfBase BlfClass>
        void addObjectDefinition()
        {
            BlfClass::addDefinition(m_definitions);
        }
        // Adds definitions for all element-type objects for this type.
        template <typename T>
        void addTypeObjectDefinitions()
        {
            addObjectDefinition<BLF_Element<T>>();
            addObjectDefinition<BLF_FilterRule<T>>();
            addObjectDefinition<BLF_Filter<T>>();
            addObjectDefinition<BLF_FilterGroup<T>>();
            addObjectDefinition<BLF_Column<T>>();
        }
        template <typename T>
        const blf::LocalObjectDefinition<T>& getObjectDefinition()
        {
            return m_definitions.get<T>();
        }
    public:
        // Get the file extension used by the ScheduleDataConverter.
        const std::string& getExtension() const;
        void setupObjectTable();

        // Adds the Column (and its elements, filters, etc to the provided DataTable), assuming that the Columns (and their elements, filters) are of the provided type.
        template <typename T>
        void addColumnToData(blf::file::DataTable& data, const Column& column, size_t columnIndex)
        {
            BLF_Column<T> blfColumn = BLF_Column<T>(&column, columnIndex);
            data.insert(getObjectDefinition<BLF_Column<T>>().serialize(blfColumn));
        }
        bool isValidScheduleFile(const char* path) const;
        // Write the Columns of a Schedule to a file at the given path.
        int writeSchedule(const char* path, const std::vector<Column>&);
        // Read a Schedule from path and return the Columns containing the correct Elements. NOTE: The function creates a copy of the provided vector, but modifies the argument directly. If the function fails at any point, it will be reset to the copy created at the start.
        // Returns a partial FileInfo if successful.
        std::optional<FileInfo> readSchedule(const char* path, std::vector<Column>&);
};