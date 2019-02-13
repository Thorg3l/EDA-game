#include "Player.hh"
/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME Cthulhu_1

struct PLAYER_NAME : public Player {

    static Player* factory () {
        return new PLAYER_NAME;
    }

    typedef vector<int> IDS;
    typedef pair<int,Pos> Arc;
    set<Pos> occupied;
    vector<vector<pair<int,int>>> distances;
    //Misc

    void bfs(vector<vector<pair<int,int>>>& distances, vector<queue<Pos>>& all_water,vector<queue<Pos>>& all_cities) {
      int total = all_water.size();
      int size = total;
      while (total) {
        for (int i = 0; i < size; ++i) {
          if (not all_water[i].empty()) {
            Pos p = all_water[i].front();
            if (pos_ok(p)) {
              all_water[i].pop();
              for (int X = 0; X < 8; ++X) {
                Pos xx = p+Dir(X);
                if (pos_ok(xx)) {
                  Cell c = cell(xx);
                  if (c.type != Wall and c.type != Station and (distances[xx.i][xx.j].first == -1 or (distances[xx.i][xx.j].first > (distances[p.i][p.j].first)+1))) {
                    distances[xx.i][xx.j].first = distances[p.i][p.j].first + 1;
                    all_water[i].push(xx);
                  }
                }
              }
            }
          }
          else {
            --total;
            all_water[i].push(Pos(-1,-1));
          }
        }
      }
      total = all_cities.size();
      size = total;
      while (total) {
        for (int i = 0; i < size; ++i) {
          if (not all_cities[i].empty()) {
            Pos p = all_cities[i].front();
            if (pos_ok(p)) {
              all_cities[i].pop();
              for (int X = 0; X < 8; ++X) {
                Pos xx = p+Dir(X);
                if (pos_ok(xx)) {
                  Cell c = cell(xx);
                  if (c.type != Wall and c.type != Station and c.type != Water and (distances[xx.i][xx.j].second == -1 or (distances[xx.i][xx.j].second > (distances[p.i][p.j].second)+1))) {
                    distances[xx.i][xx.j].second = distances[p.i][p.j].second + 1;
                    all_cities[i].push(xx);
                  }
                }
              }
            }
          }
          else {
            --total;
            all_cities[i].push(Pos(-1,-1));
          }
        }
      }
    }

    void search(vector<queue<Pos>>& all_water, vector<queue<Pos>>& all_cities) {
      for (int i = 0; i < 60; ++i) {
        for (int j = 0; j < 60; ++j) {
          if (cell(i,j).type == Water) {
            queue<Pos> q;
            q.push(Pos(i,j));
            all_water.push_back(q);
          }
          else if (cell(i,j).type == City) {
            queue<Pos> q;
            q.push(Pos(i,j));
            all_cities.push_back(q);
          }
        }
      }
    }

    void init() {
      distances = vector<vector<pair<int,int>>>(60, vector<pair<int,int>>(60,make_pair(-1,-1)));
      vector<queue<Pos>> all_water;
      vector<queue<Pos>> all_cities;
      search(all_water, all_cities);
      int n = all_water.size();
      for (int i = 0; i < n; ++i) {
        int x = all_water[i].front().i;
        int y = all_water[i].front().j;
        distances[x][y].first = 0;
      }
      n = all_cities.size();
      for (int i = 0; i < n; ++i) {
        int x = all_cities[i].front().i;
        int y = all_cities[i].front().j;
        distances[x][y].second = 0;
      }
      bfs(distances,all_water,all_cities);
      /*for (int i = 0; i < 60; ++i) {
        for (int j = 0; j < 60; ++j) {
          if (distances[i][j].second >= 10 or distances[i][j].second < 0) cerr << distances[i][j].second << ' ';
          else cerr << distances[i][j].second << "  ";
        }
        cerr << endl;
      }*/
    }

