#include "common.h"
#include "creature.h"
#include "notes.h"
#include "util.h"

creature::creature(level_map *level, const int def_idx) :
    level(level),
    state(ASLEEP),
    pos(level->make_coords())
{
    def = &creature_defs[def_idx];
    hp = def->base_hp;
}

bool creature::move()
{
    switch(state) {
    case ASLEEP:
        if (randint(1,10) == 1) {
            state = WANDERING;
        }
        break;

    case WANDERING:
        if (randint(1,10) == 1) {
            state = ASLEEP;
        } else {
            const int r = randint(0,3);
            switch(r) {
                case 0: if (valid_move(pos.up())) pos = pos.up(); break;
                case 1: if (valid_move(pos.down())) pos = pos.down(); break;
                case 2: if (valid_move(pos.left())) pos = pos.left(); break;
                case 3: if (valid_move(pos.right())) pos = pos.right(); break;
            }
        }
        break;

    case CHASING:
        if (pos.adjacent(player.pos)) {
            return true;
        } else {
            coords c = level->make_coords();
            TCODPath pathfinder(c.w,c.h,this,NULL,0.0f);
            pathfinder.compute(pos.x,pos.y,player.pos.x,player.pos.y);
            if (pathfinder.size() > 0) {
                pathfinder.get(0,&c.x,&c.y);
                pos = c;
            } else {
                state = WANDERING;
            }
        }
        break;
    }
    return false;
}

int creature::get_ac() const
{
    return 10; // + armor_bonus + shield_bonus + dexterity_modifier + size_modifier
}

void creature::take_damage(const int damage)
{
    hp -= damage;
    state = CHASING;
}

int creature::xp_value() const
{
    const int max_damage = (def->damage.mult*def->damage.sides)+def->damage.mod;
    return 10+(2*max_damage);
}

float creature::getWalkCost(int x1, int y1, int x2, int y2, void *userdata) const
{
    if (x1 != x2 && y1 != y2) {
        return 0.0f;  // no diagonal moves
    }
    coords c = level->make_coords();
    c.x = x2;
    c.y = y2;
    if (!level->cell(c).is_walkable()) {
        return 0.0f;
    }
    return 1.0f;
}

bool creature::valid_move(const coords& c) const
{
    if (!level->cell(c).is_walkable()) return false;
    if (player.pos == c) return false;
    for (auto i = creatures.begin(); i != creatures.end(); ++i) {
        creature *cr = *i;
        if (cr->pos == c) return false;
    }
    return true;
}

void populate_level(level_map *level)
{
    for (int i=0; i < level->num_rooms(); i++) {
        const map_room& r = level->room(i);
        creature *c = new creature(level, 0);
        c->pos = level->room_random(r);
        creatures.push_back(c);
    }
}

vector<creature_def> creature_defs = {
    {'b', TCODColor::green, "bat", 4, "1d4"}
};

