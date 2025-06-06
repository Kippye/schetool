#pragma once

#include <functional>
#include <map>

template <typename... Types>
class Event {
    private:
        std::map<size_t, std::function<void(Types...)>> m_listeners;
        size_t m_listenerID = 0;

    public:
        Event() {
        }
        // Adds the function as a listener to this event. Returns the ID for the function in the listener map (in case it needs to be removed later).
        size_t addListener(const std::function<void(Types...)>& listener) {
            m_listeners.insert({m_listenerID, listener});
            m_listenerID++;
            return m_listenerID - 1;
        }
        void removeListener(size_t ID) {
            if (m_listeners.find(ID) != m_listeners.end()) {
                m_listeners.erase(ID);
            }
        }
        void invoke(Types... args) {
            for (auto& listener : m_listeners) {
                listener.second(args...);
            }
        }
        size_t getListenerCount() const {
            return m_listeners.size();
        }
};

template <typename TemplateType, typename... ArgTypes>
class GuiTemplateEvent {
    private:
        std::map<size_t, std::function<void(TemplateType, ArgTypes...)>> m_listeners;
        size_t m_listenerID = 0;

    public:
        GuiTemplateEvent() {
        }
        // Adds the function as a listener to this event. Returns the ID for the function in the listener map (in case it needs to be removed later).
        size_t addListener(std::function<void(TemplateType, ArgTypes...)>& listener) {
            m_listeners.insert({m_listenerID, listener});
            m_listenerID++;
            return m_listenerID - 1;
        }
        void removeListener(size_t ID) {
            if (m_listeners.find(ID) != m_listeners.end()) {
                m_listeners.erase(ID);
            }
        }
        void invoke(TemplateType&, ArgTypes&... args) {
            for (auto& listener : m_listeners) {
                listener.second(args...);
            }
        }
};