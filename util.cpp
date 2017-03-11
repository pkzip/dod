#include <stdlib.h>
#include <sstream>
#include <libtcod.hpp>
#include "util.h"

void coords::setrand()
{
    const int r = randint(0,w*h-1);
    x = r%w;
    y = r/w;
}

bool coords::adjacent(const coords& c) const
{
    return ((abs(c.x-x) == 1) && (c.y == y)) ||
           ((abs(c.y-y) == 1) && (c.x == x));
}

dice::dice(const int mult, const int sides, const int mod) :
    mult(mult), sides(sides), mod(mod)
{
}

dice::dice(const char *s)
{
    parse(s);
}

string dice::to_string() const
{
    stringstream r;
    if (mult > 1) {
        r << mult;
    }
    r << "d" << sides;
    if (mod < 0) {
        r << mod;
    } else if (mod > 0) {
        r << "+" << mod;
    }
    return r.str();
}

bool dice::parse(const string& s)
{
    const char* p = s.c_str();
    string mult_s;
    while (*p != 'd') {
        if (isdigit(*p)) {
            mult_s += *p;
        } else {
            return false;
        }
        p++;
    }
    p++;
    if (mult_s.empty()) {
        mult = 1;
    } else {
        mult = atoi(mult_s.c_str());
    }
    string sides_s;
    while (*p != '+' && *p != '-' && *p != '\0') {
        if (isdigit(*p)) {
            sides_s += *p;
        } else {
            return false;
        }
        p++;
    }
    if (sides_s.empty()) {
        return false;
    }
    sides = atoi(sides_s.c_str());
    if (*p == '\0') {
        mod = 0;
        return true;
    }
    const bool neg = (*p == '-');
    p++;
    string mod_s;
    while (*p != '\0') {
        if (isdigit(*p)) {
            mod_s += *p;
        } else {
            return false;
        }
        p++;
    }
    if (mod_s.empty()) {
        return false;
    }
    mod = atoi(mod_s.c_str());
    if (neg) {
        mod *= -1;
    }
    return true;
}

int dice::roll(const bool clamp_neg) const
{
    TCODRandom* rng = TCODRandom::getInstance();
    int r = 0;
    for (int i = 0; i < mult; ++i) {
        r += rng->getInt(1, sides);
    }
    r += mod;
    if (clamp_neg && r < 0) {
        r = 0;
    }
    return r;
}

bool attack(const int target_ac, const int attack_bonus)
{
    const int roll = randint(1,20);
    bool hit;
    if (roll == 1) {
        hit = false;
    } else if (roll == 20) {
        hit = true;
    } else {
        const int attack_bonus = 0; // attack_bonus = base_attack_bonus + strength_modifier + size_modifier
        hit = roll + attack_bonus >= target_ac;
    }
    return hit;
}

int compute_damage(const dice& d, const int damage_penalty)
{
    int v = d.roll();
    v -= damage_penalty;
    if (v < 1) v = 1;
    return v;
}

int randint(const int min, const int max)
{
    return TCODRandom::getInstance()->get(min,max);
}

string int_to_str(const int i)
{
    stringstream ss;
    ss << i;
    return ss.str();
}
