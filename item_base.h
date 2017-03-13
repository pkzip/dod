#pragma once

#include <string>
#include <libtcod.hpp>
using namespace std;

typedef enum
{
    GOLD,
    ARMOR,  // ASCII 8
    SCROLL, // ASCII 13
    POTION,
    AMULET  // ASCII 12
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
};

class gold_coins : public item_base
{
public:
    gold_coins(const int quantity) : quantity(quantity) {}
    ~gold_coins() {}
    item_category category() const { return GOLD; }
    int map_char() const { return '*'; }
    TCODColor map_col() const { return TCODColor::yellow; }
    string name() const { return "gold coins"; }
    void use() {}  // not called
    int count() const { return quantity; }
private:
    int quantity;
};
