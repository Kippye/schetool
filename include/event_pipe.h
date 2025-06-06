#pragma once

#include "event.h"

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
        // Adds the function as a listener to the output of this event pipe. Returns the ID for the EventPipeLink in the links map (in case it needs to be removed later).
        size_t addListener(const std::function<void(Types...)>& listener) {
            return m_outEvent.addListener(listener);
        }
        void removeListener(size_t ID) {
            m_outEvent.removeListener(ID);
        }
        void invoke(Types... args) {
            m_outEvent.invoke(args...);
        }

        size_t addEvent(Event<Types...>& event) {
            m_links.insert({m_linkID, EventPipeLink<Types...>(event, m_listener)});
            m_linkID++;
            return m_linkID - 1;
        }
        void removeEvent(size_t ID) {
            auto link_pair = m_links.find(ID);
            if (link_pair != m_links.end()) {
                std::get<1>(*link_pair).disconnect();
                m_links.erase(ID);
            }
        }
        size_t getLinkedEventCount() const {
            return m_links.size();
        }
};