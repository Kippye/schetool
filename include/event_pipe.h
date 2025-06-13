#pragma once

#include "event.h"
#include <stdexcept>

template <typename... Types>
class EventPipeLink {
    private:
        bool m_isLinked = false;
        size_t m_listenerID = 0;
        Event<Types...>& m_sourceEvent;

    public:
        EventPipeLink(Event<Types...>& sourceEvent, const std::function<void(Types...)>& listener)
            : m_sourceEvent(sourceEvent) {
            m_listenerID = sourceEvent.addListener(listener);
            m_isLinked = true;
        }
        void disconnect() {
            if (m_isLinked) {
                m_sourceEvent.removeListener(m_listenerID);
            }
        }
};

template <typename... Types>
class EventPipe {
    private:
        size_t m_linkID = 0;
        std::map<size_t, EventPipeLink<Types...>> m_links;
        std::function<void(Types...)> m_listener = [&](Types... args) { m_outEvent.invoke(args...); };
        Event<Types...> m_outEvent;

    public:
        EventPipe() {
        }
        ~EventPipe() {
            for (auto [id, pipeLink] : m_links) {
                pipeLink.disconnect();
            }
        }
        // Adds the function as a listener to the output of this event pipe. Returns the ID for the EventPipeLink in the links map (in case it needs to be removed later).
        size_t addListener(const std::function<void(Types...)>& listener) {
            return m_outEvent.addListener(listener);
        }
        // Remove a listener from this event pipe.
        void removeListener(size_t ID) {
            m_outEvent.removeListener(ID);
        }
        // Invoke the event pipe's output event directly.
        void invoke(Types... args) {
            m_outEvent.invoke(args...);
        }
        // Link an event to the event pipe. When the provided event is invoked, this event pipe will also be invoked.
        size_t addEvent(Event<Types...>& event) {
            m_links.insert({m_linkID, EventPipeLink<Types...>(event, m_listener)});
            m_linkID++;
            return m_linkID - 1;
        }
        // Link another event pipe to this one as if it were a regular event. When the provided event pipe is invoked, this event pipe will also be invoked. The provided event pipe cannot be the same instance.
        size_t addEvent(EventPipe<Types...>& eventPipe) {
            if (std::addressof(eventPipe) == this) {
                throw std::invalid_argument("The added event pipe cannot be the same instance as the event pipe being added to.");
            }
            m_links.insert({m_linkID, EventPipeLink<Types...>(eventPipe.getOutputEvent(), m_listener)});
            m_linkID++;
            return m_linkID - 1;
        }
        // Remove an event from the event pipe. The event pipe will no longer be invoked when this event is invoked.
        void removeEvent(size_t ID) {
            auto link_pair = m_links.find(ID);
            if (link_pair != m_links.end()) {
                std::get<1>(*link_pair).disconnect();
                m_links.erase(ID);
            }
        }
        // Get the number of events that are currently linked to this event pipe.
        size_t getLinkedEventCount() const {
            return m_links.size();
        }
        size_t getListenerCount() const {
            return m_outEvent.getListenerCount();
        }
        // Get the output event of the event pipe.
        Event<Types...>& getOutputEvent() {
            return m_outEvent;
        }
};