#pragma once

#include <vector>
#include "creature.h"
#include "player_character.h"
#include "util.h"

using namespace std;

const int MAP_W = 80;
const int MAP_H = 23;
const int SCREEN_W = 80;
const int SCREEN_H = 25;
const int DEFAULT_AC = 10;
const int AMULET_LEVEL = 10;

extern player_character player;
extern vector<creature*> creatures;
extern string cause_of_death;
