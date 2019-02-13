#include "Player.hh"
/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME Bot_3

struct PLAYER_NAME : public Player {

    static Player* factory () {
        return new PLAYER_NAME;
    }

    typedef vector<int> IDS;
    vector<vector<Pos >> city_pos;
    vector<vector<Cell>> mapa;
    vector<int> actions; //Aigua, gasolina, lluitar, ciutat
    //Misc
    int distance(Pos p1, Pos p2) {
        return sqrt(p2.i*p2.i-2*p2.i*p1.i+p1.i*p1.i+p2.j*p2.j-2*p2.j*p1.j+p1.j*p1.j);
    }

    bool check_unit(int id_enemy, int id, bool city, bool car) {
        if (id_enemy == -1) return true;
        Unit enemy = unit(id_enemy);
        Unit ally = unit(id);
        if (enemy.player != me() and enemy.type != Car) {
            if (car) return true;
            if (not city) return (enemy.food <= ally.food+6 and enemy.water <= ally.water+6);
            else return enemy.water <= ally.water;
            return true;
        }
        return false;
    }

    Pos search_city(Pos p) {
        int j = 0;
        Pos min;
        int min_dist = -1;
        while (min_dist == -1 and j < 8) {
          Pos c_pos = city_pos[j][random(0,city_pos.size())];
          if (cell(c_pos).owner != me()) {
            min_dist = distance(c_pos,p);
            min = c_pos;
          }
          ++j;
        }
        for (int i = j; i < 8; ++i) {
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

    //Inicialitació
    void init() {
        city_pos = cities();
    }

    //Accions a realitzar(guerrers)

    Pos first_step(const vector<vector<pair<int,Pos>>>& movements, Pos objective, const Pos& ally_pos) {
      Pos move = movements[objective.i][objective.j].second;
      while (move != ally_pos) {
        objective = move;
        move = movements[objective.i][objective.j].second;
      }
      return objective;

    }

    Pos warrior_bfs(const Unit& ally) {
        vector<vector<pair<int,Pos>>> movements(60, vector<pair<int,Pos>>(60,make_pair(-1,Pos(-1,-1))));
        queue<Pos> possible_position;
        possible_position.push(ally.pos);
        movements[ally.pos.i][ally.pos.j] = make_pair(0,ally.pos);
        bool dead = false;
        while (not possible_position.empty() and not dead) {
            Pos actual = possible_position.front();
            possible_position.pop();
            auto contents = movements[actual.i][actual.j];
            if (cell(actual).type == Water and ally.water - contents.first < 20) return first_step(movements, actual, ally.pos);
            else if (cell(actual).type == City and cell(actual).owner != me()) return first_step(movements, actual, ally.pos);
            if (contents.first >= ally.food or contents.first >= ally.water) dead = true;
            for (int i = 0; i < 8; ++i) {
                Pos next = actual+Dir(i);
                if (pos_ok(next) and movements[next.i][next.j].first == -1 and cell(next).type != Wall) {
                    movements[next.i][next.j] = make_pair(contents.first+1,actual);
                    possible_position.push(next);
                }
            }
        }
        return Pos(-1,-1);
    }

    bool surroundings(const Unit& warrior, const Pos& actual) {
        vector<Dir> actions(3,None);
        for (int i = 0; i < 8; ++i) {
            if (pos_ok(actual+Dir(i))) {
                Cell possible_objective = cell(actual+Dir(i));
                if (possible_objective.type == City) {
                    if (possible_objective.id != -1) {
                        if (check_unit(possible_objective.id, warrior.id, true, false)) {
                            if (actions[0] != None) {if (random(0,2)) actions[0] = Dir(i);}
                            else actions[0] = Dir(i);
                        }
                    }
                    else if (possible_objective.owner != me()) {
                      if (actions[2] != None) {if (random(0,2)) actions[2] = Dir(i);}
                      else actions[2] = Dir(i);
                    }
                }
                else if (possible_objective.id != -1 and check_unit(possible_objective.id, warrior.id, false, false)) {
                  if (actions[1] != None) {if (random(0,2)) actions[1] = Dir(i);}
                  else actions[1] = Dir(i);
                }
            }
        }
        for (int i = 0; i < 3; ++i) {
          if (actions[i] != None) {
                command(warrior.id, actions[i]);
                return true;
            }
        }
        return false;
    }

    void search_warriors(int id) {
        Unit warrior = unit(id);
        if (surroundings(warrior, warrior.pos)) return;
        Pos obj = warrior_bfs(warrior);
        if (pos_ok(obj)) {
          for (int i = 0; i < 8; ++i) {
            if (warrior.pos+Dir(i) == obj) {
              command(id, Dir(i));
              return;
            }
          }
        }
    }

    //Gestió unitats
    void move_warriors() {
        if (round()% 4 != me()) return;
        IDS w_ids = warriors(me());
        for (int id : w_ids) search_warriors(id);
    }

    virtual void play () {
        if (round() == 0) init();
        //if (round() == 1) city_pos[9][0] = Pos(3,4);
        move_warriors();
        //move_cars();
    }
};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
