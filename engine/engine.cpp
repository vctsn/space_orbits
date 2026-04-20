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

// Конструктор по умолчанию
SimulationEngine::SimulationEngine() 
    : dt_(120), 
      total_seconds_(4 * 365 * 24 * 3600),
      save_interval_seconds_(24 * 3600),
      output_file_("data/simulation.csv") {
    cout << "Использованы настройки по умолчанию" << endl;
}

// Конструктор из конфига
SimulationEngine::SimulationEngine(const ConfigManager& cfg) {
    dt_ = cfg.dt();
    total_seconds_ = cfg.total_days() * 24 * 3600;
    save_interval_seconds_ = cfg.save_interval_hours() * 3600;
    output_file_ = cfg.output_file();
    
    cout << "Используем конфигруацию вида" << endl;
    cout << "Шаг по времени: " << dt_ << " секунд" << endl;
    cout << "Всего дней " << cfg.total_days() << endl;
    cout << "Интервал сохранения: " << cfg.save_interval_hours() << " часов" << endl;
    cout << "Сохраняем в  " << output_file_ << endl;
}

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
    
    double last_save = 0;
    
    planets_x.resize(bodies_.size() - 1);
    planets_y.resize(bodies_.size() - 1);    
    times.clear();

    for (double t = 0; t < total_seconds_; t += dt_) {
        
        // Используем save_interval_seconds_ вместо жестко заданного 24*3600
        if (t - last_save >= save_interval_seconds_) {
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
    string actual_filename = filename.empty() ? output_file_ : filename;
    ofstream file(actual_filename);
    if (file.is_open()) {
        // Заголовок
        vector<string> planet_names = {"Mercury", "Venus", "Earth", "Mars", "Jupiter", "Saturn", "Uranus", "Neptune"};
        if (bodies_.size() > 9) {
            planet_names.push_back("sat");
        }
        
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
        cout << "Все данные сохранены в " << actual_filename << endl;
    } else {
        cout << "Ошибка при создании файла " << actual_filename << endl;
    }
}

void SimulationEngine::add_spacecraft(double mass, double x, double y, double vx, double vy) {
    bodies_.push_back(new Spacecraft(mass, x, y, vx, vy));
    planets_x.push_back(vector<double>());
    planets_y.push_back(vector<double>());
    cout << "Спутник добавлен: Масса=" << mass << " кг, В точке (" << x/1e9 << ", " << y/1e9 << ") млн км" << endl;
}