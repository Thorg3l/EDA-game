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
    
    
    //Auxiliar methods
    
    void move_warriors() {
        if (round()% 4 != me()) return;
        IDS w_ids = warriors(me());
        for (int id : w_ids) {
            Pos p = unit(id).pos;
            if (pos_ok(p)) command(id, Dir(3));
        }
    }

    /**
    * Play method, invoked once per each round.
    */
    
    virtual void play () {
        move_warriors();
    }

};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
