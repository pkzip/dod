#pragma once

#include <vector>
#include "creature.h"
#include "levelmap.h"
#include "player_character.h"
#include "util.h"

using namespace std;

const int MAP_W = 80;
const int MAP_H = 23;
const int SCREEN_W = 80;
const int SCREEN_H = 25;

extern player_character player;
extern level_map level;
extern vector<creature*> creatures;
extern string cause_of_death;
