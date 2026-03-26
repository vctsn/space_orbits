#include "engine.h"
#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;

SimulationEngine::SimulationEngine() 
    : dt_(1800), total_seconds_(2 * 687 * 24 * 3600) {}

SimulationEngine::~SimulationEngine() {
    for (auto body : bodies_) {
        delete body;
    }
}

void SimulationEngine::setup() {
    // Создаем тела
    bodies_.push_back(new Sun());
    
    double v_earth = sqrt(G * Msun / 1.496e11);
    double v_mars = sqrt(G * Msun / 2.279e11);
    
    bodies_.push_back(new Earth(1.496e11, 0, 0, v_earth));
    bodies_.push_back(new Mars(2.279e11, 0, 0, v_mars));
    
    cout << "Солнечная система настроена" << endl;
}

void SimulationEngine::run() {
    cout << "Запуск симуляции..." << endl;
    cout << "Шаг: " << dt_ << " сек, Время: " << total_seconds_/(24*3600) << " дней" << endl;
    
    double last_save = 0;
    
    for (double t = 0; t < total_seconds_; t += dt_) {
        // Сохраняем раз в сутки
        if (t - last_save >= 24 * 3600) {
            times.push_back(t / (24 * 3600));
            earth_x.push_back(bodies_[1]->getX() / 1e9);
            earth_y.push_back(bodies_[1]->getY() / 1e9);
            mars_x.push_back(bodies_[2]->getX() / 1e9);
            mars_y.push_back(bodies_[2]->getY() / 1e9);
            last_save = t;
        }
        
        // Обновляем поле
        field_.update_from_bodies(bodies_);
        
        // Обновляем планеты (Солнце не обновляем)
        for (int i = 1; i < bodies_.size(); i++) {
            bodies_[i]->update(dt_, field_);
        }
    }
    
    cout << "Симуляция завершена. Сохранено точек: " << times.size() << endl;
}

void SimulationEngine::save_csv(const string& filename, const vector<double>& x, 
                                const vector<double>& y, const string& name) {
    ofstream file(filename);
    if (file.is_open()) {
        file << "t,x,y\n";
        for (size_t i = 0; i < times.size(); i++) {
            file << times[i] << "," << x[i] << "," << y[i] << "\n";
        }
        file.close();
        cout << name << " сохранен в " << filename << endl;
    } else {
        cout << "Ошибка при создании файла " << filename << endl;
    }
}