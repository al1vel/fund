#ifndef MSGQUEUE_H
#define MSGQUEUE_H
#include <string>

struct message_buffer {
    long mtype;
    char mtext[32];
};

class MsgQueue {
private:
    int msg_id = -1;
    std::string path;

public:
    MsgQueue(int id, bool create);

    void detach(bool remove) const;

    ~MsgQueue();

    void send_message(long who, const std::string& msg) const;

    std::pair<long, std::string> receive_message(long type) const;
};

#endif //MSGQUEUE_H
