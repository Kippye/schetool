#include <catch2/catch_test_macros.hpp>
#include <catch2/reporters/catch_reporter_event_listener.hpp>
#include <catch2/reporters/catch_reporter_registrars.hpp>

#include "filter_rule.h"
#include "element_base.h"
#include "filter_editor_subgui.h"

// TEST_CASE("FilterGroupEditorState")
// {
//     FilterGroupEditorState editorFilterState;

//     CHECK(editorFilterState.hasValidFilter() == false);
//     editorFilterState.setType(SCH_NUMBER);
//     CHECK(editorFilterState.getType() == SCH_NUMBER);
//     editorFilterState.setFilter(std::make_shared<FilterRule<int>>(420));
//     CHECK(editorFilterState.hasValidFilter() == true);
//     CHECK(editorFilterState.getFilter<int>()->getPassValue() == 420);
// }

// TEST_CASE("TypeComparisonOptions")
// {
//     TypeComparisonOptions typeCompOptions;

//     SECTION("Type in map failure checks")
//     {
//         CHECK(strcmp(typeCompOptions.getOptions(SCH_LAST).string, "") == 0);
//         CHECK(typeCompOptions.getOptions(SCH_LAST).count == 0);
//         CHECK(typeCompOptions.getOptionSelection(SCH_LAST) == 0);
//     }
//     SECTION("setOptionSelection: setting valid selection")
//     {
//         typeCompOptions.setOptionSelection(SCH_DATE, 1);
//         CHECK(typeCompOptions.getOptionSelection(SCH_DATE) == 1);
//     }
//     SECTION("setOptionSelection: selection index too high")
//     {
//         typeCompOptions.setOptionSelection(SCH_NUMBER, 52);
//         CHECK(typeCompOptions.getOptionSelection(SCH_NUMBER) < 52);
//     }
// }