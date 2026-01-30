#pragma once

#include "messages/message_queue.h"

enum class StatusMessageType {
    Info,
    Warning,
    Error
};

struct StatusMessage : public Message {
        StatusMessageType messageType;

        StatusMessage(StatusMessageType messageType, Message message) : Message(message) {
            this->messageType = messageType;
        }

        static StatusMessage info(Message message) {
            return StatusMessage(StatusMessageType::Info, message);
        }

        static StatusMessage warning(Message message) {
            return StatusMessage(StatusMessageType::Warning, message);
        }

        static StatusMessage error(Message message) {
            return StatusMessage(StatusMessageType::Error, message);
        }
};

class StatusMessageQueue : public MessageQueue<StatusMessage> {};