#include <catch2/catch_test_macros.hpp>
#include <catch2/reporters/catch_reporter_event_listener.hpp>
#include <catch2/reporters/catch_reporter_registrars.hpp>

#include "filter_group.h"

TEST_CASE("FilterGroup") {
    FilterGroup filterGroup = FilterGroup();

    SECTION("FilterGroup filters modification") {
        CHECK_THROWS_AS(filterGroup.getFilter(13), std::out_of_range);  // index out of range
        Filter filter = Filter();
        filterGroup.addFilter(filter);
        CHECK(filterGroup.getFilterCount() == 1);  // increased size
        filterGroup.addFilter(filter);
        CHECK(filterGroup.getFilterCount() == 2);  // increased size
        Filter filterWithRule = Filter();
        // filterWithRule.addRule(FilterRule<int>(52));
        // filter.replaceRule(1, thirdRule);
        // value replaced
        // CHECK(filterGroup.getFilter(1).getRule(0).getAsType<int>().getPassValue() == filterWithRule.getRule(0).getAsType<int>().getPassValue());
        filterGroup.removeFilter(0);
        CHECK(filterGroup.getFilterCount() == 1);  // size reduced
        filterGroup.removeFilter(100);  // remove at invalid index should do nothing
        CHECK(filterGroup.getFilterCount() == 1);  // size stays the same
    }
    SECTION("FilterGroup operators") {
        Element<std::string> element = Element<std::string>(SCH_TEXT, "PASS");
        FilterRule<std::string> failRule = FilterRule<std::string>("FAILURE");
        FilterRule<std::string> passRule = FilterRule<std::string>("PASS");
        FilterRule<std::string> passIsNotRule = FilterRule<std::string>("");
        passIsNotRule.setComparison(Comparison::IsNot);

        // false OR true = true
        Filter passFilter = Filter({FilterRuleContainer(failRule), FilterRuleContainer(passRule)}, LogicalOperatorEnum::Or);
        // true AND false = false
        Filter failFilter =
            Filter({FilterRuleContainer(passIsNotRule), FilterRuleContainer(failRule)}, LogicalOperatorEnum::And);

        SECTION("One fail Filter") {
            filterGroup.addFilter(failFilter);
            SECTION("'And' operator") {
                filterGroup.setOperator(LogicalOperatorEnum::And);
                CHECK(filterGroup.checkPasses(&element) == false);
            }
            SECTION("'Or' operator") {
                filterGroup.setOperator(LogicalOperatorEnum::Or);
                CHECK(filterGroup.checkPasses(&element) == false);
            }
        }
        SECTION("One pass Filter") {
            filterGroup.addFilter(passFilter);
            SECTION("'And' operator") {
                filterGroup.setOperator(LogicalOperatorEnum::And);
                CHECK(filterGroup.checkPasses(&element) == true);
            }
            SECTION("'Or' operator") {
                filterGroup.setOperator(LogicalOperatorEnum::Or);
                CHECK(filterGroup.checkPasses(&element) == true);
            }
        }
        SECTION("One fail, one pass Filter") {
            filterGroup.addFilter(failFilter);
            filterGroup.addFilter(passFilter);
            SECTION("'And' operator") {
                filterGroup.setOperator(LogicalOperatorEnum::And);
                CHECK(filterGroup.checkPasses(&element) == false);
            }
            SECTION("'Or' operator") {
                filterGroup.setOperator(LogicalOperatorEnum::Or);
                CHECK(filterGroup.checkPasses(&element) == true);
            }
        }
        SECTION("Two fail Filters") {
            filterGroup.addFilter(failFilter);
            filterGroup.addFilter(failFilter);
            SECTION("'And' operator") {
                filterGroup.setOperator(LogicalOperatorEnum::And);
                CHECK(filterGroup.checkPasses(&element) == false);
            }
            SECTION("'Or' operator") {
                filterGroup.setOperator(LogicalOperatorEnum::Or);
                CHECK(filterGroup.checkPasses(&element) == false);
            }
        }
        SECTION("Two pass Filters") {
            filterGroup.addFilter(passFilter);
            filterGroup.addFilter(passFilter);
            SECTION("'And' operator") {
                filterGroup.setOperator(LogicalOperatorEnum::And);
                CHECK(filterGroup.checkPasses(&element) == true);
            }
            SECTION("'Or' operator") {
                filterGroup.setOperator(LogicalOperatorEnum::Or);
                CHECK(filterGroup.checkPasses(&element) == true);
            }
        }
        SECTION("Three Filters") {
            filterGroup.addFilter(passFilter);
            filterGroup.addFilter(failFilter);
            filterGroup.addFilter(passFilter);
            SECTION("'And' operator") {
                filterGroup.setOperator(LogicalOperatorEnum::And);
                CHECK(filterGroup.checkPasses(&element) == false);
            }
            SECTION("'Or' operator") {
                filterGroup.setOperator(LogicalOperatorEnum::Or);
                CHECK(filterGroup.checkPasses(&element) == true);
            }
        }
    }
}