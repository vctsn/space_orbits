#include "engine.h"
#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;

// Меркурий
#define R_mercury 5.790e10
#define M_mercury 3.285e23
// Венера
#define R_venus 1.082e11
#define M_venus 4.867e24
// Земля
#define R_earth 1.496e11
#define M_earth 5.972e24
// Марс
#define R_mars 2.279e11
#define M_mars 6.417e23
// Юпитер
#define R_jupiter 7.786e11
#define M_jupiter 1.898e27
// Сатурн
#define R_saturn 1.434e12
#define M_saturn 5.683e26
// Уран
#define R_uranus 2.871e12
#define M_uranus 8.681e25
// Нептун
#define R_neptune 4.495e12
#define M_neptune 1.024e26


SimulationEngine::SimulationEngine() 
    : dt_(120), total_seconds_(4 * 365 * 24 * 3600) {}

SimulationEngine::~SimulationEngine() {
    for (auto body : bodies_) {
        delete body;
    }
}

const double SimulationEngine::get_velocity_one( const double R){
    return sqrt(G*Msun/R);
}

void SimulationEngine::setup() {
    // Создаем тела
    bodies_.push_back(new Sun());
    
    double v_mercury = get_velocity_one(R_mercury);
    double v_venus = get_velocity_one(R_venus);
    double v_earth = get_velocity_one(R_earth);
    double v_mars = get_velocity_one(R_mars);
    double v_jupiter = get_velocity_one(R_jupiter);
    double v_saturn = get_velocity_one(R_saturn);
    double v_uranus = get_velocity_one(R_uranus);
    double v_neptune = get_velocity_one(R_neptune);
    
    bodies_.push_back(new Mercury(R_mercury, 0, 0, v_mercury, M_mercury));
    bodies_.push_back(new Venus(R_venus, 0, 0, v_venus, M_venus));
    bodies_.push_back(new Earth(R_earth, 0, 0, v_earth, M_earth));
    bodies_.push_back(new Mars(R_mars, 0, 0, v_mars, M_mars));
    bodies_.push_back(new Jupiter(R_jupiter, 0, 0, v_jupiter, M_jupiter));
    bodies_.push_back(new Saturn(R_saturn, 0, 0, v_saturn, M_saturn));
    bodies_.push_back(new Uranus(R_uranus, 0, 0, v_uranus, M_uranus));
    bodies_.push_back(new Neptune(R_neptune, 0, 0, v_neptune, M_neptune));

    cout << "Солнечная система настроена" << endl;
}

void SimulationEngine::run() {
    cout << "Запуск симуляции..." << endl;
    cout << "Шаг: " << dt_ << " сек, Время: " << total_seconds_/(24*3600) << " дней" << endl;
    
    double last_save = 0;

    
    planets_x.resize(bodies_.size() - 1);
    planets_y.resize(bodies_.size() - 1);    
    times.clear();

    for (double t = 0; t < total_seconds_; t += dt_) {
        
        // Сохраняем раз в сутки
        if (t - last_save >= 24 * 3600) {
            times.push_back(t / (24 * 3600));             
            for (int i = 1; i < bodies_.size(); i++) {
                planets_x[i-1].push_back(bodies_[i]->getX() / 1e9);
                planets_y[i-1].push_back(bodies_[i]->getY() / 1e9);
            }
            
            last_save = t;
        }
        
        field_.update_from_bodies(bodies_);
        for (int i = 1; i < bodies_.size(); i++) {
            bodies_[i]->update(dt_, field_);
        }
    }
    
    cout << "Симуляция завершена. Сохранено точек: " << times.size() << endl;
}

void SimulationEngine::save_csv(const string& filename) {
    ofstream file(filename);
    if (file.is_open()) {
        // Заголовок
        vector<string> planet_names = {"Mercury", "Venus", "Earth", "Mars", "Jupiter", "Saturn", "Uranus", "Neptune", "sat"};
        file << "t";
        for (const auto& name : planet_names) {
            file << "," << name << "_x," << name << "_y";
        }
        file << "\n";
        
        // Данные из сохранённых векторов planets_x и planets_y
        for (size_t i = 0; i < times.size(); i++) {
            file << times[i];
            for (size_t j = 0; j < planets_x.size(); j++) {
                file << "," << planets_x[j][i] << "," << planets_y[j][i];
            }
            file << "\n";
        }
        file.close();
        cout << "Все планеты сохранены в " << filename << endl;
    } else {
        cout << "Ошибка при создании файла " << filename << endl;
    }
}

void SimulationEngine::add_spacecraft(double mass, double x, double y, double vx, double vy) {
    bodies_.push_back(new Spacecraft(mass, x, y, vx, vy));
    planets_x.push_back(vector<double>());
    planets_y.push_back(vector<double>());
    cout << "Спутник добавлен" << endl;
}