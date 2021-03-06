#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <sstream>
#include <libtcod.hpp>
#include "armor.h"
#include "common.h"
#include "item_base.h"
#include "notes.h"
#include "weapon.h"

level_map *current_level = nullptr;
TCODConsole *io;
int turn;
int dlev;
bool exit_game;

void wait_continue()
{
    io->setDefaultForeground(TCODColor::cyan);
    io->setAlignment(TCOD_RIGHT);
    io->print(SCREEN_W-1,SCREEN_H-1,"[ Press SPACE to continue ]");
    io->setAlignment(TCOD_LEFT);
    io->flush();
    while (!io->isWindowClosed()) {
        TCOD_key_t key;
        TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS,&key,NULL,true);
        if (key.vk == TCODK_ENTER && (key.lalt || key.ralt)) {
            io->setFullscreen(!io->isFullscreen());
            io->flush();
        }
        if (key.vk == TCODK_SPACE) break;
   }
}

string readline(const int x, const int y, const int max)
{
    string s;
    TCOD_key_t key;
    TCODColor fg = io->getDefaultForeground();
    do {
        io->setDefaultForeground(TCODColor::lightestGrey);
        io->putChar(x+s.length(),y,219);
        io->flush();
        TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS,&key,NULL,true);
        if (isalnum(key.c) || key.c == 32) {
            if (s.length() < max) {
                char c = key.c;
                if (key.shift) c = toupper(c);
                io->setChar(x+s.length(),y,c);
                s += c;
            }
        } else if (key.vk == TCODK_BACKSPACE) {
            if (!s.empty()) {
                io->setDefaultForeground(fg);
                io->putChar(x+s.length(),y,32);
                s=s.substr(0,s.length()-1);
            }
        }
    }
    while (key.vk != TCODK_ENTER && !io->isWindowClosed());
    io->setDefaultForeground(fg);
    io->putChar(x+s.length(),y,32);
    return s;
}

void show_title()
{
    io->clear();
    io->setAlignment(TCOD_CENTER);
    io->setDefaultForeground(TCODColor::yellow);
    io->print(SCREEN_W/2,4,"Return to the Dungeons of Doom");
    io->setDefaultForeground(TCODColor::white);
    io->print(SCREEN_W/2,8,"2017 Seven Day Rogue-Like");
    io->setDefaultForeground(TCODColor::lime);
    io->print(SCREEN_W/2,10,"by Mad Sorcerer");
    wait_continue();
}

void player_setup()
{
    io->clear();
    io->setDefaultForeground(TCODColor::white);
    io->setAlignment(TCOD_LEFT);
    io->print(0,0,"A gray mist surrounds you, and you are overcome.  You awaken in a dark alley.");
    io->print(0,1,"Another time, another place.  A cold fog is in the air, no light to be seen");
    io->print(0,2,"except from a crack in a doorway at the bottom of some stairs leading to the");
    io->print(0,3,"basement of an old brick building.  You venture down and pull open the heavy");
    io->print(0,4,"oak door.");
    io->print(0,6,"A haggard old man with long grey hair stands behind a bar.  There are some");
    io->print(0,7,"tables and chairs, and a fire burns in a hearth.  The room is otherwise");
    io->print(0,8,"empty.  The old man looks up.  \"Welcome to the Adventurer's Guild!  Pull");
    io->print(0,9,"up a chair, weary traveller.  I will draw you a pint.\"");
    io->print(0,11,"You are drawn towards the bar, and sit down.  The barkeeper hands you a");
    io->print(0,12,"pint of ale.  \"What is your name?\", he asks.");
    io->setDefaultForeground(TCODColor::cyan);
    io->print(0,14,"Name your character: ");
    io->setDefaultForeground(TCODColor::white);
    player.name = readline(21,14,24);
    if (player.name.empty()) {
        player.name = "Rogue";
    }

    io->clear();
    io->print(0,0,string("\"Welcome " + player.name + "!\", exclaims the barkeeper.").c_str());
    io->print(0,2,"\"37 long years ago, I journeyed into the deepest recesses of the fabled");
    io->print(0,3,"Dungeons of Doom.  I brought back with me the Amulet of Yendor, and barely");
    io->print(0,4,"escaped with my life.  After I recovered, I opened this Guild.  For many");
    io->print(0,5,"exciting years, the guild was the hub of activity for all the young");
    io->print(0,6,"adventurers.  But times have changed, and as you can see, the chairs now sit");
    io->print(0,7,"empty.  The gear I once proudly stocked was stolen one night, and along with");
    io->print(0,8,"it, my prized amulet!\"");
    io->print(0,10,"\"Rumor has it the thieves took everything back to the dungeons.  Recover the");
    io->print(0,11,"amulet, and the Guild is yours to restore.  I have not much left to give you");
    io->print(0,12,"for your quest, save a single Scroll of Identify and a lantern to light your");
    io->print(0,13,"way.\"");
    io->print(0,15,"Refreshed from the beer, the bartender hands you the scroll, and using the");
    io->print(0,16,"lantern leads you through the dark alleys to the grounds of an old convent.");
    io->print(0,17,"Together you pull open a barricaded stone door, and he bids you farewell as");
    io->print(0,18,"you take the lantern and descend the stairs leading you back to the forbidden");
    io->print(0,19,"Dungeons of Doom.  As you descend the last step, you hear a shriek from above");
    io->print(0,20,"and the sounds of the stone door closing amidst a sinister laugh.");
    wait_continue();
}

