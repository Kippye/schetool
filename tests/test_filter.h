#include <catch2/catch_test_macros.hpp>
#include <catch2/reporters/catch_reporter_event_listener.hpp>
#include <catch2/reporters/catch_reporter_registrars.hpp>

#include "filter.h"
#include "element.h"
#include "date_container.h"

TEST_CASE("FilterRule")
{
    SECTION("Filtering different Element types")
    {
        DateContainer creationDate = DateContainer();
        TimeContainer creationTime = TimeContainer();

        SECTION("Bool filter")
        {
            Element<bool> element = Element<bool>(SCH_BOOL, true, creationDate, creationTime);
            FilterRule filter = FilterRule(true);
            CHECK(filter.checkPasses(&element) == true);
            
            element.setValue(false);
            CHECK(filter.checkPasses(&element) == false);
        }
        SECTION("Number filter")
        {
            Element<int> element = Element<int>(SCH_NUMBER, 4206969, creationDate, creationTime);
            FilterRule filter = FilterRule(4206969);
            CHECK(filter.checkPasses(&element) == true);

            element.setValue(1234567);
            CHECK(filter.checkPasses(&element) == false);
        }
        SECTION("Decimal filter")
        {
            Element<double> element = Element<double>(SCH_DECIMAL, 152.6565, creationDate, creationTime);
            FilterRule filter = FilterRule(152.6565);
            CHECK(filter.checkPasses(&element) == true);

            element.setValue(123.54);
            CHECK(filter.checkPasses(&element) == false);
        }
        SECTION("Text filter")
        {
            Element<std::string> element = Element<std::string>(SCH_TEXT, "schetool is Cool!", creationDate, creationTime);
            FilterRule filter = FilterRule(std::string("schetool is Cool!"));
            CHECK(filter.checkPasses(&element) == true);

            element.setValue("schetool is not Cool if this passes! >:(");
            CHECK(filter.checkPasses(&element) == false);
        }
        // SECTION("Select filter")
        // {
        //     Element<SelectContainer> element = Element<SelectContainer>(SCH_SELECT, SelectContainer(), creationDate, creationTime);
        //     SelectOptionChange selectOptionChange = SelectOptionChange();
        //     selectOptionChange.replace(OPTION_MODIFICATION_ADD, 0, 0);
        //     element.getValueReference().update(SelectOptionChange(), size_t optionCount)
        //     FilterRule filter = FilterRule(4206969);
        //     CHECK(filter.checkPasses(&element) == true);
        //     element.setValue(1234567);
        //     CHECK(filter.checkPasses(&element) == false);
        // }
        // SECTION("Time filter")
        // {
        //     Element<int> element = Element<int>(SCH_TIME, 4206969, creationDate, creationTime);
        //     FilterRule filter = FilterRule(4206969);
        //     CHECK(filter.checkPasses(&element) == true);
        //     element.setValue(1234567);
        //     CHECK(filter.checkPasses(&element) == false);
        // }
        SECTION("Date filter")
        {
            TimeWrapper time = TimeWrapper(132, 4, 25);
            Element<DateContainer> element = Element<DateContainer>(SCH_DATE, DateContainer(time), creationDate, creationTime);
            FilterRule filter = FilterRule(DateContainer(time));
            CHECK(filter.checkPasses(&element) == true);

            time.setMonthDay(13);
            time.setMonth(1);
            element.setValue(time);
            CHECK(filter.checkPasses(&element) == false);
        }
    }
}

TEST_CASE("Filter")
{
    Filter filter = Filter();

    SECTION("Filter rules modification")
    {
        CHECK(filter.getRule(13).isEmpty()); // index out of range
        FilterRule<int> filterRule = FilterRule<int>(52);
        filter.addRule(filterRule);
        CHECK(filter.getRuleCount() == 1); // increased size
        FilterRule<int> secondRule = FilterRule<int>(100);
        filter.addRule(secondRule);
        CHECK(filter.getRuleCount() == 2); // increased size
        FilterRule<int> thirdRule = FilterRule<int>(19);
        filter.replaceRule(1, thirdRule);
        // value replaced
        CHECK(filter.getRule(1).getAsType<int>().getPassValue() == thirdRule.getPassValue());
        filter.removeRule(0);
        CHECK(filter.getRuleCount() == 1); // size reduced
        filter.removeRule(100); // remove at invalid index should do nothing
        CHECK(filter.getRuleCount() == 1); // size stays the same
    }
    SECTION("Filter operators")
    {
        Element<std::string> element = Element<std::string>(SCH_TEXT, "PASS", DateContainer(), TimeContainer());
        FilterRule<std::string> failRule = FilterRule<std::string>("FAILURE");
        FilterRule<std::string> passRule = FilterRule<std::string>("PASS");

        SECTION("One fail FilterRule")
        {
            filter.addRule(failRule);
            SECTION("'And' operator")
            {
                filter.setOperator(LogicalOperatorEnum::And);
                CHECK(filter.checkPasses(&element) == false);
            }
            SECTION("'Or' operator")
            {
                filter.setOperator(LogicalOperatorEnum::Or);
                CHECK(filter.checkPasses(&element) == false);
            }
        }
        SECTION("One pass FilterRule")
        {
            filter.addRule(passRule);
            SECTION("'And' operator")
            {
                filter.setOperator(LogicalOperatorEnum::And);
                CHECK(filter.checkPasses(&element) == true);
            }
            SECTION("'Or' operator")
            {
                filter.setOperator(LogicalOperatorEnum::Or);
                CHECK(filter.checkPasses(&element) == true);
            }
        }
        SECTION("One fail, one pass FilterRule")
        {
            filter.addRule(failRule);
            filter.addRule(passRule);
            SECTION("'And' operator")
            {
                filter.setOperator(LogicalOperatorEnum::And);
                CHECK(filter.checkPasses(&element) == false);
            }
            SECTION("'Or' operator")
            {
                filter.setOperator(LogicalOperatorEnum::Or);
                CHECK(filter.checkPasses(&element) == true);
            }
        }
        SECTION("Two fail FilterRules")
        {
            filter.addRule(failRule);
            filter.addRule(failRule);
            SECTION("'And' operator")
            {
                filter.setOperator(LogicalOperatorEnum::And);
                CHECK(filter.checkPasses(&element) == false);
            }
            SECTION("'Or' operator")
            {
                filter.setOperator(LogicalOperatorEnum::Or);
                CHECK(filter.checkPasses(&element) == false);
            }
        }
        SECTION("Two pass FilterRules")
        {
            filter.addRule(passRule);
            filter.addRule(passRule);
            SECTION("'And' operator")
            {
                filter.setOperator(LogicalOperatorEnum::And);
                CHECK(filter.checkPasses(&element) == true);
            }
            SECTION("'Or' operator")
            {
                filter.setOperator(LogicalOperatorEnum::Or);
                CHECK(filter.checkPasses(&element) == true);
            }
        }
        SECTION("Three FilterRules")
        {
            filter.addRule(passRule);
            filter.addRule(failRule);
            filter.addRule(passRule);
            SECTION("'And' operator")
            {
                filter.setOperator(LogicalOperatorEnum::And);
                CHECK(filter.checkPasses(&element) == false);
            }
            SECTION("'Or' operator")
            {
                filter.setOperator(LogicalOperatorEnum::Or);
                CHECK(filter.checkPasses(&element) == true);
            }
        }
    }
}