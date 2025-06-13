#include <catch2/catch_test_macros.hpp>
#include <catch2/reporters/catch_reporter_event_listener.hpp>
#include <catch2/reporters/catch_reporter_registrars.hpp>

#include "event_pipe.h"
#include <functional>

TEST_CASE("EventPipe") {
    SECTION("Add listeners") {
        std::function<void(int)> listener1 = [&](int arg) {};
        std::function<void(int)> listener2 = [&](int arg) {};
        EventPipe<int> eventPipe;
        CHECK(eventPipe.getListenerCount() == 0);
        eventPipe.addListener(listener1);
        CHECK(eventPipe.getListenerCount() == 1);
        eventPipe.addListener(listener2);
        CHECK(eventPipe.getListenerCount() == 2);
    }

    SECTION("Remove listeners") {
        std::function<void(int)> listener1 = [&](int arg) {};
        std::function<void(int)> listener2 = [&](int arg) {};
        EventPipe<int> eventPipe;
        CHECK(eventPipe.getListenerCount() == 0);
        size_t firstListenerID = eventPipe.addListener(listener1);
        CHECK(eventPipe.getListenerCount() == 1);
        size_t secondListenerID = eventPipe.addListener(listener2);
        CHECK(eventPipe.getListenerCount() == 2);
        eventPipe.removeListener(firstListenerID);
        CHECK(eventPipe.getListenerCount() == 1);
        eventPipe.removeListener(firstListenerID);
        CHECK(eventPipe.getListenerCount() == 1);
        eventPipe.removeListener(secondListenerID);
        CHECK(eventPipe.getListenerCount() == 0);
    }

    SECTION("Invoke event") {
        int invokedCount = 0;
        std::function<void(int)> listener = [&](int arg) {
            CHECK(arg == invokedCount + 16);
            invokedCount++;
        };
        EventPipe<int> eventPipe;
        size_t listenerID = eventPipe.addListener(listener);
        eventPipe.invoke(invokedCount + 16);
        CHECK(invokedCount == 1);
        eventPipe.invoke(invokedCount + 16);
        CHECK(invokedCount == 2);
        eventPipe.removeListener(listenerID);
        eventPipe.invoke(invokedCount + 16);
        CHECK(invokedCount == 2);
    }

    SECTION("Pipe invoked event") {
        int invokedCount = 0;
        std::function<void(int)> listener = [&](int arg) {
            CHECK(arg == invokedCount + 16);
            invokedCount++;
        };
        Event<int> sourceEvent;
        EventPipe<int> eventPipe;
        // Events aren't piped unless they are added (duh)
        eventPipe.addListener(listener);
        sourceEvent.invoke(invokedCount + 16);
        CHECK(invokedCount == 0);
        // Add an event to the pipe and it gets piped through when invoked
        size_t eventID = eventPipe.addEvent(sourceEvent);
        sourceEvent.invoke(invokedCount + 16);
        CHECK(invokedCount == 1);
        // Events are no longer passed through after being removed
        eventPipe.removeEvent(eventID);
        sourceEvent.invoke(invokedCount + 16);
        CHECK(invokedCount == 1);
    }

    SECTION("Removes listeners from events when destroyed") {
        Event<int> sourceEvent;
        {
            EventPipe<int> eventPipe;
            eventPipe.addEvent(sourceEvent);
            CHECK(eventPipe.getLinkedEventCount() == 1);
            CHECK(sourceEvent.getListenerCount() == 1);
        }
        // When the event pipe goes out of scope and gets destroyed, it should automatically remove its listeners from events
        CHECK(sourceEvent.getListenerCount() == 0);
    }

    SECTION("Can't pipe itself") {
        EventPipe<int> eventPipe;
        CHECK_THROWS_AS(eventPipe.addEvent(eventPipe), std::invalid_argument);
    }

    SECTION("Pipe another event pipe") {
        int invokedCount = 0;
        std::function<void(int)> listener = [&](int arg) {
            CHECK(arg == invokedCount + 16);
            invokedCount++;
        };
        Event<int> sourceEvent;
        EventPipe<int> middlePipe;
        EventPipe<int> endPipe;
        // When the event is invoked, it gets passed through the middle pipe to the end pipe and through its output event
        middlePipe.addEvent(sourceEvent);
        size_t middlePipeID = endPipe.addEvent(middlePipe);
        endPipe.addListener(listener);
        sourceEvent.invoke(invokedCount + 16);
        CHECK(invokedCount == 1);
        // When the middle pipe is removed, the end pipe no longer receives the source event
        endPipe.removeEvent(middlePipeID);
        sourceEvent.invoke(invokedCount + 16);
        CHECK(invokedCount == 1);
    }
}