#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
using namespace std;

enum Weapon_type { sword, arror, bomb };
enum Warrior_votation { dragon, ninja, iceman, lion, wolf };
enum Color { red, blue, none };

int M, N, R, K, T;
int init_element[5], init_force[5];
int HOUR = 0;
inline void show_time(int min);

class Weapon {
    Weapon_type _type;

    public:
    Weapon(Weapon_type t): _type(t) {}
    virtual ~Weapon() {}
    inline Weapon_type type() const { return _type; }
};

class Sword : public Weapon {
    int _atk;

    public:
    Sword(int force): Weapon(sword), _atk(force/5) {}
    inline int atk() const { return _atk; }

    void dull();
};

class Arror : public Weapon {
    int _atk;
    int _dur;

    public:
    Arror(): Weapon(arror), _atk(R), _dur(3) {}
    inline int atk() const { return _atk; }
    inline int dur() const { return _dur; }

    void dull();
};

class Bomb : public Weapon {
    public:
    Bomb(): Weapon(bomb) {}
};


class City {
    int _index;
    int _element;
    Color _flag;
    Warrior* _red_warrior;
    Warrior* _blue_warrior;
    Color _last_winner;

    public:
    Warrior* _update_red = nullptr;
    Warrior* _update_blue = nullptr;
    City(int idx, int ele=0, Color clr=none): _index(idx), _element(ele), 
        _flag(clr), _red_warrior(nullptr), _blue_warrior(nullptr) {}
    virtual ~City() {}
    inline int index() const { return _index; }
    inline int element() const { return _element; }
    inline Color flag() const { return _flag; }
    inline Warrior* red_warrior() { return _red_warrior; }
    inline Warrior* blue_warrior() { return _blue_warrior; }

    void generate_element();
    int give_element();
    void record_winner(Color winner);
    void change_flag(Color color);
    void update();
};

class Headquarter : public City {
    int _enermy_arrived;
    int _warriors_generated;
    Warrior_votation _generate_order[5];
    int _order_num;

    public:
    Headquarter(Color clr);
    inline int enermy_arrived() const { return _enermy_arrived; }

    void generate_warrior();
    bool occupied_check();
    void receive_element();
    void send_element();
    void enermy_arrive(); //check occupied
};


class Warrior {
    Color _color;
    Warrior_votation _vocation;
    int _index;
    int _force;
    int _element;
    Sword* ps;
    Arror* pa;
    Bomb* pb;
    int _position;

    public:
    Warrior(Color clr, Warrior_votation vct, int idx);
    virtual ~Warrior() {}
    inline Color color() const { return _color; }
    inline Warrior_votation vocation() const { return _vocation; }
    inline int index() const { return _index; }
    inline int force() const { return _force; }
    inline int element() const { return _element; }
    inline int position() const { return _position; }
    virtual void runaway() {}

    void hurt(int val);
    virtual void fight(Warrior* enermy);
    void arror_attack(Warrior* enermy);
    void bomb_attack(Warrior* enermy); //check will_killed
    virtual void fight_back(Warrior* enermy);
    void submit_element();
    void award_element();
    virtual void forward();
    bool will_killed(Warrior* enermy);
};

class Dragon : public Warrior {
    double _morale;

    public:
    Dragon(Color clr, int idx, int ele_rest): Warrior(clr, dragon, idx), 
        _morale(double(ele_rest)/init_element[dragon]) {}
    inline double morale() const { return _morale; }

    virtual void fight(Warrior* enermy); //vary morale, yell
};

class Ninja : public Warrior {
    public:
    Ninja(Color clr, int idx): Warrior(clr, ninja, idx) {}

    virtual void fight_back(Warrior* enermy); //no fight_back
};

class Iceman : public Warrior {
    int _steps;

    public:
    Iceman(Color clr, int idx): Warrior(clr, iceman, idx), _steps(0) {}

    virtual void forward(); //decrease element, increase force
};

class Lion : public Warrior {
    int _loyalty;

    public:
    Lion(Color clr, int idx, int ele_rest): Warrior(clr, lion, idx),
        _loyalty(ele_rest) {}
    inline int loyalty() const { return _loyalty; }
    
    virtual void fight(Warrior* enermy); //vary loyalty, lose element
    virtual void runaway();
};

class Wolf : public Warrior {
    public:
    Wolf(Color clr, int idx): Warrior(clr, wolf, idx) {}

    virtual void fight(Warrior* enermy); //rob weapon
};


Headquarter* headquarter[2];
City* city[30];
void initialize_world();
// :00
void headquarter_generate_warrior();
// :05
void lions_runaway();
// :10
void warrior_move_forward();
bool headquarter_occupied();
// :20
void city_generate_element();
// :30
void warrior_submit_element();
// :35
void warrior_arrow_attack();
// :38
void warrior_bomb_attack();
// :40
void warrior_fight();
// :50
void headquarter_report();
// :55
void warrior_report();


