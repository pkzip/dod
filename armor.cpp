#include "armor.h"
#include "common.h"
#include "notes.h"
#include "util.h"

vector<armor_def> armor_defs = {
    {1,"cloth armor", 8},
    {2,"leather armor", 7},
    {3,"chain mail", 5},
    {4,"plate mail", 3}
};

string armor::name() const
{
    if (!id || bonus == 0) return desc;
    return desc + " " + bonus_to_str(bonus);
}

void armor::use()
{
    if (player.worn_armor != nullptr) {
        if (player.worn_armor->cursed()) {
            notes::add("unable to remove cursed armor");
            return;
        }
    }
    if (player.worn_armor == this) {
        player.worn_armor = nullptr;
        notes::add("armor removed");
        player.ac = DEFAULT_AC;
        return;
    }
    player.worn_armor = this;
    player.ac = ac - bonus;
    id = true;
    notes::add("now wearing: " + name());
}

armor *make_armor_for_level(const int level)
{
    vector<int> candidates;
    for (int i=0; i < armor_defs.size(); i++) {
        if (level >= armor_defs[i].first_level) {
            candidates.push_back(i);
        }
    }
    int sel = candidates[randint(0,candidates.size()-1)];
    const armor_def& def = armor_defs[sel];
    const int roll = dice("3d6").roll();
    int bonus = 0;
    switch(roll) {
        case 3: bonus = -3; break;
        case 4:
        case 5: bonus = -2; break;
        case 6:
        case 7: bonus = -1; break;
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13: bonus = 0; break;
        case 14:
        case 15: bonus = 1; break;
        case 16:
        case 17: bonus = 2; break;
        case 18: bonus = 3; break;
    }
    return new armor(def.name, def.ac, bonus);
}
