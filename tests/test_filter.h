#include <catch2/catch_test_macros.hpp>
#include <catch2/reporters/catch_reporter_event_listener.hpp>
#include <catch2/reporters/catch_reporter_registrars.hpp>

#include "filter.h"
#include "element.h"

TEST_CASE("Filter")
{
    SECTION("Filtering different Element types")
    {
        DateContainer creationDate = DateContainer();
        TimeContainer creationTime = TimeContainer();

        SECTION("Bool filter")
        {
            Element<bool> element = Element<bool>(SCH_BOOL, true, creationDate, creationTime);
            Filter filter = Filter(true);
            CHECK(filter.checkPasses(&element) == true);
            
            element.setValue(false);
            CHECK(filter.checkPasses(&element) == false);
        }
        SECTION("Number filter")
        {
            Element<int> element = Element<int>(SCH_NUMBER, 4206969, creationDate, creationTime);
            Filter filter = Filter(4206969);
            CHECK(filter.checkPasses(&element) == true);

            element.setValue(1234567);
            CHECK(filter.checkPasses(&element) == false);
        }
        SECTION("Decimal filter")
        {
            Element<double> element = Element<double>(SCH_DECIMAL, 152.6565, creationDate, creationTime);
            Filter filter = Filter(152.6565);
            CHECK(filter.checkPasses(&element) == true);

            element.setValue(123.54);
            CHECK(filter.checkPasses(&element) == false);
        }
        SECTION("Text filter")
        {
            Element<std::string> element = Element<std::string>(SCH_TEXT, "schetool is Cool!", creationDate, creationTime);
            Filter filter = Filter(std::string("schetool is Cool!"));
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
        //     Filter filter = Filter(4206969);
        //     CHECK(filter.checkPasses(&element) == true);
        //     element.setValue(1234567);
        //     CHECK(filter.checkPasses(&element) == false);
        // }
        // SECTION("Time filter")
        // {
        //     Element<int> element = Element<int>(SCH_TIME, 4206969, creationDate, creationTime);
        //     Filter filter = Filter(4206969);
        //     CHECK(filter.checkPasses(&element) == true);
        //     element.setValue(1234567);
        //     CHECK(filter.checkPasses(&element) == false);
        // }
        SECTION("Date filter")
        {
            tm time;
            time.tm_mday = 25;
            time.tm_mon = 4;
            time.tm_year = 132;
            Element<DateContainer> element = Element<DateContainer>(SCH_DATE, DateContainer(time), creationDate, creationTime);
            Filter filter = Filter(DateContainer(time));
            CHECK(filter.checkPasses(&element) == true);

            time.tm_mday = 13;
            time.tm_mon = 1;
            element.setValue(time);
            CHECK(filter.checkPasses(&element) == false);
        }
    }
}