    bool scan(const Pos& p, const Unit& ally) {
        for (int i = 0; i < 8; ++i) {
            Pos aux = p + Dir(i);
            if (pos_ok(aux)) {
                Cell c = cell(aux);
                if (c.id != -1) {
                    Unit enemy = unit(c.id);
                    if (enemy.player != me()) {
                        if (enemy.type == Car) return false;
                        else if (ally.type != Car and enemy.food+3 >= ally.food-6 and enemy.water+3 >= ally.water-6)
                            return false;
                    }
                }
            }
        }
        return true;
    }

    bool check_unit(int id_enemy, int id, bool city, bool car) {
        if (id_enemy == -1) return true;
        Unit enemy = unit(id_enemy);
        Unit ally = unit(id);
        if (enemy.player != me() and enemy.type != Car) {
            if (car) return true;
            if (not city) return (enemy.food-6 <= ally.food+3 and enemy.water-6 <= ally.water+3);
            else return enemy.water <= ally.water;
            return true;
        }
        return false;
    }

    bool check_cell(Cell c, Pos p, const Unit& u) {
        if (c.type == Water or c.type == Station or c.type == Wall or occupied.find(p) != occupied.end()) return false;
        if (c.id != -1) {
            if (c.type == City) {
                if (u.type == Car) return false;
                else return (check_unit(c.id, u.id, true, false));
            }
            return check_unit(c.id, u.id, true, u.type == Car);
        }
        if (c.type == City and u.type == Car) return false;
        return true;
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
                if (u.pos+Dir(i) == obj and occupied.find(u.pos+Dir(i)) == occupied.end()) {
                command(u.id, Dir(i));
                occupied.insert(u.pos+Dir(i));
                return;
                }
            }
        }
        command(u.id, None);
        occupied.insert(u.pos);
    }

    bool surroundings(const Unit& u, const Pos& actual, bool warrior) {
        vector<Dir> actions(3,None);
        Pos p = actual;
        for (int i = 0; i < 8; ++i) {
            p = actual+Dir(i);
            if (pos_ok(p) and occupied.find(p) == occupied.end() and scan(p,u)) {
                if (warrior) {
                    Cell possible_objective = cell(p);
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
                    Cell possible_objective = cell(p);
                    if (possible_objective.type != City and possible_objective.id != -1) {
                        if (check_unit(possible_objective.id,u.id,false, true)) {
                            command(u.id, Dir(i));
                            occupied.insert(p+Dir(i));
                            return true;
                        }
                    }
                    return false;
                }
            }
        }
        for (int i = 0; i < 3; ++i) {
            if (actions[i] != None and occupied.find(p+actions[i]) == occupied.end()) {
                command(u.id, actions[i]);
                occupied.insert(p+actions[i]);
                return true;
            }
        }
        return false;
    }

    //Accions a realitzar(guerrers)

    void survive(const Unit& u, int min, Dir d, bool water) {
      Pos p = u.pos;
      if (check_cell(cell(p+d),p+d,u) and scan(p+d,u)) {
        occupied.insert(p+d);
        command(u.id,d);
        return;
      }
      Dir d_aux = None;
      bool none = (check_cell(cell(p+d_aux),p+d_aux,u) and scan(p+d_aux,u));
      Dir other_dir = None;
      for (int i = 0; i < 8; ++i) {
        d_aux = Dir(i);
        if (d_aux != d) {
          Pos aux = p+d_aux;
          if (pos_ok(aux) and ((not water and distances[aux.i][aux.j].second == min) or (water and distances[aux.i][aux.j].first == min) or not none) and check_cell(cell(aux),aux,u) and scan(aux,u)) {
            if (not none) other_dir = d_aux;
            else {
              occupied.insert(aux);
              command(u.id,d_aux);
              return;
            }
          }
        }
      }
      occupied.insert(p+other_dir);
      command(u.id,other_dir);
    }

    Pos warrior_bfs(const Unit& ally, int& status) {
        vector<vector<pair<int,Pos>>> movements(60, vector<pair<int,Pos>>(60,make_pair(-1,Pos(-1,-1))));
        queue<Pos> possible_position;
        possible_position.push(ally.pos);
        movements[ally.pos.i][ally.pos.j] = make_pair(0,ally.pos);
        bool dead = false;
        while (not possible_position.empty() and not dead) {
            Pos actual = possible_position.front();
            possible_position.pop();
            auto contents = movements[actual.i][actual.j];
            if (occupied.find(actual) == occupied.end()) {
                if (cell(actual).type == City and cell(actual).owner != me())
                    return first_step(movements, actual, ally.pos);
            }
            if (contents.first >= ally.food) {
              status += 2;
              dead = true;
            }
            if (contents.first >= ally.water) {
              ++status;
              dead = true;
            }
            for (int i = 0; i < 8; ++i) {
                Pos next = actual+Dir(i);
                if (pos_ok(next) and movements[next.i][next.j].first == -1 and (contents.first > 1 or scan(next,ally))) {
                    if (check_cell(cell(next),next,ally)) {
                        movements[next.i][next.j] = make_pair(contents.first+1,actual);
                        possible_position.push(next);
                    }
                }
            }
        }
        return Pos(-1,-1);
    }

    void search_warriors(int id) {
      Dir d_water = None;
      Dir d_food = None;
      int m_water = 3600;
      int m_food = m_water;
      Unit u = unit(id);
      Pos p = u.pos;
      if (surroundings(u, p, true)) return;
      for (int i = 0; i < 8; ++i) {
        Pos aux = p + Dir(i);
        if (pos_ok(aux)) {
          if (distances[aux.i][aux.j].first != -1  and distances[aux.i][aux.j].first < m_water) {
              m_water = distances[aux.i][aux.j].first;
              d_water = Dir(i);
          }
          if (distances[aux.i][aux.j].second != -1  and distances[aux.i][aux.j].second < m_food) {
              m_food = distances[aux.i][aux.j].second;
              d_food = Dir(i);
          }
        }
      }
      if (u.food-5 < m_food) {
        survive(u,m_food,d_food,false);
        return;
      }
      else if (u.water-5 < m_water) {
        survive(u,m_water,d_water,true);
        return;
      }
      //Warrior survives
      int status = 0;
      Pos obj = warrior_bfs(u,status);
      if (not status) go_to(obj, u);
      else if (status == 1) {
        survive(u,m_water,d_water,true);
        return;
      }
      else if (status == 2) {
        survive(u,m_food,d_food,false);
        return;
      }
      else command(u.id,None);
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
            if (cell(actual).type == Station and ally.food - contents.first < 40) return first_step(movements, actual, ally.pos);
            else if (cell(actual).type == City and contents.first > 8) return first_step(movements, actual, ally.pos);
            else if (cell(actual).id != -1 and check_unit(cell(actual).id, ally.id, false, true)) {
                return first_step(movements, actual, ally.pos);
            }
            if (cell(actual).type != Station and cell(actual).type != City) {
                for (int i = 0; i < 8; ++i) {
                    Pos next = actual+Dir(i);
                    if (pos_ok(next) and movements[next.i][next.j].first == -1 and (contents.first > 1 or scan(next,ally))) {
                        if (check_cell(cell(next),next,ally)) {
                            movements[next.i][next.j] = make_pair(contents.first+1,actual);
                            possible_position.push(next);
                        }
                    }
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
        auto v = random_permutation(w_ids.size());
        for (int i : v) {
            search_warriors(w_ids[i]);
        }
    }

    void move_cars() {
      IDS c_ids = cars(me());
      auto v = random_permutation(c_ids.size());
      for (int i : v) if (can_move(c_ids[i])) {
          search_cars(c_ids[i]);
      }
    }

    virtual void play () {
        if (round() == 0) init();
        occupied.clear();
        move_cars();
        move_warriors();

    }
};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