void game_stats()
{
    io->print(SCREEN_W/2,16,string("Final Gold: " + int_to_str(player.gold)).c_str());
    io->print(SCREEN_W/2,18,string("Final Experience: " + int_to_str(player.xp)).c_str());
    io->print(SCREEN_W/2,20,string("Turns Played: " + int_to_str(turn)).c_str());
}

void game_over()
{
    io->clear();
    io->setDefaultForeground(TCODColor::white);
    io->setAlignment(TCOD_CENTER);
    io->print(SCREEN_W/2,6,"GAME OVER");
    io->print(SCREEN_W/2,10,string("Here lies " + player.name + ", a level " + int_to_str(player.clev) + " adventurer").c_str());
    io->print(SCREEN_W/2,12,string("Killed by " + cause_of_death + " on level " + int_to_str(dlev) + " of the Dungeons of Doom").c_str());
    game_stats();
    wait_continue();
}

void end_game()
{
    io->clear();
    io->setDefaultForeground(TCODColor::white);
    io->setAlignment(TCOD_CENTER);
    io->print(SCREEN_W/2,6,"CONGRATULATIONS!!!!");
    io->print(SCREEN_W/2,10,string(player.name + " returns to the surface with the Amulet of Yendor").c_str());
    game_stats();
    wait_continue();
    exit_game = true;
}

bool confirm_quit()
{
    io->setDefaultForeground(TCODColor::white);
    io->setAlignment(TCOD_LEFT);
    io->print(0,0,"quit game - are you sure (y/N)?");
    io->flush();
    TCOD_key_t key;
    TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS,&key,NULL,true);
    return (key.vk == TCODK_CHAR && key.c == 'y');
}

int show_inventory(const string& prompt)
{
    io->clear();
    io->setDefaultForeground(TCODColor::white);
    io->setAlignment(TCOD_LEFT);
    io->print(0,0,prompt.c_str());

    int ln=0;
    while (ln < player.inv.size()) {
        item_base *item = player.inv[ln];
        string status;
        if (item == player.worn_armor) status = "[wearing]";
        if (item == player.wielding) status = "[wielding]";
        io->print(0,ln+2,"%c) %s %s",'a'+ln,item->name().c_str(),status.c_str());
        ln++;
    }
    if (ln == 0) {
        notes::add("nothing to show");
        return -1;
    }
    TCOD_key_t key;
    while (!io->isWindowClosed()) {
        io->flush();
        TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS,&key,NULL,true);
        if (key.vk == TCODK_CHAR) {
            if (key.c >= 'a' && key.c < 'a'+ln) {
                return key.c - 'a';
            }
        } else if (key.vk == TCODK_ESCAPE || key.vk == TCODK_SPACE) {
            return -1;
        }
    }
    return -1;
}

