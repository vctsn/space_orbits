// test_energy.cpp
#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
using namespace std;

const double G = 6.67430e-11;
const double Msun = 1.98847e30;
const double Mearth = 5.9722e24;
const double Mmars = 6.4171e23;
const double AU = 1.496e11;

struct State {
    double x, y, vx, vy, t;
    State(double x=0, double y=0, double vx=0, double vy=0, double t=0)
        : x(x), y(y), vx(vx), vy(vy), t(t) {}
};

class Cosmic_bodies;

class Gravity_field {
private:
    struct Mass {
        double x, y, mass;
        Mass(double x, double y, double m) : x(x), y(y), mass(m) {}
    };
    vector<Mass> masses;
public:
    struct Point {
        double gx, gy;
        Point(double gx=0, double gy=0) : gx(gx), gy(gy) {}
    };
    
    Point calculating_field(double x, double y) const {
        double gx = 0, gy = 0;
        for (const Mass& m : masses) {
            double rx = m.x - x;
            double ry = m.y - y;
            double r = sqrt(rx*rx + ry*ry);
            if (r > 1e-10) {
                double a = G * m.mass / (r*r*r);
                gx += a * rx;
                gy += a * ry;
            }
        }
        return Point(gx, gy);
    }
    
    void pos_update(const vector<Cosmic_bodies*>& bodies);
};

class Cosmic_bodies {
protected:
    double mass;
    double x, y, vx, vy;
public:
    Cosmic_bodies(double m, double x0, double y0, double vx0, double vy0)
        : mass(m), x(x0), y(y0), vx(vx0), vy(vy0) {}
    
    double getMass() const { return mass; }
    double getX() const { return x; }
    double getY() const { return y; }
    double getVx() const { return vx; }
    double getVy() const { return vy; }
    
    void update(double dt, const Gravity_field& field) {
        Gravity_field::Point g = field.calculating_field(x, y);
        vx += g.gx * dt;
        vy += g.gy * dt;
        x += vx * dt;
        y += vy * dt;
    }
};

void Gravity_field::pos_update(const vector<Cosmic_bodies*>& bodies) {
    masses.clear();
    for (auto b : bodies) {
        masses.push_back(Mass(b->getX(), b->getY(), b->getMass()));
    }
}

class Sun : public Cosmic_bodies {
public:
    Sun() : Cosmic_bodies(Msun, 0, 0, 0, 0) {}
};

class Earth : public Cosmic_bodies {
public:
    Earth(double x, double y, double vx, double vy) 
        : Cosmic_bodies(Mearth, x, y, vx, vy) {}
};

class Mars : public Cosmic_bodies {
public:
    Mars(double x, double y, double vx, double vy) 
        : Cosmic_bodies(Mmars, x, y, vx, vy) {}
};

double total_energy(vector<Cosmic_bodies*>& bodies) {
    double E = 0;
    for (auto b : bodies) {
        E += 0.5 * b->getMass() * (b->getVx()*b->getVx() + b->getVy()*b->getVy());
    }
    for (size_t i = 0; i < bodies.size(); i++) {
        for (size_t j = i+1; j < bodies.size(); j++) {
            double dx = bodies[i]->getX() - bodies[j]->getX();
            double dy = bodies[i]->getY() - bodies[j]->getY();
            double r = sqrt(dx*dx + dy*dy);
            if (r > 1e-10) {
                E -= G * bodies[i]->getMass() * bodies[j]->getMass() / r;
            }
        }
    }
    return E;
}

int main() {
    cout << "Proverka energii\n";
    cout << "================\n\n";
    
    Sun sun;
    Earth earth(AU, 0, 0, sqrt(G * Msun / AU));
    Mars mars(2.279e11, 0, 0, sqrt(G * Msun / 2.279e11));
    
    vector<Cosmic_bodies*> bodies = {&sun, &earth, &mars};
    Gravity_field field;
    
    double E_start = total_energy(bodies);
    cout << "E_start = " << E_start << " J\n";
    
    double dt = 6 * 3600;
    double total_time = 2 * 687 * 24 * 3600;
    int steps = total_time / dt;
    
    for (int i = 0; i <= steps; i++) {
        if (i % (30*4) == 0) {
            double E = total_energy(bodies);
            double err = fabs((E - E_start) / E_start) * 100;
            int day = i * dt / 86400;
            cout << "Day " << day << ": error = " << err << "%\n";
        }
        
        field.pos_update(bodies);
        earth.update(dt, field);
        mars.update(dt, field);
    }
    
    double E_end = total_energy(bodies);
    double drift = (E_end - E_start) / E_start * 100;
    
    cout << "\nResult:\n";
    cout << "Drift = " << drift << "%\n";
    
    return 0;
}