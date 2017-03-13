#pragma once

#include <vector>
#include "item_base.h"
#include "util.h"
using namespace std;

typedef struct
{
    int first_level;
    string name;
    dice damage;
} weapon_def;

extern vector<weapon_def> weapon_defs;

class weapon : public item_base
{
public:
    weapon(const string& desc, const dice& damage, const int bonus) : desc(desc), damage(damage), bonus(bonus), id(false) {}
    ~weapon() {}
    item_category category() const { return WEAPON; }
    int map_char() const { return 24; }
    TCODColor map_col() const { return TCODColor::silver; }
    string name() const;
    void use();
    bool try_drop();
    bool identified() const { return id; }
    bool cursed() const { return bonus < 0; }
    const dice& get_damage() const { return damage; }
private:
    string desc;
    dice damage;
    int bonus;
    bool id;
};

weapon *make_weapon_for_level(const int level);
