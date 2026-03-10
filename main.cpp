//
// Created by ilia on 10-Mar-26.
//

//инклуды
#include <iostream>
#include <vector>
#include <cmath>
#include <string>


//юзингы
using std::function;
using std::cout;
using std::endl;
using std::vector;
using std::sqrt;

//я не очень понял, зачем спайс, можно просто объявить контсанты
//ибо больше мы ничего не наследуем
const double G = 6.67430e-11;
const double Msun = 1.98847e30;

//структрура вектор состояния
struct State {
    double x, y, vx, vy, t;
    State(double x = 0, double y = 0, double vx = 0, double vy = 0, double t = 0)
        : x(x), y(y), vx(vx), vy(vy), t(t) {}
};


class Gravity_field {
private:
    //описывает точечную массу в пространстве
    struct Mass {
        double x, y, mass;
        Mass(double x, double y, double m) : x(x), y(y), mass(m) {} //при создании объекта структуры нужно задать ее полям параметры
    };
    vector<Mass> masses;

public:
    //обработка (добавление и отчиска) вектора с массами
    void add_mass(double x, double y, double m){
        masses.push_back(Mass(x,y, m));
    }

    void clear_mass() {
        masses.clear();
    }

    //перенес точку в паблик
    struct Point
    {
        double gx, gy; //компоненты ускорения по осям
        Point (double gx = 0, double gy = 0) : gx(gx), gy(gy) {} //задаем коррдинаты точки и ускорение по осям в этой точке
    };
    //пока оставим такой счет поля
    Point calculating_field(double x, double y) //вычисляем поле в точках x, y
    {
        double gy = 0,  gx = 0;
        for (const Mass& m : masses)
        {
            double rx = x - m.x;
            double ry = y - m.y;
            double r = sqrt(rx * rx + ry * ry);
            gx += G * m.mass * rx / (r*r*r);
            gy += G * m.mass * ry / (r * r * r);
        }
        return Point(gx, gy); //знак gx и gy говорит о направлении
    }
    //виртуальная функция, тут мы обновляем положение масс
    //и тут юзаем ссылку, чтобы работать с адресами космотел.
    //там мб с константами поиграться
    void pos_update(vector<Cosmic_bodies*>& bodies){}
};


class Cosmic_bodies {
protected:

    double mass;
    struct two_dec_vector {
        double x, y;
        two_dec_vector(double x_val = 0, double y_val = 0) : x(x_val), y(y_val) {}
    };
    two_dec_vector pos; //позиция
    two_dec_vector vel;   //и скорость
public:
    //два констуктора
    Cosmic_bodies(double m, double x0, double y0, double vx0, double vy0)
       : mass(m), pos(x0, y0), vel(vx0, vy0) {}
    //по умолчанию
    Cosmic_bodies() : mass(0), pos(0,0), vel(0,0) {}
    virtual ~Cosmic_bodies(){}

    //получаем
    double getMass() const { return mass; }
    double getX() const { return pos.x; }
    double getY() const { return pos.y; }
    double getVx() const { return vel.x; }
    double getVy() const { return vel.y; }
    //уставнавливаем

    //вот тут прописыааем физику, точнее виртуальный метод
    //вычисленя производных, который идет в интегратора
    virtual State derivatives(const State& s, double t, const Gravity_field& field) const = 0;
    //и метод обновления положения
    // считаем следущие значения с помощью рк45


    virtual void update(double dt, double t, const Gravity_field& field) {
        //мое начальное состояние
        State current(pos.x, pos.y, vel.x, vel.y, t);

        //k1 = f(x,y)
        State k1 = derivatives(current, t, field);
        //k2 = f(x+dt/2, y + k1*dt/2)
        State y2 = {
            current.x + dt/2 * k1.vx,
            current.y + dt/2 * k1.vy,
            current.vx + dt/2 * k1.x,
            current.vy + dt/2 * k1.y,
            t + dt/2
        };
        State k2 = derivatives(y2, t + dt/2, field);
        //k3 = f(x+dt/2, y + k2*dt/2)
        State y3 = {
            current.x + dt/2 * k2.vx,
            current.y + dt/2 * k2.vy,
            current.vx + dt/2 * k2.x,
            current.vy + dt/2 * k2.y,
            t + dt/2
        };
        State k3 = derivatives(y3, t + dt/2, field);
        //k2 = f(x+dt, y + k3*dt)
        State y4 = {
            current.x + dt * k3.vx,
            current.y + dt * k3.vy,
            current.vx + dt * k3.x,
            current.vy + dt * k3.y,
            t + dt
        };
        State k4 = derivatives(y4, t + dt, field);

        //суммируем с весами y = y_old + dx/6 (k1+2k2+2k3+k4)
        State next;
        next.x  = current.x  + (dt/6) * (k1.vx + 2*k2.vx + 2*k3.vx + k4.vx);
        next.y  = current.y  + (dt/6) * (k1.vy + 2*k2.vy + 2*k3.vy + k4.vy);
        next.vx = current.vx + (dt/6) * (k1.x  + 2*k2.x  + 2*k3.x  + k4.x);
        next.vy = current.vy + (dt/6) * (k1.y  + 2*k2.y  + 2*k3.y  + k4.y);
        next.t  = t + dt;
        //переприсваиваем
        pos.x = next.x;
        pos.y = next.y;
        vel.x = next.vx;
        vel.y = next.vy;

};


class Planet : public Cosmic_bodies {
    public:
        Planet(double m, double x, double y, double vx, double vy) : Cosmic_bodies(m, x, y, vx, vy) {}
        //вот она функция, физики
        //мы возвращем производные
        // производная координаты скорость
        // производная скорости ускорение (обрааюсь к твоему методу поска g)
        State derivatives(const State& s, double t, const Gravity_field& field) const override {
            Gravity_field::Point g = field.calculating_field(s.x, s.y);
            return {s.vx, s.vy, g.gx, g.gy, 1.0}; }

};

class Spacecraft : public Cosmic_bodies {
public:
    //констуктор
    Spacecraft(double m, double x, double y, double vx, double vy)
       : Cosmic_bodies(m, x, y, vx, vy) {}
    //метод с производными
    State derivatives(const State& s, double t, const Gravity_field& field) const override {
        Gravity_field::Point g = field.calculating_field(s.x, s.y);
        return {s.vx, s.vy, g.gx, g.gy, 1.0};
    }
    //обновление
    void update(double dt, double t, const Gravity_field& field) override {
        Cosmic_bodies::update(dt, t, field);
    }
};

//реализация метода грави поля
void Gravity_field::pos_update(const vector<Cosmic_bodies*>& bodies) {
    masses.clear();
    for (auto body : bodies) {
        masses.push_back(Mass(body->getX(), body->getY(), body->getMass()));
    }
}




