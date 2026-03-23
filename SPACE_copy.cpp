#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <fstream>
using std::cout;
using std::endl;
using std::vector;
using std::sqrt;
using std::string;
using std::ofstream;

const double Msun = 1.98847e30;
const double G = 6.67430e-11;

struct State {
    double x, y, vx, vy, t;
    State(double x = 0, double y = 0, double vx = 0, double vy = 0, double t = 0)
        : x(x), y(y), vx(vx), vy(vy), t(t) {}
};

// Forward declaration
class Cosmic_bodies;

class Gravity_field {
private:
    struct Mass {
        double x, y, mass;
        Mass(double x, double y, double m) : x(x), y(y), mass(m) {}
    };
    vector<Mass> masses;
    
public:
    void update_from_bodies(const vector<Cosmic_bodies*>& bodies);
    void add_mass(double x, double y, double m);
    void clear_mass();
    
    struct Point {
        double gx, gy;
        Point(double gx = 0, double gy = 0) : gx(gx), gy(gy) {}
    };
    
    Point calculating_field(double x, double y) const;
    void pos_update(const vector<Cosmic_bodies*>& bodies);
};

class Cosmic_bodies {
protected:
    double mass;
    double pos_x, pos_y;
    double vel_x, vel_y;
    
public:
    Cosmic_bodies(double m, double x0, double y0, double vx0, double vy0)
        : mass(m), pos_x(x0), pos_y(y0), vel_x(vx0), vel_y(vy0) {}
    
    virtual ~Cosmic_bodies() {}
    
    double getMass() const { return mass; }
    double getX() const { return pos_x; }
    double getY() const { return pos_y; }
    double getVx() const { return vel_x; }
    double getVy() const { return vel_y; }
    
    void setPosition(double x, double y) {
        pos_x = x;
        pos_y = y;
    }
    
    void setVelocity(double vx, double vy) {
        vel_x = vx;
        vel_y = vy;
    }
    
    // Метод для обновления положения и скорости (Эйлер-Кромер)
    virtual void update(double dt, const Gravity_field& field) {
        // Вычисляем ускорение в текущей точке
        Gravity_field::Point g = field.calculating_field(pos_x, pos_y);
        
        // Сначала обновляем скорость
        vel_x += g.gx * dt;
        vel_y += g.gy * dt;
        
        // Затем обновляем позицию с новой скоростью
        pos_x += vel_x * dt;
        pos_y += vel_y * dt;
    }
};


void Gravity_field::update_from_bodies(const vector<Cosmic_bodies*>& bodies) {
    masses.clear();
    for (auto body : bodies) {
        masses.push_back(Mass(body->getX(), body->getY(), body->getMass()));
    }
}

void Gravity_field::add_mass(double x, double y, double m) {
    masses.push_back(Mass(x, y, m));
}

void Gravity_field::clear_mass() {
    masses.clear();
}

Gravity_field::Point Gravity_field::calculating_field(double x, double y) const {
    double total_gx = 0;
    double total_gy = 0;
    for (const Mass& m : masses) {
        double rx = m.x - x;
        double ry = m.y - y;
        double r = sqrt(rx * rx + ry * ry);
        if (r > 1e-10) {
            double factor = G * m.mass / (r * r * r);
            total_gx += factor * rx;
            total_gy += factor * ry;
        }
    }
    return Point(total_gx, total_gy);
}

void Gravity_field::pos_update(const vector<Cosmic_bodies*>& bodies) {
    update_from_bodies(bodies);
}

// Производные классы
class Planet : public Cosmic_bodies {
public:
    Planet(double m, double x, double y, double vx, double vy) 
        : Cosmic_bodies(m, x, y, vx, vy) {}
};

class Spacecraft : public Cosmic_bodies {
public:
    Spacecraft(double m, double x, double y, double vx, double vy) 
        : Cosmic_bodies(m, x, y, vx, vy) {}
};

class Sun : public Cosmic_bodies {
public:
    Sun() : Cosmic_bodies(1.98847e30, 0, 0, 0, 0) {}
    void update(double dt, const Gravity_field& field) override {
        // Солнце неподвижно
    }
};

class Earth : public Planet {
public:
    Earth(double x, double y, double vx, double vy, double m = 5.9722e24) 
        : Planet(m, x, y, vx, vy) {}
};

class Mars : public Planet {
public:
    Mars(double x, double y, double vx, double vy, double m = 6.4171e23) 
        : Planet(m, x, y, vx, vy) {}
};

