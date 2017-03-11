#pragma once

#include <string>
#include <vector>
#include <libtcod.hpp>
#include "levelmap.h"
#include "util.h"

typedef struct
{
    int first_level;
    int last_level;
    int ch;
    TCODColor col;
    std::string name;
    int base_hp;
    dice damage;
} creature_def;

extern std::vector<creature_def> creature_defs;

typedef enum
{
    ASLEEP,
    WANDERING,
    CHASING
} creature_state;

class creature : public ITCODPathCallback
{
public:
    creature(level_map *level, const int def_idx);
    bool move();
    int get_ac() const;
    void take_damage(const int damage);
    bool is_dead() const { return hp <= 0; }
    int xp_value() const;
    virtual float getWalkCost(int x1, int y1, int x2, int y2, void *userdata) const;
    creature_def *def;
    level_map *level;
    coords pos;
    creature_state state;
    int hp;
private:
    bool valid_move(const coords& c) const;
};

void populate_level(level_map *level);
