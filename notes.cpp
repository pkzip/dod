#include <deque>
#include "notes.h"

std::deque<string> msgs;

void notes::add(const string& msg)
{
    msgs.push_back(msg);
}

bool notes::empty()
{
    return msgs.empty();
}

string notes::next()
{
    if (msgs.empty()) {
        return "";
    } else {
        string msg = msgs.front();
        msgs.pop_front();
        return msg;
    }
}
