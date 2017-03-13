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
            notes::add("unable to remove cursed " + player.worn_armor->name());
            return;
        }
    }
    if (player.worn_armor == this) {
        player.worn_armor = nullptr;
        notes::add("removed " + name());
        player.ac = DEFAULT_AC;
        return;
    }
    player.worn_armor = this;
    player.ac = ac - bonus;
    id = true;
    notes::add("now wearing: " + name());
}

bool armor::try_drop()
{
    if (player.worn_armor == this) {
        if (cursed()) {
            notes::add("unable to remove cursed armor");
            return false;
        }
        player.worn_armor = nullptr;
        player.ac = DEFAULT_AC;
    }
    return true;
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
    const int bonus = standard_bonus_roll();
    return new armor(def.name, def.ac, bonus);
}
