#include "common.h"
#include "notes.h"
#include "player_character.h"

player_character::player_character() :
    pos(MAP_W,MAP_H),
    hp(12),
    max_hp(12),
    strength(16),
    max_strength(16),
    gold(0),
    xp(0),
    clev(1),
    ac(DEFAULT_AC),
    worn_armor(nullptr),
    wielding(nullptr),
    regen(0)
{
}

void player_character::process_turn()
{
    if (hp < max_hp) {
        regen += strength + clev;
        if (regen >= 200) {
            hp++;
            regen = 0;
        }
    }
}

static int xp_to_lev(const int xp)
{
    int lev = 1;
    int n = 0;
    while (xp > (1<<n)*100) {
        lev++;
        n++;
    }
    return lev;
}

void player_character::grant_xp(const int add)
{
    xp += add;
    const int new_lev = xp_to_lev(xp);
    if (new_lev > clev) {
        clev = new_lev;
        max_hp = 12 + (4*clev);
        hp = max_hp;
        regen = 0;
        notes::add("welcome to level " + int_to_str(clev));
    }
}
