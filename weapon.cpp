#include "common.h"
#include "notes.h"
#include "util.h"
#include "weapon.h"

vector<weapon_def> weapon_defs = {
    {1,"club", "1d4"},
    {1,"dagger", "1d4+1"},
    {2,"short sword", "1d6"},
    {4,"long sword", "1d8"}
};

string weapon::name() const
{
    if (!id || bonus == 0) return desc;
    return desc + " " + bonus_to_str(bonus);
}

void weapon::use()
{
    if (player.wielding != nullptr) {
        if (player.wielding->cursed()) {
            notes::add("unable to stop wielding cursed " + player.wielding->name());
            return;
        }
    }
    if (player.wielding == this) {
        player.wielding = nullptr;
        notes::add("stopped wielding the " + name());
        return;
    }
    player.wielding = this;
    id = true;
    notes::add("now wielding a " + name());
}

bool weapon::try_drop()
{
    if (player.wielding == this) {
        if (cursed()) {
            notes::add("unable to stop wielding cursed " + player.wielding->name());
            return false;
        }
        player.wielding = nullptr;
    }
    return true;
}

weapon *make_weapon_for_level(const int level)
{
    vector<int> candidates;
    for (int i=0; i < weapon_defs.size(); i++) {
        if (level >= weapon_defs[i].first_level) {
            candidates.push_back(i);
        }
    }
    int sel = candidates[randint(0,candidates.size()-1)];
    const weapon_def& def = weapon_defs[sel];
    const int bonus = standard_bonus_roll();
    return new weapon(def.name, def.damage, bonus);
}
