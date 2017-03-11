#pragma once

#include <string>
using namespace std;

class coords
{
public:
    coords(const int w, const int h, const int x=0, const int y=0) : w(w),h(h),x(x),y(y) {}
    int w,h,x,y;
    bool top_edge() const { return y == 0; }
    bool bottom_edge() const { return y == h-1; }
    bool left_edge() const { return x == 0; }
    bool right_edge() const { return x == w-1; }
    coords up() const { return coords(w, h, x, y>0 ? y-1 : y); }
    coords down() const { return coords(w, h, x, y<h ? y+1 : y); }
    coords left() const { return coords(w, h, x > 0 ? x-1 : x, y); }
    coords right() const { return coords(w, h, x < w ? x+1 : x, y); }
    int lin() const { return y*w+x; }
    void setlin(const int i) { x = i%w; y = i/w; }
    void setrand();
    bool operator == (const coords& rhs) const { return (w == rhs.w) && (h == rhs.h) && (x == rhs.x) && (y == rhs.y); }
    bool operator != (const coords& rhs) const { return !(*this == rhs); }
    bool adjacent(const coords& c) const;
};

class dice
{
public:
    dice(const int mult = 1, const int sides = 6, const int mod = 0);
    dice(const char *s);
    string to_string() const;
    bool parse(const string& s);
    int roll(const bool clamp_neg = true) const;
    int mult;
    int sides;
    int mod;
};

int randint(const int min, const int max);
bool attack(const int target_ac, const int attack_bonus);
int compute_damage(const dice& d, const int damage_penalty);
string int_to_str(const int i);
