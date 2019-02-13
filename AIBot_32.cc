#include "Player.hh"
/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME Bot_32

struct PLAYER_NAME : public Player {

    static Player* factory () {
        return new PLAYER_NAME;
    }

    typedef vector<int> IDS;
    vector<vector<Pos >> city_pos;
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

    Pos first_step(const vector<vector<pair<int,Pos>>>& movements, Pos objective, const Pos& ally_pos) {
      Pos move = movements[objective.i][objective.j].second;
      while (move != ally_pos) {
        objective = move;
        move = movements[objective.i][objective.j].second;
      }
      return objective;
    }

    void go_to(Pos obj, Unit u) {
      if (pos_ok(obj)) {
        for (int i = 0; i < 8; ++i) {
          if (u.pos+Dir(i) == obj) {
            command(u.id, Dir(i));
            return;
          }
        }
      }
    command(u.id, None);
    }

    bool surroundings(const Unit& u, const Pos& actual, bool warrior) {
        vector<Dir> actions(3,None);
        for (int i = 0; i < 8; ++i) {
            if (pos_ok(actual+Dir(i))) {
                if (warrior) {
                  Cell possible_objective = cell(actual+Dir(i));
                  if (possible_objective.type == City) {
                    if (possible_objective.id != -1) {
                      if (check_unit(possible_objective.id, u.id, true, false)) {
                          if (actions[0] != None) {if (random(0,2)) actions[0] = Dir(i);}
                          else actions[0] = Dir(i);
                      }
                    }
                    else if (possible_objective.owner != me()) {
                      if (actions[2] != None) {if (random(0,2)) actions[2] = Dir(i);}
                      else actions[2] = Dir(i);
                    }
                  }
                  else if (possible_objective.id != -1 and check_unit(possible_objective.id, u.id, false, false)) {
                    if (actions[1] != None) {if (random(0,2)) actions[1] = Dir(i);}
                    else actions[1] = Dir(i);
                  }
                }
                else {
                    Cell possible_objective = cell(actual+Dir(i));
                    if (possible_objective.type != City and possible_objective.id != -1
                        and check_unit(possible_objective.id,u.id,false, true)) {
                      command(u.id, Dir(i));
                      return true;
                    }
                }
            }
        }
        for (int i = 0; i < 3; ++i) {
          if (actions[i] != None) {
                command(u.id, actions[i]);
                return true;
          }
        }
        return false;
    }

    //Inicialitació
    void init() {
        city_pos = cities();
    }

    //Accions a realitzar(guerrers)

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
            if (cell(actual).type == Water and ally.water - contents.first < 10) return first_step(movements, actual, ally.pos);
            if (cell(actual).type == City and cell(actual).owner != me()) return first_step(movements, actual, ally.pos);
            if (contents.first >= ally.food or contents.first >= ally.water) dead = true;
            for (int i = 0; cell(actual).type != Water and i < 8; ++i) {
                Pos next = actual+Dir(i);
                if (pos_ok(next) and movements[next.i][next.j].first == -1 and cell(next).type != Wall 
                    and cell(next).type != Station) {
                    movements[next.i][next.j] = make_pair(contents.first+1,actual);
                    possible_position.push(next);
                }
            }
        }
        return Pos(-1,-1);
    }

    void search_warriors(int id) {
        Unit warrior = unit(id);
        if (surroundings(warrior, warrior.pos, true)) return;
        Pos obj = warrior_bfs(warrior);
        go_to(obj, warrior);
    }

    //Accions a realitzar(cotxes)
    Pos car_bfs(const Unit& ally) {
        vector<vector<pair<int,Pos>>> movements(60, vector<pair<int,Pos>>(60,make_pair(-1,Pos(-1,-1))));
        queue<Pos> possible_position;
        possible_position.push(ally.pos);
        movements[ally.pos.i][ally.pos.j] = make_pair(0,ally.pos);
        while (not possible_position.empty()) {
            Pos actual = possible_position.front();
            possible_position.pop();
            auto contents = movements[actual.i][actual.j];
            if (cell(actual).type == Station and ally.food - contents.first < 20) return first_step(movements, actual, ally.pos);
            else if (cell(actual).id != -1 and check_unit(cell(actual).id, ally.id, false, true)) {
                return first_step(movements, actual, ally.pos);
            }
            for (int i = 0; cell(actual).type != Station and i < 8; ++i) {
                Pos next = actual+Dir(i);
                if (pos_ok(next) and movements[next.i][next.j].first == -1 and cell(next).type != Wall and cell(next).type != Water
                    and cell(next).type != City) {
                    movements[next.i][next.j] = make_pair(contents.first+1,actual);
                    possible_position.push(next);
                }
            }
        }
        return Pos(-1,-1);
    }

    void search_cars(int id) {
      Unit car = unit(id);
      if (surroundings(car, car.pos, false)) return;
      Pos obj = car_bfs(car);
      go_to(obj, car);
    }

    //Gestió unitats
    void move_warriors() {
        if (round()% 4 != me()) return;
        IDS w_ids = warriors(me());
        for (int id : w_ids) search_warriors(id);
    }

    void move_cars() {
      IDS c_ids = cars(me());
      for (int id : c_ids) if (can_move(id)) search_cars(id);
    }

    virtual void play () {
        if (round() == 0) init();
        //if (round() == 1) city_pos[9][0] = Pos(3,4);
        move_warriors();
        move_cars();
    }
};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
