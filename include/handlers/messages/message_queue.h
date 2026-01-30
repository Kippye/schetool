#pragma once

#include <queue>
#include <string>
#include <optional>

struct Message {
        std::string message;

        Message(std::string msg) : message(msg) {
        }

        Message(const char* msg) : message(msg) {
        }
};

template <typename T>
concept message = std::is_base_of<Message, T>::value;

template <message M>
class MessageQueue {
    protected:
        static std::queue<M> m_messageQueue;

    public:
        static void push(M message) {
            m_messageQueue.push(message);
        }

        static size_t get() {
            return m_messageQueue.size();
        }

        static std::optional<M> pop() {
            auto message = peek();
            if (m_messageQueue.size() > 0) {
                m_messageQueue.pop();
            }
            return message;
        }

        static std::optional<M> peek() {
            if (m_messageQueue.size() == 0) {
                return std::nullopt;
            }

            return m_messageQueue.front();
        }
};

template <message M>
std::queue<M> MessageQueue<M>::m_messageQueue = {};