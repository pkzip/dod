#pragma once

#include <vector>
#include "item_base.h"
using namespace std;

typedef struct
{
    int first_level;
    string name;
    int ac;
} armor_def;

extern vector<armor_def> armor_defs;

class armor : public item_base
{
public:
    armor(const string& desc, const int ac, const int bonus) : desc(desc), ac(ac), bonus(bonus), id(false) {}
    ~armor() {}
    item_category category() const { return ARMOR; }
    int map_char() const { return 8; }
    TCODColor map_col() const { return TCODColor::silver; }
    string name() const;
    void use();
    bool try_drop();
    bool identified() const { return id; }
    bool cursed() const { return bonus < 0; }
private:
    string desc;
    int ac;
    int bonus;
    bool id;
};

armor *make_armor_for_level(const int level);