void render_map()
{
    do {
        io->clear();
        coords p(MAP_W, MAP_H);
        for (p.y=0; p.y < p.h; p.y++) {
            for (p.x=0; p.x < p.w; p.x++) {
                const map_cell& cell = current_level->cell(p);
                if (cell.have_seen()) {
                    int ch = cell.ch;
                    TCODColor col = cell.col;
                    if (cell.item != nullptr) {
                        ch = cell.item->map_char();
                        col = cell.item->map_col();
                    }
                    if (!cell.is_visible()) {
                        col = col * 0.5;
                    }
                    io->setCharForeground(p.x,p.y+1,col);
                    io->setChar(p.x,p.y+1,ch);
                }
            }
        }
        io->setCharForeground(player.pos.x,player.pos.y+1,TCODColor::yellow);
        io->setChar(player.pos.x,player.pos.y+1,1);
        for (auto i = creatures.begin(); i != creatures.end(); ++i) {
            creature *c = *i;
            if (c->visible()) {
                io->setCharForeground(c->pos.x,c->pos.y+1,c->def->col);
                io->setChar(c->pos.x,c->pos.y+1,c->def->ch);
            }
        }
        io->setDefaultForeground(TCODColor::yellow);
        io->print(0,SCREEN_H-1,string("Level:" + int_to_str(dlev)).c_str());
        io->print(11,SCREEN_H-1,string("Hits:" + int_to_str(player.hp) + "(" + int_to_str(player.max_hp) + ")").c_str());
        io->print(25,SCREEN_H-1,string("Str:" + int_to_str(player.strength) + "(" + int_to_str(player.max_strength) + ")").c_str());
        io->print(39,SCREEN_H-1,string("Gold:" + int_to_str(player.gold)).c_str());
        io->print(51,SCREEN_H-1,string("Armor:" + int_to_str(player.ac)).c_str());
        io->print(61,SCREEN_H-1,string("Exp:" + int_to_str(player.clev) + "/" + int_to_str(player.xp)).c_str());
        if (!notes::empty()) {
            const string msg = notes::next();
            io->setDefaultForeground(TCODColor::white);
            io->print(0,0,msg.c_str());
            if (!notes::empty()) {
                io->setDefaultForeground(TCODColor::black);
                io->setDefaultBackground(TCODColor::white);
                io->printEx(msg.length()+1,0,TCOD_BKGND_SET,TCOD_LEFT,"MORE");
                io->setDefaultBackground(TCODColor::black);
                io->flush();
                while (!io->isWindowClosed()) {
                    TCOD_key_t key;
                    TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS,&key,NULL,true);
                    if (key.vk == TCODK_SPACE || key.vk == TCODK_ENTER) break;
                }
            }
        }
    }
    while (!notes::empty() && !io->isWindowClosed());
}

void attack_player(creature *c)
{
    const bool hit = attack(player.ac, 0);
    if (hit) {
        const int d = compute_damage(c->def->damage,0);
        player.hp -= d;
        notes::add("the " + c->def->name + " hits you");
        if (player.hp <= 0) {
            cause_of_death = "a " + c->def->name;
        }
    } else {
        notes::add("the " + c->def->name + " attacks you and misses");
    }
}

void try_attack(creature *c)
{
    const int target_ac = c->get_ac();
    const bool hit = attack(target_ac, 0);
    if (hit) {
        dice damage_roll("1d2");
        if (player.wielding != nullptr) damage_roll = player.wielding->get_damage();
        const int d = compute_damage(damage_roll,0);
        c->take_damage(d);
        if (c->is_dead()) {
            notes::add("you defeat the " + c->def->name);
            player.grant_xp(c->xp_value());
            for (auto i = creatures.begin(); i != creatures.end(); ++i) {
                if (*i == c) {
                    creatures.erase(i);
                    delete c;
                    break;
                }
            }
        } else {
            notes::add("you hit the " + c->def->name);
        }
    } else {
        notes::add("you miss the " + c->def->name);
    }
    turn++;
}

void update_visibility()
{
    // clear visibility flags across map and handle lit room
    const int room_num = current_level->cell(player.pos).room;
    for (int x=0; x < MAP_W; x++) {
        for (int y=0; y < MAP_H; y++) {
            const coords p(MAP_W,MAP_H,x,y);
            map_cell& cell = current_level->cell_ref(p);
            cell.clear_visible();
            if (room_num >= 0 && current_level->room(room_num).lit && cell.room == room_num) {
                cell.set_seen();
                cell.set_visible();
            }
        }
    }
    // handle lantern view
    for (int x=player.pos.x-1; x <= player.pos.x+1; x++) {
        for (int y=player.pos.y-1; y <= player.pos.y+1; y++) {
            const coords p(MAP_W,MAP_H,x,y);
            map_cell& cell = current_level->cell_ref(p);
            cell.set_seen();
            cell.set_visible();
        }
    }
    // update creature visibility based on map visibility
    for (auto i = creatures.begin(); i != creatures.end(); i++) {
        creature *c = *i;
        c->in_view = current_level->cell(c->pos).is_visible();
    }

}

bool place_item(item_base *item, const map_room& r)
{
    coords c = current_level->make_coords();
    int tries = 0;
    do {
        c = current_level->room_random(r);
        if (tries++ > 10) {  // failed to place
            delete item;
            return false;
        }
    } while (current_level->cell(c).is_exit() ||
             current_level->get_entry_point() == c ||
             current_level->cell(c).item != nullptr);
    current_level->cell_ref(c).item = item;
    return true;
}

void add_treasure()
{
    if (dlev >= AMULET_LEVEL) {
        bool placed = false;
        while (placed == false) {
            const int amulet_room = randint(0,current_level->num_rooms()-1);
            const map_room& ar = current_level->room(amulet_room);
            placed = place_item(new amulet(), ar);
        }
    }
    for (int i=0; i < current_level->num_rooms(); i++) {
        const map_room& r = current_level->room(i);
        switch(randint(0,3)) {
            case 0: // nothing
                break;
            case 1: // gold coins
                place_item(new gold_coins(randint(1,dlev*5)), r);
                break;
            case 2: // armor
                place_item(make_armor_for_level(dlev), r);
                break;
            case 3: // weapon
                place_item(make_weapon_for_level(dlev), r);
                break;
        }
    }
}

