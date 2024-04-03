#pragma once
#include <string>
#include <ctime>
#include <set>
#include <vector>

enum SCHEDULE_TYPE
{
    SCH_BOOL,
    SCH_NUMBER,
    SCH_DECIMAL,
    SCH_TEXT,
    SCH_SELECT,
    SCH_TIME,
    SCH_DATE,
    SCH_WEEKDAY,
    SCH_LAST,
};

struct DateContainer
{
    tm time;

    DateContainer();
    DateContainer(const tm& t);
    std::string getString() const;
    const tm* getTime() const;
    void setTime(const tm& time);
    void setMonthDay(unsigned int day);
    void setMonth(int month);
    void setYear(int year, bool convert);

    static int convertToValidYear(int year, bool hasBeenSubtracted = false, bool subtractTmBaseYear = true);

    friend bool operator<(const DateContainer& left, const DateContainer& right)
    {
        if (left.time.tm_year < right.time.tm_year) { return true; }
        else if (left.time.tm_year == right.time.tm_year)
        {
            if (left.time.tm_mon < right.time.tm_mon) { return true; }
            else if (left.time.tm_mon == right.time.tm_mon)
            {
                return left.time.tm_mday < right.time.tm_mday;
            }
            else return false;
        }
        else return false;
    }

    friend bool operator>(const DateContainer& left, const DateContainer& right)
    {
        if (left.time.tm_year > right.time.tm_year) { return true; }
        else if (left.time.tm_year == right.time.tm_year)
        {
            if (left.time.tm_mon > right.time.tm_mon) { return true; }
            else if (left.time.tm_mon == right.time.tm_mon)
            {
                return left.time.tm_mday > right.time.tm_mday;
            }
            else return false;
        }
        else return false;
    }
};

struct TimeContainer
{
    int hours;
    int minutes;

    TimeContainer();
    TimeContainer(int h, int m);
    TimeContainer(const tm& t);
    std::string getString() const;
    int getHours() const;
    int getMinutes() const;
    void setTime(int hours, int minutes);

    TimeContainer& operator+=(const TimeContainer& rhs) // compound assignment (does not need to be a member,
    {                           // but often is, to modify the private members)
        this->minutes += rhs.minutes;
        this->hours += this->minutes / 60;
        this->minutes -= this->minutes / 60 * 60;
        this->hours += rhs.hours;
        this->hours = this->hours > 23 ? this->hours - this->hours / 24 * 24 : this->hours;
        return *this; // return the result by reference
    }

    TimeContainer& operator-=(const TimeContainer& rhs) // compound assignment (does not need to be a member,
    {                           // but often is, to modify the private members)
        this->minutes -= rhs.minutes;
        this->hours = this->minutes < 0 ? this->hours - 1 : this->hours;
        this->minutes = this->minutes < 0 ? 60 + this->minutes : this->minutes;
        this->hours -= rhs.hours;
        this->hours = this->hours < 0 ? 24 + this->hours : this->hours;
        return *this; // return the result by reference
    }
    
    // friends defined inside class body are inline and are hidden from non-ADL lookup
    friend TimeContainer operator+(TimeContainer lhs, const TimeContainer& rhs)
    {
        lhs += rhs; // reuse compound assignment
        return lhs; // return the result by value (uses move constructor)
    }

    friend TimeContainer operator-(TimeContainer lhs, const TimeContainer& rhs)
    {
        lhs -= rhs; // reuse compound assignment
        return lhs; // return the result by value (uses move constructor)
    }

    friend bool operator<(const TimeContainer& left, const TimeContainer& right)
    {
        if (left.hours < right.hours) { return true; }
        else if (left.hours == right.hours) 
        {  
            return left.minutes < right.minutes;
        }
        else return false;
    }

    friend bool operator>(const TimeContainer& left, const TimeContainer& right)
    {
        if (left.hours > right.hours) { return true; }
        else if (left.hours == right.hours) 
        {  
            return left.minutes > right.minutes;
        }
        else return false;
    }
};

