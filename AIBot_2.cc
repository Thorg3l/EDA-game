#include "Player.hh"
/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME Bot_2


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
        if (cell(origin+d).type != Wall and cell(origin+d).type != Water and cell(origin+d).type != Station) command(id, d);
        else command(id, Dir((d+random(0,7))%7));
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
            Pos select = search_city(p);
            go_to(p,select,id);
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
