#include <iostream>
#include <vector>
#include <cmath>
#include <functional>
#include <string>
#include <fstream>
using std::function;
using std::cout;
using std::endl;
using std::vector;
using std::sqrt;
using std::string;
using std::ofstream;


const double Msun = 1.98847e30;
const double G = 6.67430e-11;


struct State //структрура вектор состояния тела
{
    double x, y, vx, vy, t;
    State(double x = 0, double y = 0, double vx = 0, double vy = 0, double t = 0)
        : x(x), y(y), vx(vx), vy(vy), t(t) {
    }
};

class Cosmic_bodies;

class Gravity_field
{
private:
    struct Mass //описывает точечную массу в пространстве
    {
        double x, y, mass;
        Mass(double x, double y, double m) : x(x), y(y), mass(m) {}
    };
    vector<Mass> masses;
public:
    void add_mass(double x, double y, double m)
    {
        masses.push_back(Mass(x, y, m));
    }
    void clear_mass()
    {
        masses.clear();
    }
    struct Point //описывает точку пространства
    {
        double gx, gy; //компоненты ускорения по осям
        Point(double gx = 0, double gy = 0) : gx(gx), gy(gy) {} //задаем коррдинаты точки и ускорение по осям в этой точке
    };
    Point calculating_field(double x, double y) const //вычисляем поле в точках x, y
    {
        double total_gy = 0;
        double total_gx = 0;
        for (const Mass& m : masses)
        {
            double rx = x - m.x;
            double ry = y - m.y;
            double r = sqrt(rx * rx + ry * ry);
            //проверка на 0
            if (r < 1e-10 ){
                total_gx += 0 ;
                total_gy += 0 ;
            }
            else {
                total_gx -= G * m.mass * rx / (r * r * r);
                total_gy -= G * m.mass * ry / (r * r * r);
            }
        }
        return Point(total_gx, total_gy); //знак gx и gy говорит о направлении
    }
    void pos_update(const vector<Cosmic_bodies*>& bodies);
};


class Cosmic_bodies
{
protected:
    double mass;
    struct two_dec_vector
    {
        double x, y;
        two_dec_vector(double x_val = 0, double y_val = 0) : x(x_val), y(y_val) {}
    };
    two_dec_vector pos; //координаты x и y
    two_dec_vector vel;   //скорость по осям
public:
    Cosmic_bodies(double m, double x0, double y0, double vx0, double vy0)
        : mass(m), pos(x0, y0), vel(vx0, vy0) {
    }
    virtual ~Cosmic_bodies() {}
    double getMass() const { return mass; }
    double getX() const { return pos.x; }
    double getY() const { return pos.y; }
    double getVx() const { return vel.x; }
    double getVy() const { return vel.y; }
    //метод вычисления производных, который идет в интегратор
    State derivatives(const State& s, double t, const Gravity_field& field) const
    {
        Gravity_field::Point g = field.calculating_field(s.x, s.y);
        return State(s.vx, s.vy, g.gx, g.gy, 1.0);
    }
    //метод обновления положения. Считаем следущие значения с помощью рк45
    virtual void update(double dt, double t, const Gravity_field& field)
{
    State current(pos.x, pos.y, vel.x, vel.y, t);
    
    // k1 = f(current)
    State k1 = derivatives(current, t, field);
    
    // k2 = f(current + dt/2 * k1)
    State y2 = {
        current.x + dt/2 * k1.x,      // позиция x + dt/2 * vx
        current.y + dt/2 * k1.y,      // позиция y + dt/2 * vy
        current.vx + dt/2 * k1.vx,    // скорость vx + dt/2 * ax
        current.vy + dt/2 * k1.vy,     // скорость vy + dt/2 * ay
        t + dt/2
    };
    State k2 = derivatives(y2, t + dt/2, field);
    
    // k3 = f(current + dt/2 * k2)
    State y3 = {
        current.x + dt/2 * k2.x,
        current.y + dt/2 * k2.y,
        current.vx + dt/2 * k2.vx,
        current.vy + dt/2 * k2.vy,
        t + dt/2
    };
    State k3 = derivatives(y3, t + dt/2, field);
    
    // k4 = f(current + dt * k3)
    State y4 = {
        current.x + dt * k3.x,
        current.y + dt * k3.y,
        current.vx + dt * k3.vx,
        current.vy + dt * k3.vy,
        t + dt
    };
    State k4 = derivatives(y4, t + dt, field);
    
    
    pos.x = current.x + (dt/6) * (k1.x + 2*k2.x + 2*k3.x + k4.x);
    pos.y = current.y + (dt/6) * (k1.y + 2*k2.y + 2*k3.y + k4.y);
    vel.x = current.vx + (dt/6) * (k1.vx + 2*k2.vx + 2*k3.vx + k4.vx);
    vel.y = current.vy + (dt/6) * (k1.vy + 2*k2.vy + 2*k3.vy + k4.vy);
}
};