enum SELECT_MODIFICATION
{
    SELECT_MODIFICATION_REMOVE,
    SELECT_MODIFICATION_MOVE,
    SELECT_MODIFICATION_CLEAR
};

struct SelectOptionChange
{
    SELECT_MODIFICATION type;
    size_t firstIndex;
    size_t secondIndex;
    bool applied;

    public:
        void replace(SELECT_MODIFICATION type, size_t firstIndex, size_t secondIndex);
};

struct SelectOptions
{
    private:
        std::vector<std::string> m_options = {};
        SelectOptionChange m_lastModification;
        bool m_mutable;
    public:
        SelectOptions();
        SelectOptions(const std::vector<std::string>& options);

        const std::vector<std::string>& getOptions() const;
        const SelectOptionChange& getLastChange() const;
        void addOption(const std::string& option);
        void removeOption(const std::string& option);
        void removeOption(size_t option);
        void moveOption(size_t firstIndex, size_t secondIndex);
        void clearOptions();
        void setIsMutable(bool to);
        bool getIsMutable() const;
        // void renameOption(size_t option, const char* name);
        void modificationApplied();
};

struct SelectContainer
{
    private:
        std::set<size_t> m_selection;
        SelectOptions* m_options;
    public:
        SelectContainer();
        SelectContainer(SelectOptions& options) : m_options(&options){}

        friend bool operator<(const SelectContainer& left, const SelectContainer& right)
        {
            if (right.m_selection.size() == 0) { return true; }
            if (left.m_selection.size() == 0) { return false; }

            size_t leftForemostOption = 1000;
            size_t rightForemostOption = 1000;

            for (size_t i = 0; i < left.m_options->getOptions().size(); i++)
            {
                // has this option selected
                if (leftForemostOption == 1000 && left.m_selection.find(i) != left.m_selection.end())
                {
                    leftForemostOption = i;
                }
                if (rightForemostOption == 1000 && right.m_selection.find(i) != right.m_selection.end())
                {
                    rightForemostOption = i;
                }
            }

            return left.m_options->getOptions()[leftForemostOption] < right.m_options->getOptions()[rightForemostOption];
        }

        friend bool operator>(const SelectContainer& left, const SelectContainer& right)
        {
            if (right.m_selection.size() == 0) { return false; }
            if (left.m_selection.size() == 0) { return true; }

            size_t leftForemostOption = 1000;
            size_t rightForemostOption = 1000;

            for (size_t i = 0; i < left.m_options->getOptions().size(); i++)
            {
                // has this option selected
                if (leftForemostOption == 1000 && left.m_selection.find(i) != left.m_selection.end())
                {
                    leftForemostOption = i;
                }
                if (rightForemostOption == 1000 && right.m_selection.find(i) != right.m_selection.end())
                {
                    rightForemostOption = i;
                }
            }

            return left.m_options->getOptions()[leftForemostOption] > right.m_options->getOptions()[rightForemostOption];
        }

        const std::set<size_t>& getSelection() const;
        void replaceSelection(const std::set<size_t>& selection);
        void setSelected(size_t index, bool selected);
        void update();
};

class ElementBase
{
    protected:
        SCHEDULE_TYPE m_type;
        DateContainer m_creationDate;
        TimeContainer m_creationTime;
        DateContainer m_editDate;
        TimeContainer m_editTime;
    public:
        ElementBase();
        ElementBase(SCHEDULE_TYPE type, const DateContainer& creationDate, const TimeContainer& creationTime);
        SCHEDULE_TYPE getType() const;
        const DateContainer& getCreationDate() const;
        const TimeContainer& getCreationTime() const;
        const DateContainer& getEditDate() const;
        const TimeContainer& getEditTime() const;

        virtual std::string getString() const;
};