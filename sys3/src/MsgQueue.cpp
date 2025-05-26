#include "MsgQueue.h"
#include <cstring>
#include <stdexcept>
#include <sys/ipc.h>
#include <fstream>
#include <Logger.h>
#include <memory>
#include <sys/msg.h>

MsgQueue::MsgQueue(int id, bool create, const std::shared_ptr<Logger> &logger) {
    this->logger = logger;
    path = "msg_queue_" + std::to_string(id) + ".key";
    if (create) {
        std::ofstream(path.c_str()).put('\n');
        logger->debug("[QUEUE]: Created file " + path + " for queue " + std::to_string(id));
    }

    key_t key = ftok(path.c_str(), id);
    if (key == -1) {
        throw std::runtime_error("Failed to generate key");
        logger->error("[QUEUE]: Failed to generate key " + std::to_string(id));
    }

    int flags = 0666 | (create ? IPC_CREAT : 0);
    msg_id = msgget(key, flags);
    if (msg_id == -1) {
        throw std::runtime_error("Failed to get message queue.");
        logger->error("[QUEUE]: Failed to open message queue " + std::to_string(id));
    }
    logger->debug("[QUEUE]: Opened message queue " + std::to_string(msg_id));
}

void MsgQueue::detach(bool remove) const {
    if (remove) {
        msgctl(msg_id, IPC_RMID, nullptr);
        std::remove(path.c_str());
        logger->debug("[QUEUE]: Closed message queue " + std::to_string(msg_id));
    }
}

MsgQueue::~MsgQueue() {
    detach(false);
}

void MsgQueue::send_message(long who, const std::string& msg) const {
    message_buffer buf{};
    buf.mtype = who;
    strcpy(buf.mtext, msg.c_str());

    if (msgsnd(msg_id, &buf, sizeof(buf.mtext), 0) == -1) {
        throw std::runtime_error("Failed to send message to queue.");
        logger->error("[QUEUE]: Failed to send message to queue " + std::to_string(msg_id));
    }
    logger->debug("[QUEUE]: Sent <" + msg + "> to queue " + std::to_string(msg_id));
}

std::pair<long, std::string> MsgQueue::receive_message(long type) const {
    message_buffer received{};
    if (msgrcv(msg_id, &received, sizeof(received.mtext), type, 0) == -1) {
        throw std::runtime_error("Failed to receive message from queue.");
        logger->error("[QUEUE]: Failed to receive message from queue " + std::to_string(msg_id));
    }
    logger->debug("[QUEUE]: Received from queue " + std::to_string(msg_id));
    return std::make_pair(received.mtype, received.mtext);
}