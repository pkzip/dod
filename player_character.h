#pragma once

#include <string>
#include "util.h"
using namespace std;

class player_character
{
public:
    player_character();
    int ac() const;
    void process_turn();
    void grant_xp(const int add);
    string name;
    coords pos;
    int hp;
    int max_hp;
    int strength;
    int max_strength;
    int gold;
    int xp;
    int clev;
private:
    int regen;
};
