#include "Player.hh"
/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME Bot_1


struct PLAYER_NAME : public Player {
    /**
    * Factory: returns a new instance of this class.
    * Do not modify this function.
    */
    static Player* factory () {
        return new PLAYER_NAME;
    }

    /**
    * Types and attributes for your player can be defined here.
    */
    typedef vector<int> IDS;
    vector<vector<Pos >> city_pos;
    
    //Auxiliar methods
    
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
        Pos min;
        int min_dist = -1;
        bool first = true;
        for (int i = 0; i < 8; ++i) {
            Pos c_pos = city_pos[i][0];
            if (cell(c_pos).owner != me()) {
                if (first) {
                    first = false;
                    min = c_pos;
                    min_dist = sqrt((c_pos.i-p.i)*(c_pos.i-p.i)-(c_pos.j-p.j)*(c_pos.j-p.j));
                }
                else {
                    int dist = sqrt((c_pos.i-p.i)*(c_pos.i-p.i)-(c_pos.j-p.j)*(c_pos.j-p.j));
                    if (dist < min_dist) {
                        min_dist = dist;
                        min = c_pos;
                    }
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
            if (cell(p).type == City and cell(p).owner != me()) command(id, None);
            else {
                Pos select = search_city(p);
                if (pos_ok(select)) go_to(p,select,id);
                else command(id, None);
            }
        }
    }

    /**
    * Play method, invoked once per each round.
    */
    
    virtual void play () {
        city_pos = cities();
        move_warriors();
    }

};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
