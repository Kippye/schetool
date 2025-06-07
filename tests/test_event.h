#include <catch2/catch_test_macros.hpp>
#include <catch2/reporters/catch_reporter_event_listener.hpp>
#include <catch2/reporters/catch_reporter_registrars.hpp>

#include "event.h"
#include <functional>

TEST_CASE("Event") {
    SECTION("Add listeners") {
        std::function<void(int)> listener1 = [&](int arg) {
        };
        std::function<void(int)> listener2 = [&](int arg) {
        };
        Event<int> event;
        CHECK(event.getListenerCount() == 0);
        event.addListener(listener1);
        CHECK(event.getListenerCount() == 1);
        event.addListener(listener2);
        CHECK(event.getListenerCount() == 2);
    }

    SECTION("Remove listeners") {
        std::function<void(int)> listener1 = [&](int arg) {
        };
        std::function<void(int)> listener2 = [&](int arg) {
        };
        Event<int> event;
        CHECK(event.getListenerCount() == 0);
        size_t firstListenerID = event.addListener(listener1);
        CHECK(event.getListenerCount() == 1);
        size_t secondListenerID = event.addListener(listener2);
        CHECK(event.getListenerCount() == 2);
        event.removeListener(firstListenerID);
        CHECK(event.getListenerCount() == 1);
        event.removeListener(firstListenerID);
        CHECK(event.getListenerCount() == 1);
        event.removeListener(secondListenerID);
        CHECK(event.getListenerCount() == 0);
    }

    SECTION("Invoke event") {
        int invokedCount = 0;
        std::function<void(int)> listener = [&](int arg) {
            CHECK(arg == invokedCount + 16);
            invokedCount++;
        };
        Event<int> event;
        size_t listenerID = event.addListener(listener);
        event.invoke(invokedCount + 16);
        CHECK(invokedCount == 1);
        event.invoke(invokedCount + 16);
        CHECK(invokedCount == 2);
        event.removeListener(listenerID);
        event.invoke(invokedCount + 16);
        CHECK(invokedCount == 2);
    }
}