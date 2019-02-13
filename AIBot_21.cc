#include "Player.hh"
/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME Bot_21


struct PLAYER_NAME : public Player {
    
    static Player* factory () {
        return new PLAYER_NAME;
    }
    
    typedef vector<int> IDS;
    vector<vector<Pos >> city_pos;
    vector<vector<Cell>> mapa;
    //Misc
    int distance(Pos p1, Pos p2) {
        return sqrt(p2.i*p2.i-2*p2.i*p1.i+p1.i*p1.i+p2.j*p2.j-2*p2.j*p1.j+p1.j*p1.j);
    }
    
    bool check_unit(int id_enemy, int id, bool city) {
        if (id_enemy == -1) return true;
        Unit enemy = unit(id_enemy);
        Unit ally = unit(id);
        if (enemy.player != me() and enemy.type != Car) {
            if (not city) return enemy.food <= ally.food and enemy.water <= ally.water;
            else return true;
        }
        return false;
    }
    
    //Init
    void init() {
        city_pos = cities();
        mapa = vector<vector<Cell>>(60, vector<Cell>(60));
        for (int i = 0; i < 60; ++i) {
            for (int j = 0; j < 60; ++j) {
                mapa[i][j] = cell(i,j);
            }
        }
    }
    
    //Scaneig
    
    Dir scan(Pos p, int id) {
        Dir d_city = None;
        Dir combat = None;
        Dir empty = None;
        for (int i = 0; i < 8; ++i) {
            Cell aux = cell(p+Dir(i));
            if (aux.type == City and aux.id != me()) {
                if (check_unit(aux.id, id, true)) return Dir(i);
                d_city = Dir(i);
            }
            else if (aux.id == -1 and aux.type != Wall and aux.type != Water and aux.type != Station) {
                if (check_unit(aux.id, id, false)) combat = Dir(i);
                else empty = Dir(i);
            }
        }
        if (d_city != None) return d_city;
        else if (combat != None) return combat;
        return empty;
    }
    
    void go_to(Pos origin, Pos destiny, int id) {
        Dir d = None;
        if (origin.i < destiny.i) {
            if (origin.j < destiny.j) d = BR;
            else if (origin.j > destiny.j) d = LB;
            else d = Bottom;
        }
        else if (origin.i > destiny.i) {
            if (origin.j < destiny.j) d = RT;
            else if (origin.j > destiny.j) d = TL;
            else d = Top;
        }
        else {
            if (origin.j < destiny.j) d = Right;
            else if (origin.j > destiny.j) d = Left;
        }
        if (check_unit(cell(origin+d).id, id, false) and cell(origin+d).type != Wall and cell(origin+d).type != Water and 
            cell(origin+d).type != Station) command(id, d);
        else command(id, scan(origin, id));
    }  
    
    Pos search_city(Pos p) {
        Pos min = city_pos[0][random(0,city_pos.size())];
        int min_dist = distance(min,p);
        for (int i = 1; i < 8; ++i) {
            Pos c_pos = city_pos[i][random(0,city_pos.size()-1)];
            if (cell(c_pos).owner != me()) {
                int dist = distance(c_pos,p);
                if (dist < min_dist) {
                    min_dist = dist;
                    min = c_pos;
                }
            }
        }
        return min;
    }
    
    void move_warriors() {
        if (round()% 4 != me()) return;
        IDS w_ids = warriors(me());
        for (int id : w_ids) {
            Pos p = unit(id).pos;
            if (cell(p).type != City or cell(p).owner == me()) {
                Pos select = search_city(p);
                go_to(p,select,id);
            }
            else {
                Dir select = scan(p,id);
                command(id, select);
            }
        }
    }
    
    virtual void play () {
        if (round() == 0) init();
        move_warriors();
        //move_cars();
    }
};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
