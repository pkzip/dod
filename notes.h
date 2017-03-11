#pragma once

#include <string>

using std::string;

class notes
{
public:
    static void add(const string& msg);
    static bool empty();
    static string next();
};
