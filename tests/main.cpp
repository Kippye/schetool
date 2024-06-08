#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do
                           // this in one cpp file
#include <catch2/catch_test_macros.hpp>
#include <catch2/reporters/catch_reporter_event_listener.hpp>
#include <catch2/reporters/catch_reporter_registrars.hpp>
#include "main.h"

#include "test_date_container.h" // must be tested before filters since filters use it
#include "test_filter.h"
#include "test_schedule_gui.h"

Program program = Program();

class testProgramSetupListener : public Catch::EventListenerBase 
{
    public:
        using Catch::EventListenerBase::EventListenerBase;

        void testRunStarting(Catch::TestRunInfo const&) override 
        {
            //program = Program();
            std::cout << program.windowManager.getTitle() << std::endl;
        }
};

CATCH_REGISTER_LISTENER(testProgramSetupListener)

TEST_CASE("Input system", "[input]") 
{
    std::function<void ()> invalidListener = std::function<void()>(nullptr);
    program.input.addEventListener(INPUT_EVENT_SC_RENAME, invalidListener);
    CHECK(program.input.getEventListenerCount(INPUT_EVENT_SC_RENAME) == 0);
    std::function<void ()> listener = std::function<void()>([]{});
    program.input.addEventListener(INPUT_EVENT_SC_RENAME, listener);
    CHECK(program.input.getEventListenerCount(INPUT_EVENT_SC_RENAME) == 1);
    program.input.invokeEvent(INPUT_EVENT_SC_RENAME);
}