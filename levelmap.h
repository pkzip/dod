#pragma once

#include <vector>
#include <libtcod.hpp>
#include "util.h"

using namespace std;

#define MAP_WALKABLE 0x01
#define MAP_WALL 0x02
#define MAP_COORIDOR 0x04
#define MAP_EXIT 0x08
#define MAP_SEEN 0x10
#define MAP_VISIBLE 0x20

class map_cell {
public:
    map_cell() : ch(' '), col(TCODColor::white), flags(0), room(-1) {}
    int ch;
    TCODColor col;
    int flags;
    int room;
    bool is_walkable() const { return (flags & MAP_WALKABLE) != 0; }
    void set_walkable() { flags = flags | MAP_WALKABLE; }
    bool is_wall() const { return (flags & MAP_WALL) != 0; }
    void set_wall() { flags = flags | MAP_WALL; }
    bool is_cooridor() const { return (flags & MAP_COORIDOR) != 0; }
    void set_cooridor() { flags = flags | MAP_COORIDOR; }
    bool is_door() const { return is_wall() && is_walkable(); }
    bool is_exit() const { return (flags & MAP_EXIT) != 0; }
    void set_exit() { flags = flags | MAP_EXIT; }
    bool have_seen() const { return (flags & MAP_SEEN) != 0; }
    void set_seen() { flags = flags | MAP_SEEN; }
    bool is_visible() const { return (flags & MAP_VISIBLE) != 0; }
    void set_visible() { flags = flags | MAP_VISIBLE; }
    void clear_visible() { flags = flags & (~MAP_VISIBLE); }
};

class map_room {
public:
    map_room() : x1(-1), y1(-1), x2(-1), y2(-1) {}
    map_room(int x1, int y1, int x2, int y2) : x1(x1), y1(y1), x2(x2), y2(y2) {}
    vector<int> connections;
    int x1,y1,x2,y2;
    bool lit;
    bool connected() const { return !connections.empty(); }
};

class level_map : public ITCODPathCallback {
public:
    level_map(const int w, const int h, const int dungeon_level);
    level_map(const level_map&) = delete;
    level_map& operator = (level_map&) = delete;
    const map_cell& cell(const int x, const int y) const;
    map_cell& cell_ref(const int x, const int y);
    const map_cell& cell(const coords& c) const;
    map_cell& cell_ref(const coords& c);
    const int num_rooms() const { return rooms.size(); }
    const map_room& room(const int idx) const { return rooms[idx]; }
    void generate_classic();
    coords get_entry_point() const { return entry; }
    coords make_coords() const { return coords(level_w,level_h); }
    virtual float getWalkCost(int x1, int y1, int x2, int y2, void *userdata) const;
    coords room_center(const map_room& r) const;
    coords room_random(const map_room& r) const;
    int get_dungeon_level() const { return dungeon_level; }
private:
    bool is_room_lit(const bool main_route) const;
    vector<map_cell> cells;
    vector<map_room> rooms;
    int level_w;
    int level_h;
    int dungeon_level;
    coords entry;
};
