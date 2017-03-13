#pragma once

#include <string>
#include <libtcod.hpp>
#include "notes.h"
using namespace std;

typedef enum
{
    GOLD,
    ARMOR,  // ASCII 8
    WEAPON, // ASCII 24
    AMULET  // ASCII 12
//    SCROLL, // ASCII 13
//    POTION,
} item_category;

class item_base
{
public:
    virtual ~item_base() {}
    virtual item_category category() const = 0;
    virtual int map_char() const = 0;
    virtual TCODColor map_col() const = 0;
    virtual string name() const = 0;
    virtual void use() = 0;
    virtual bool try_drop() = 0;
};

class gold_coins : public item_base
{
public:
    gold_coins(const int quantity) : quantity(quantity) {}
    ~gold_coins() {}
    item_category category() const { return GOLD; }
    int map_char() const { return '*'; }
    TCODColor map_col() const { return TCODColor::yellow; }
    string name() const { return "gold coins"; } // not called
    void use() {}  // not called
    bool try_drop() { return true; } // not called
    int count() const { return quantity; }
private:
    int quantity;
};

class amulet : public item_base
{
public:
    amulet() {}
    ~amulet() {}
    item_category category() const { return AMULET; }
    int map_char() const { return 12; }
    TCODColor map_col() const { return TCODColor::purple; }
    string name() const { return "amulet of yendor"; }
    void use() { notes::add("nothing happens"); }
    bool try_drop() { notes::add("you don't want to drop that"); return false; }
};
