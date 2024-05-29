#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do
                           // this in one cpp file
#include <catch2/catch_test_macros.hpp>
#include <catch2/reporters/catch_reporter_event_listener.hpp>
#include <catch2/reporters/catch_reporter_registrars.hpp>
#include "main.h"

Program program = Program();

class testProgramSetupListener : public Catch::EventListenerBase {
public:
    using Catch::EventListenerBase::EventListenerBase;

    void testRunStarting(Catch::TestRunInfo const&) override {
        //program = Program();
        std::cout << program.windowManager.getTitle() << std::endl;
    }
};

CATCH_REGISTER_LISTENER(testProgramSetupListener)

TEST_CASE( "Input system", "[input]" ) {
    program.input.addCallbackListener(INPUT_CALLBACK_SC_RENAME, std::function<void()>(nullptr));
    REQUIRE(program.input.getCallbackListenerCount(INPUT_CALLBACK_SC_RENAME) == 0);
    std::function<void ()> listener = std::function<void()>([]{});
    program.input.addCallbackListener(INPUT_CALLBACK_SC_RENAME, listener);
    REQUIRE(program.input.getCallbackListenerCount(INPUT_CALLBACK_SC_RENAME) == 1);
    program.input.invokeCallback(INPUT_CALLBACK_SC_RENAME);
}