void new_level(const int level_num)
{
    delete current_level;
    for (auto i = creatures.begin(); i != creatures.end(); ++i) {
        delete *i;
    }
    creatures.clear();
    dlev = level_num;
    current_level = new level_map(MAP_W,MAP_H,level_num);
    current_level->generate_classic();
    player.pos = current_level->get_entry_point();
    add_treasure();
    populate_level(current_level);
    update_visibility();
}

void use_item()
{
    const int sel = show_inventory("select item to use or to stop using");
    if (sel < 0) return;
    item_base *item = player.inv[sel];
    item->use();
}

void drop_item()
{
    if (current_level->cell(player.pos).item != nullptr) {
        notes::add("this spot is already occupied");
        return;
    }    
    const int sel = show_inventory("select item to drop");
    if (sel < 0) return;
    item_base *item = player.inv[sel];
    if (item->try_drop()) {
        player.inv.erase(player.inv.begin() + sel);
        current_level->cell_ref(player.pos).item = item;
    }    
}

void pickup_item()
{
    item_base *item = current_level->cell(player.pos).item;
    if (item == nullptr) return;
    if (item->category() == GOLD) {
        current_level->cell_ref(player.pos).item = nullptr;
        gold_coins *coins = static_cast<gold_coins*>(item);
        notes::add("you find " + int_to_str(coins->count()) + " gold coins");
        player.gold += coins->count();
        delete coins;
    } else {
        if (player.inv.size() >= 20) {
            notes::add("pack is full");
        } else {
            current_level->cell_ref(player.pos).item = nullptr;
            player.inv.push_back(item);
            notes::add("you pick up the " + item->name());
            item->pickup();
        }
    }
}

void try_move(const coords& target)
{
    if (player.pos == target) return; // moving off edge of map
    if (!current_level->cell(target).is_walkable()) return;
    if (current_level->cell(target).is_exit()) {
        const int next_lev = dlev + (player.ascending ? -1 : 1);
        if (next_lev == 0) {
            end_game();
            return;
        }
        new_level(next_lev);
        string dir = "descend";
        if (player.ascending) dir = "ascend";
        notes::add("you " + dir + " to level " + int_to_str(dlev));
        return;
    }
    for (auto i = creatures.begin(); i != creatures.end(); ++i) {
        creature *c = *i;
        if (c->pos == target) {
            try_attack(c);
            return;
        }
    }
    player.pos = target;
    pickup_item();
    update_visibility();  // needed for monster AI
    turn++;
}

void process_turn()
{
    player.process_turn();
    for (auto i = creatures.begin(); i != creatures.end(); ++i) {
        creature *c = *i;
        if (c->move()) {
            attack_player(c);
        }
    }
}

int main(int argc, char *argv[])
{
    TCODConsole::initRoot(SCREEN_W,SCREEN_H,"Return to the Dungeons of Doom",true);
    io = TCODConsole::root;

    show_title();
    player_setup();
    new_level(1);
    turn = 0;
    exit_game = false;

    while (!io->isWindowClosed() && !exit_game && player.hp > 0) {
        io->clear();
        update_visibility();
        render_map();
        io->flush();
        TCOD_key_t key;
        const int last_turn = turn;
        TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS,&key,NULL,true);
        switch(key.vk) {
            case TCODK_UP : try_move(player.pos.up()); break;
            case TCODK_DOWN : try_move(player.pos.down()); break;
            case TCODK_LEFT : try_move(player.pos.left()); break;
            case TCODK_RIGHT : try_move(player.pos.right()); break;
            case TCODK_CHAR : {
                switch(key.c) {
                    case '2': try_move(player.pos.up()); break;
                    case '4': try_move(player.pos.left()); break;
                    case '6': try_move(player.pos.right()); break;
                    case '8': try_move(player.pos.down()); break; 
                    case '5':
                    case '.': turn++; break;
                    case 'i': show_inventory("player inventory"); break;
                    case 'd': drop_item(); break;
                    case 'u': use_item(); break;
                    case 'h': if (key.lctrl) { player.hp = player.max_hp = 999; } break;  // CHEAT
                    case 'n': if (key.lctrl) { new_level(player.ascending?--dlev:++dlev); } break;  // CHEAT
                }
                break;
            }
            case TCODK_ESCAPE : if (confirm_quit()) { exit_game = true; } break;
            case TCODK_ENTER : if (key.lalt || key.ralt) { io->setFullscreen(!io->isFullscreen()); } break;
            default:break;
        }
        if (last_turn < turn) {
            process_turn();
        }
    }

    if (!io->isWindowClosed() && !exit_game) {
        game_over();
    }
}
