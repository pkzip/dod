#include <assert.h>
#include "levelmap.h"
#include "util.h"

const int GRID_W = 3;
const int GRID_H = 3;

level_map::level_map(const int w, const int h) :
    cells(w*h),
    rooms(GRID_W*GRID_H),
    level_w(w),
    level_h(h),
    entry(w,h)
{
}

const map_cell& level_map::cell(const int x, const int y) const
{
    return cells[y*level_w+x];
}

const map_cell& level_map::cell(const coords& c) const
{
    return cells[c.lin()];
}

void level_map::generate_classic()
{
    coords c(GRID_W,GRID_H);
    c.setrand();
    const int starting_room = c.lin();
    int exit_room;
    // build initial chain of connected rooms
    while (1) {
        bool up = (!c.top_edge()) && (!rooms[c.up().lin()].connected());
        bool down = (!c.bottom_edge()) && (!rooms[c.down().lin()].connected());
        bool left = (!c.left_edge()) && (!rooms[c.left().lin()].connected());
        bool right = (!c.right_edge()) && (!rooms[c.right().lin()].connected());
        if (!(up || down || left || right)) {
            exit_room = c.lin();
            break;
        }
        coords next = c;
        while (next == c) {
            int dir = randint(0, 3);
            switch(dir) {
                case 0: if (up) {
                        next = c.up();
                    }
                    break;
                case 1: if (down) {
                        next = c.down();
                    }
                    break;
                case 2: if (left) {
                        next = c.left();
                    }
                    break;
                case 3: if (right) {
                        next = c.right();
                    }
                    break;
            }
        }
        rooms[c.lin()].connections.push_back(next.lin());
        rooms[next.lin()].connections.push_back(c.lin());
        c = next;
    }

    // connect any remaining rooms to the chain
    coords last = c;
    while (1) {
        int unconnected = 0;
        for (int i=0; i < GRID_W*GRID_H; i++) {
            if (!rooms[i].connected()) {
                c.setlin(i);
                bool up = (!c.top_edge()) && (rooms[c.up().lin()].connected());
                bool down = (!c.bottom_edge()) && (rooms[c.down().lin()].connected());
                bool left = (!c.left_edge()) && (rooms[c.left().lin()].connected());
                bool right = (!c.right_edge()) && (rooms[c.right().lin()].connected());
                if (!(up || down || left || right)) {
                    unconnected++;
                    continue;
                }
                int dir = randint(0,3);
                coords next = c;
                switch(dir) {
                    case 0: if (up) {
                            next = c.up();
                        }
                        break;
                    case 1: if (down) {
                            next = c.down();
                        }
                        break;
                    case 2: if (left) {
                            next = c.left();
                        }
                        break;
                    case 3: if (right) {
                            next = c.right();
                        }
                        break;
                }
                if (next == c) {
                    unconnected++;
                } else {
                    rooms[i].connections.push_back(next.lin());
                    rooms[next.lin()].connections.push_back(i);
                    last = next;
                }
            }
        }
        if (unconnected == 0) {
            break;
        }
    }

    // positions the rooms on the grid
    const int xdiv1 = 26; //randint(24,28);
    const int xdiv2 = 53; //randint(51,55);
    const int ydiv1 = 7; //randint(6,8);
    const int ydiv2 = 14; //randint(13,15);
    for (c.y = 0; c.y < c.h; c.y++) {
        for (c.x = 0; c.x < c.w; c.x++) {
            coords ul(level_w,level_h);
            coords lr(level_w,level_h);
            switch(c.x) {
                case 0:
                    ul.x = 0;
                    lr.x = xdiv1-1;
                    break;
                case 1:
                    ul.x = xdiv1+1;
                    lr.x = xdiv2-1;
                    break;
                case 2:
                    ul.x = xdiv2+1;
                    lr.x = level_w-1;
                    break;
            }
            switch(c.y) {
                case 0:
                    ul.y = 0;
                    lr.y = ydiv1-1;
                    break;
                case 1:
                    ul.y = ydiv1+1;
                    lr.y = ydiv2-1;
                    break;
                case 2:
                    ul.y = ydiv2+1;
                    lr.y = level_h-1;
                    break;
            }
            const int mh = lr.y-ul.y;
            const int mw = lr.x-ul.x;
            const int rh = randint(3,mh);
            const int rw = randint(3,mw);
            const int xoffset = randint(0,mw-rw);
            const int yoffset = randint(0,mh-rh);
            const coords rul(level_w,level_h,ul.x+xoffset,ul.y+yoffset);
            const coords rlr(level_w,level_h,ul.x+xoffset+rw,ul.y+yoffset+rh);
            rooms[c.lin()].x1 = rul.x;
            rooms[c.lin()].y1 = rul.y;
            rooms[c.lin()].x2 = rlr.x;
            rooms[c.lin()].y2 = rlr.y;
            //printf("room=%d (lw=%d,lh=%d,x1=%d,y1=%d,x2=%d,y2=%d)\n",c.lin(),level_w,level_h,rul.x,rul.y,rlr.x,rlr.y);
            coords p(level_w,level_h);
            assert(rul.x < level_w);
            assert(rlr.x < level_w);
            assert(rul.y < level_h);
            assert(rlr.y < level_h);
            for (p.x=rul.x; p.x<=rlr.x; p.x++) {
                for (p.y=rul.y; p.y<=rlr.y; p.y++) {
                    int ch = '.';
                    TCODColor col = TCODColor::sepia;
                    if (p.y == rul.y) {
                        cells[p.lin()].set_wall();
                        if (p.x == rul.x) {
                            ch = 201;
                        } else if (p.x == rlr.x) {
                            ch = 187;
                        } else {
                            ch = 205;
                        }
                    } else if (p.y == rlr.y) {
                        cells[p.lin()].set_wall();
                        if (p.x == rul.x) {
                            ch = 200;
                        } else if (p.x == rlr.x) {
                            ch = 188;
                        } else {
                            ch = 205;
                        }
                    } else if (p.x == rul.x || p.x == rlr.x) {
                        cells[p.lin()].set_wall();
                        ch = 186;
                    } else {
                        col = TCODColor::green;
                        ch = 250;
                        cells[p.lin()].set_walkable();
                    }
                    cells[p.lin()].ch = ch;
                    cells[p.lin()].col = col;
                }
            }
        }
    }

    // draw the cooridors between rooms
    for (size_t i=0; i<GRID_W*GRID_H; i++) {
        for (auto j=rooms[i].connections.begin(); j!=rooms[i].connections.end(); ++j) {
            const int connect_to = *j;
            if (connect_to < i) continue;  // tunnel to lower number room already drawn in previous iteration
            const coords begin = room_random(rooms[i]);
            const coords end = room_random(rooms[connect_to]);
            TCODPath pathfinder(level_w,level_h,this,NULL,0.0f);
            pathfinder.compute(begin.x,begin.y,end.x,end.y);
            for (int k=0; k<pathfinder.size(); k++) {
                coords c(level_w,level_h);
                pathfinder.get(k,&c.x,&c.y);
                map_cell& m = cells[c.lin()];
                if (m.is_walkable()) continue;
                if (m.is_wall()) {
                    m.set_walkable();
                    m.ch = 206;  // door
                } else {
                    m.ch = 178;  // cooridor
                    m.col = TCODColor::grey;
                    m.set_cooridor();
                    m.set_walkable();
                }
            }
        }
    }

    // add the stairs, preferably not adjacent to a door
    coords exit(level_w,level_h);
    int exit_rounds = 0;
    while (exit_rounds++ < 10) { // possible to have a tiny central room with adjacent doors to all cells
        exit = room_random(rooms[exit_room]);
        if (!exit.top_edge() && cells[exit.up().lin()].is_door()) continue;
        if (!exit.bottom_edge() && cells[exit.down().lin()].is_door()) continue;
        if (!exit.left_edge() && cells[exit.left().lin()].is_door()) continue;
        if (!exit.right_edge() && cells[exit.right().lin()].is_door()) continue;
        break;
    };
    cells[exit.lin()].ch = '>';
    cells[exit.lin()].col = TCODColor::white;
    cells[exit.lin()].set_exit();

    entry = room_random(rooms[starting_room]);
}

float level_map::getWalkCost(int x1, int y1, int x2, int y2, void *userdata) const
{
    if (x1 != x2 && y1 != y2) {
        return 0.0f;  // no diagonal moves
    }
    const map_cell& t = cell(x2,y2);
    if (t.is_wall() && !t.is_walkable()) {
        if (t.ch == 205 || t.ch == 186) {
            return 3.0f;
        } else {
            return 0.0f;  // no wall corners
        }
    } else {
        return 1.0f;
    }
}

coords level_map::room_center(const map_room& r) const
{
    coords c(level_w,level_h);
    c.x = r.x1 + (r.x2 - r.x1)/2;
    c.y = r.y1 + (r.y2 - r.y1)/2;
    return c;
}

coords level_map::room_random(const map_room& r) const
{
    coords c(level_w,level_h);
    c.x = randint(r.x1+1,r.x2-1);
    c.y = randint(r.y1+1,r.y2-1);
    return c;
}