void CSV(const string& filename, const vector<double>& times, 
         const vector<double>& x, const vector<double>& y) {
    ofstream file(filename);
    if (file.is_open()) {
        file << "t,x,y\n";
        for (size_t i = 0; i < times.size(); i++) {
            file << times[i] << "," << x[i] << "," << y[i] << "\n";
        }
        file.close();
        cout << "Файл " << filename << " успешно создан\n";
    } else {
        cout << "Ошибка при создании файла " << filename << endl;
    }
}

int main() {
    cout << "=== Моделирование солнечной системы ===" << endl;
    cout << "Метод: Эйлер-Кромер (полу-неявный Эйлер)" << endl;
    
    // Создаем объекты
    Sun sun;
    
    // Орбитальные скорости (круговая орбита)
    double v_earth = sqrt(G * Msun / 1.496e11);  // ~29780 m/s
    double v_mars = sqrt(G * Msun / 2.279e11);   // ~24130 m/s
    
    Earth earth(1.496e11, 0, 0, v_earth);
    Mars mars(2.279e11, 0, 0, v_mars);
    
    // Вектор всех тел
    vector<Cosmic_bodies*> bodies = {&sun, &earth, &mars};
    Gravity_field field;
    
    // Для хранения траекторий
    vector<double> times, earth_x, earth_y, mars_x, mars_y;
    
    // Параметры моделирования
    double dt = 1800;  // 0.5 часа = 1800 секунд (для лучшей точности)
    double total_seconds = 2 * 687 * 24 * 3600;  // 2 марсианских года
    double last_save_time = 0;
    double last_print_time = 0;
    
    cout << "Шаг интегрирования: " << dt << " сек (" << dt/3600 << " часов)" << endl;
    cout << "Общее время: " << total_seconds/(24*3600) << " дней" << endl;
    cout << "Начальные позиции:" << endl;
    cout << "  Земля: (" << earth.getX()/1e9 << ", " << earth.getY()/1e9 << ") млн км" << endl;
    cout << "  Марс:  (" << mars.getX()/1e9 << ", " << mars.getY()/1e9 << ") млн км" << endl;
    cout << "Начальные скорости:" << endl;
    cout << "  Земля: " << earth.getVx() << " м/с" << endl;
    cout << "  Марс:  " << mars.getVx() << " м/с" << endl;
    cout << endl;
    
    // Основной цикл моделирования
    for (double ct = 0; ct < total_seconds; ct += dt) {
        // Сохраняем данные раз в сутки
        if (ct - last_save_time >= 24 * 3600) {
            times.push_back(ct / (24 * 3600));  // Сохраняем в днях
            earth_x.push_back(earth.getX() / 1e9);  // в млн км
            earth_y.push_back(earth.getY() / 1e9);
            mars_x.push_back(mars.getX() / 1e9);
            mars_y.push_back(mars.getY() / 1e9);
            last_save_time = ct;
        }
        
        // Выводим прогресс каждые 30 дней
        if (ct - last_print_time >= 30 * 24 * 3600 && ct > 0) {
            double days = ct / (24 * 3600);
            cout << "Прогресс: " << days << " дней (" 
                 << (ct/total_seconds)*100 << "%)" << endl;
            cout << "  Земля: (" << earth.getX()/1e9 << ", " << earth.getY()/1e9 << ") млн км" << endl;
            cout << "  Марс:  (" << mars.getX()/1e9 << ", " << mars.getY()/1e9 << ") млн км" << endl;
            last_print_time = ct;
        }
        
        // Обновляем поле по текущим позициям
        field.update_from_bodies(bodies);
        
        // Обновляем все тела (кроме Солнца)
        for (auto body : bodies) {
            if (dynamic_cast<Sun*>(body) == nullptr) {
                body->update(dt, field);
            }
        }
    }
    
    cout << endl << "Моделирование завершено!" << endl;
    cout << "Сохранено точек: " << times.size() << endl;
    
    // Сохраняем траектории
    CSV("earth_orbit.csv", times, earth_x, earth_y);
    CSV("mars_orbit.csv", times, mars_x, mars_y);
    
    // Выводим конечные позиции
    cout << "\nКонечные позиции:" << endl;
    cout << "  Земля: (" << earth.getX()/1e9 << ", " << earth.getY()/1e9 << ") млн км" << endl;
    cout << "  Марс:  (" << mars.getX()/1e9 << ", " << mars.getY()/1e9 << ") млн км" << endl;
    
    return 0;
}