#include <catch2/catch_test_macros.hpp>
#include <catch2/reporters/catch_reporter_event_listener.hpp>
#include <catch2/reporters/catch_reporter_registrars.hpp>

#include "filters/filter.h"
#include "element.h"
#include "date_container.h"
#include <memory>

TEST_CASE("FilterRule") {
    SECTION("Filtering different Element types") {
        SECTION("Bool filter") {
            std::shared_ptr<Element<bool>> element = std::make_shared<Element<bool>>(SCH_BOOL, true);
            FilterRule filter = FilterRule(true);
            CHECK(filter.checkPasses(element) == true);

            element->setValue(false);
            CHECK(filter.checkPasses(element) == false);
        }
        SECTION("Number filter") {
            std::shared_ptr<Element<int>> element = std::make_shared<Element<int>>(SCH_NUMBER, 4206969);
            FilterRule filter = FilterRule(4206969);
            CHECK(filter.checkPasses(element) == true);

            element->setValue(1234567);
            CHECK(filter.checkPasses(element) == false);
        }
        SECTION("Decimal filter") {
            std::shared_ptr<Element<double>> element = std::make_shared<Element<double>>(SCH_DECIMAL, 152.6565);
            FilterRule filter = FilterRule(152.6565);
            CHECK(filter.checkPasses(element) == true);

            element->setValue(123.54);
            CHECK(filter.checkPasses(element) == false);
        }
        SECTION("Text filter") {
            std::shared_ptr<Element<std::string>> element =
                std::make_shared<Element<std::string>>(SCH_TEXT, "schetool is Cool!");
            FilterRule filter = FilterRule(std::string("schetool is Cool!"));
            CHECK(filter.checkPasses(element) == true);

            element->setValue("schetool is not Cool if this passes! >:(");
            CHECK(filter.checkPasses(element) == false);
        }
        // SECTION("Select filter")
        // {
        //     Element<SelectContainer> element = Element<SelectContainer>(SCH_SELECT, SelectContainer());
        //     SelectOptionUpdateInfo SelectOptionUpdateInfo = SelectOptionUpdateInfo();
        //     SelectOptionUpdateInfo.replace(OPTION_MODIFICATION_ADD, 0, 0);
        //     element.getValueReference().update(SelectOptionUpdateInfo(), size_t optionCount)
        //     FilterRule filter = FilterRule(4206969);
        //     CHECK(filter.checkPasses(&element) == true);
        //     element->setValue(1234567);
        //     CHECK(filter.checkPasses(&element) == false);
        // }
        // SECTION("Time filter")
        // {
        //     Element<int> element = Element<int>(SCH_TIME, 4206969);
        //     FilterRule filter = FilterRule(4206969);
        //     CHECK(filter.checkPasses(&element) == true);
        //     element->setValue(1234567);
        //     CHECK(filter.checkPasses(&element) == false);
        // }
        SECTION("Date filter") {
            TimeWrapper time = TimeWrapper({2032, 4, 25});
            std::shared_ptr<Element<DateContainer>> element =
                std::make_shared<Element<DateContainer>>(SCH_DATE, DateContainer(time));
            FilterRule filter = FilterRule(DateContainer(time));
            CHECK(filter.checkPasses(element) == true);

            time.setMonthDayUTC(13);
            time.setMonthUTC(1);
            element->setValue(time);
            CHECK(filter.checkPasses(element) == false);
        }
    }
}

TEST_CASE("Filter") {
    Filter filter = Filter();

    SECTION("Filter rules modification") {
        CHECK_THROWS_AS(filter.getRule(13), std::out_of_range);  // index out of range
        FilterRule<int> filterRule = FilterRule<int>(52);
        filter.addRule(filterRule);
        CHECK(filter.getRuleCount() == 1);  // increased size
        FilterRule<int> secondRule = FilterRule<int>(100);
        filter.addRule(secondRule);
        CHECK(filter.getRuleCount() == 2);  // increased size
        FilterRule<int> thirdRule = FilterRule<int>(19);
        filter.replaceRule(1, thirdRule);
        // value replaced
        CHECK(filter.getRule(1).getAsType<int>().getPassValue() == thirdRule.getPassValue());
        filter.removeRule(0);
        CHECK(filter.getRuleCount() == 1);  // size reduced
        filter.removeRule(100);  // remove at invalid index should do nothing
        CHECK(filter.getRuleCount() == 1);  // size stays the same
    }
    SECTION("Filter operators") {
        std::shared_ptr<Element<std::string>> element = std::make_shared<Element<std::string>>(SCH_TEXT, "PASS");
        FilterRule<std::string> failRule = FilterRule<std::string>("FAILURE");
        FilterRule<std::string> passRule = FilterRule<std::string>("PASS");

        SECTION("One fail FilterRule") {
            filter.addRule(failRule);
            SECTION("'And' operator") {
                filter.setOperator(LogicalOperatorEnum::And);
                CHECK(filter.checkPasses(element) == false);
            }
            SECTION("'Or' operator") {
                filter.setOperator(LogicalOperatorEnum::Or);
                CHECK(filter.checkPasses(element) == false);
            }
        }
        SECTION("One pass FilterRule") {
            filter.addRule(passRule);
            SECTION("'And' operator") {
                filter.setOperator(LogicalOperatorEnum::And);
                CHECK(filter.checkPasses(element) == true);
            }
            SECTION("'Or' operator") {
                filter.setOperator(LogicalOperatorEnum::Or);
                CHECK(filter.checkPasses(element) == true);
            }
        }
        SECTION("One fail, one pass FilterRule") {
            filter.addRule(failRule);
            filter.addRule(passRule);
            SECTION("'And' operator") {
                filter.setOperator(LogicalOperatorEnum::And);
                CHECK(filter.checkPasses(element) == false);
            }
            SECTION("'Or' operator") {
                filter.setOperator(LogicalOperatorEnum::Or);
                CHECK(filter.checkPasses(element) == true);
            }
        }
        SECTION("Two fail FilterRules") {
            filter.addRule(failRule);
            filter.addRule(failRule);
            SECTION("'And' operator") {
                filter.setOperator(LogicalOperatorEnum::And);
                CHECK(filter.checkPasses(element) == false);
            }
            SECTION("'Or' operator") {
                filter.setOperator(LogicalOperatorEnum::Or);
                CHECK(filter.checkPasses(element) == false);
            }
        }
        SECTION("Two pass FilterRules") {
            filter.addRule(passRule);
            filter.addRule(passRule);
            SECTION("'And' operator") {
                filter.setOperator(LogicalOperatorEnum::And);
                CHECK(filter.checkPasses(element) == true);
            }
            SECTION("'Or' operator") {
                filter.setOperator(LogicalOperatorEnum::Or);
                CHECK(filter.checkPasses(element) == true);
            }
        }
        SECTION("Three FilterRules") {
            filter.addRule(passRule);
            filter.addRule(failRule);
            filter.addRule(passRule);
            SECTION("'And' operator") {
                filter.setOperator(LogicalOperatorEnum::And);
                CHECK(filter.checkPasses(element) == false);
            }
            SECTION("'Or' operator") {
                filter.setOperator(LogicalOperatorEnum::Or);
                CHECK(filter.checkPasses(element) == true);
            }
        }
    }
}