void Gravity_field :: pos_update(const vector<Cosmic_bodies*>& bodies)
{
    masses.clear();
    for (auto body : bodies)
    {
        masses.push_back(Mass(body->getX(), body->getY(), body->getMass()));
    }
}


class Planet : public Cosmic_bodies
{
public:
    Planet(double m, double x, double y, double vx, double vy) : Cosmic_bodies(m, x, y, vx, vy) {}
};


class Spacecraft : public Cosmic_bodies
{
public:
    Spacecraft(double m, double x, double y, double vx, double vy) : Cosmic_bodies(m, x, y, vx, vy) {}
};


class Sun : public Cosmic_bodies
{
public:
    Sun() : Cosmic_bodies(1.98847e30, 0, 0, 0, 0) {};
    void update(double dt, double t, const Gravity_field& field) {}; //Солнце неподвижно
};

class  Earth : public Planet
{
public:
    Earth(double x, double y, double vx, double vy, double m = 5.9722e24) : Planet(m, x, y, vx, vy) {};
};

class Mars : public Planet
{
public:
    Mars(double x, double y, double vx, double vy, double m = 6.4171e23) : Planet(m, x, y, vx, vy) {};
};



void CSV(const string& filename, const vector<double>& times, const vector<double>& x, const vector<double>& y)
{
    ofstream file(filename);
    file << "t,x,y\n";
    for (size_t i = 0; i < times.size(); i++)
    {
        file << times[i] << "," << x[i] << "," << y[i] << "\n";
    }
    file.close();
    cout << "Файл " << filename << " успешно создан\n";
}

int main() {
    //объявляем классы планет,
    Sun sun;
    Earth earth(1.496e11, 0, 0, 29800);
    Mars mars(2.279e11, 0, 0, 24100);
    // и вектора для хранения указателей на эти классы
    vector<Cosmic_bodies*> bodies = {&sun, &earth, &mars};
    Gravity_field field;

    // Для хранения траекторий
    vector<double> times, earth_x, earth_y, mars_x, mars_y;
    
    double dt = 360*60;                    
    double total_seconds = 2 * 687 * 24 * 3600; 
    
    for (double ct = 0, lt=0; ct< total_seconds; ct+=dt)
    {
        if (ct-lt>24*3600)
        {
            times.push_back(ct);
            earth_x.push_back(earth.getX());
            earth_y.push_back(earth.getY());
            mars_x.push_back(mars.getX());
            mars_y.push_back(mars.getY());
            lt=ct;
        }
        field.pos_update(bodies);
        earth.update(dt, ct, field);
        field.pos_update(bodies);
        mars.update(dt, ct, field);
    }
    CSV("../data/earth_orbit.csv", times, earth_x, earth_y);
    CSV("../data/mars_orbit.csv", times, mars_x, mars_y);
    
    return 0;
}

