#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do
                           // this in one cpp file
#include <catch2/catch_test_macros.hpp>
#include <catch2/reporters/catch_reporter_event_listener.hpp>
#include <catch2/reporters/catch_reporter_registrars.hpp>
#include "main.h"

#include "test_time_wrapper.h" // tested early because almost everything uses it
#include "test_date_container.h" // must be tested before filters since filters use it
#include "test_select_options.h"
#include "test_filter.h" // tested before FilterGroups since they are contained
#include "test_filter_group.h"
#include "test_time_handler.h"

class testListener : public Catch::EventListenerBase 
{
    public:
        using Catch::EventListenerBase::EventListenerBase;

        void testRunStarting(Catch::TestRunInfo const&) override 
        {
            std::cout << "TESTS BEGIN" << std::endl;
            std::setlocale(LC_ALL, "");
        }

        void testRunEnded(Catch::TestRunStats const& test) override
        {
            std::cout << "TESTS END" << std::endl;
            Program program = Program();
            program.loop();
        }
};

CATCH_REGISTER_LISTENER(testListener)

TEST_CASE("Input system", "[input]") 
{
    TextureLoader textureLoader;
    Window windowManager; // dummy window manager, not initalised, should do nothing with glfw.
    Input input;
    input.init(&windowManager); // init because input sets up its event map in init()
    std::function<void()> invalidListener = std::function<void()>(nullptr);
    input.addEventListener(INPUT_EVENT_SC_RENAME, invalidListener);
    CHECK(input.getEventListenerCount(INPUT_EVENT_SC_RENAME) == 0);
    std::function<void()> listener = std::function<void()>([]{});
    input.addEventListener(INPUT_EVENT_SC_RENAME, listener);
    CHECK(input.getEventListenerCount(INPUT_EVENT_SC_RENAME) == 1);
    input.invokeEvent(INPUT_EVENT_SC_RENAME);
}