int main() {
    cin >> M >> N >> R >> K >> T;
    for (int i=0; i<5; ++i) cin >> init_element[i];
    for (int i=0; i<5; ++i) cin >> init_force[i];
    initialize_world();
    while (true) {
        if (HOUR*60 > T) break;
        headquarter_generate_warrior();
        if (HOUR*60+5 > T) break;
        lions_runaway();
        if (HOUR*60+10 > T) break;
        warrior_move_forward();
        if (headquarter_occupied()) break;
        if (HOUR*60+20 > T) break;
        city_generate_element();
        if (HOUR*60+30 > T) break;
        warrior_submit_element();
        if (HOUR*60+35 > T) break;
        warrior_arrow_attack();
        if (HOUR*60+38 > T) break;
        warrior_arrow_attack();
        if (HOUR*60+40 > T) break;
        warrior_fight();
        if (HOUR*60+50 > T) break;
        headquarter_report();
        if (HOUR*60+55 > T) break;
        warrior_report();
        ++HOUR;
    }
    return 0;
}




Warrior::Warrior(Color clr, Warrior_votation vct, int idx): 
    _color(clr), _vocation(vct), _index(idx), _force(init_force[vct]),
    _element(init_element[vct]) {
        ps = nullptr, pa = nullptr, pb = nullptr;
        if (vct == dragon or vct == ninja or vct == iceman) {
            if (idx % 3 == 0) ps = new Sword(init_force[vct]);
            else if (idx % 3 == 1) pa = new Arror;
            else pb = new Bomb;
        }
        if (vct == ninja) {
            if ((idx + 1) % 3 == 0) ps = new Sword(init_force[vct]);
            else if ((idx + 1) % 3 == 1) pa = new Arror;
            else pb = new Bomb;
        }

        _position = (clr == red ? 0 : N+1);
}

Headquarter::Headquarter(Color clr): City(clr==red ? 1 : N+1, M, clr), 
    _enermy_arrived(0), _warriors_generated(0), _order_num(0) {
        if (clr == red) 
            _generate_order[0] = iceman, 
            _generate_order[1] = lion, 
            _generate_order[2] = wolf, 
            _generate_order[3] = ninja, 
            _generate_order[4] = dragon;
        else 
            _generate_order[0] = lion, 
            _generate_order[1] = dragon, 
            _generate_order[2] = ninja, 
            _generate_order[3] = iceman, 
            _generate_order[4] = wolf;
}

void initialize_world() {
    for (int i=0; i<1; ++i) headquarter[i] = new Headquarter(Color(i));
    city[0] = headquarter[red], city[N+1] = headquarter[blue];
    for (int i=1; i<=N; ++i) city[i] = new City(i);
}
void headquarter_generate_warrior() {
    headquarter[red]->generate_warrior();
    headquarter[blue]->generate_warrior();
}
void lions_runaway() {
    for (int i=0; i<=N+1; ++i) {
        if (i < N+1) {
            auto tmp = city[i]->red_warrior();
            if (tmp != nullptr and tmp->vocation() == lion) tmp->runaway();
        }
        if (i > 0) {
            auto tmp = city[i]->blue_warrior();
            if (tmp != nullptr and tmp->vocation() == lion) tmp->runaway();
        }
    }
}
void warrior_move_forward() {
    for (int i=0; i<=N+1; ++i) {
        if (i > 0) {
            auto tmp = city[i-1]->red_warrior();
            if (tmp != nullptr) 
                tmp->forward(), city[i]->_update_red = tmp;
        }
        if (i < N+1) {
            auto tmp = city[i+1]->blue_warrior();
            if (tmp != nullptr)
                tmp->forward(), city[i]->_update_blue = tmp;
        }
    }
    for (int i=0; i<=N+1; ++i) city[i]->update();
    if (headquarter[red]->blue_warrior() != nullptr) 
        headquarter[red]->enermy_arrive();
    if (headquarter[blue]->red_warrior() != nullptr) 
        headquarter[blue]->enermy_arrive();
}
bool headquarter_occupied() {
    if (headquarter[red]->occupied_check() or 
    headquarter[blue]->occupied_check()) return true;
    return false;
}
void city_generate_element() {
    for (int i=0; i<=N+1; ++i) city[i]->generate_element();
}
void warrior_submit_element() {
    for (int i=1; i<=N; ++i) {
        auto tmp1 = city[i]->red_warrior(), tmp2 = city[i]->blue_warrior();
        if (tmp1 != nullptr and tmp2 == nullptr) tmp1->submit_element();
        else if (tmp1 == nullptr and tmp2 != nullptr) tmp2->submit_element();
    }
}
void warrior_arrow_attack() {
    for (int i=0; i<=N+1; ++i) {
        if (i < N and city[i]->red_warrior() != nullptr 
            and city[i+1]->blue_warrior() != nullptr)
            city[i]->red_warrior()->arror_attack(city[i+1]->blue_warrior());
        if (i > 1 and city[i]->blue_warrior() != nullptr
            and city[i-1]->red_warrior() != nullptr)
            city[i]->blue_warrior()->arror_attack(city[i-1]->red_warrior());
    }
}
void warrior_bomb_attack() {
    for (int i=1; i<=N; ++i) {
        auto tmp1 = city[i]->red_warrior(), tmp2 = city[i]->blue_warrior();
        if (tmp1 != nullptr and tmp2 != nullptr) {
            tmp1->bomb_attack(tmp2);
            tmp2->bomb_attack(tmp1);
        }
    }
}
