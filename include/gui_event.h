#pragma once

#include <functional>
#include <map>
#include <type_traits>

template <typename... Types>
class GuiEvent
{
    private:
        std::map<size_t, std::function<void(Types...)>> m_listeners;
        size_t m_eventID = 0;
    public:
        GuiEvent()
        {
        }
        // Adds the function as a listener to this event. Returns the ID for the function in the listener map (in case it needs to be removed later).
        size_t addListener(std::function<void(Types...)>& listener)
        {
            m_listeners.insert({m_eventID, listener});
            m_eventID++;
            return m_eventID - 1;
        }
        void removeListener(size_t ID)
        {
            if (m_listeners.find(ID) != m_listeners.end())
            {
                m_listeners.erase(ID);
            }
        }
        void invoke(Types&&... args)
        {
            for (auto& listener: m_listeners)
            {
                listener.second(args...);
            }
        }
};