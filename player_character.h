#pragma once

#include <string>
#include <vector>
#include "util.h"
using namespace std;

class item_base;
class armor;

class player_character
{
public:
    player_character();
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
    int ac;
    vector<item_base*> inv;
    armor *worn_armor;
private:
    int regen;
};
