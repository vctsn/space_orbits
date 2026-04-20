#ifndef ENGINE_H
#define ENGINE_H

#include "../physics/physics.h"
#include <vector>
#include <string>
#include "../config.h"

using std::vector;
using std::string;

class SimulationEngine {
private:
    vector<Cosmic_bodies*> bodies_;
    Gravity_field field_;
    double dt_;
    double total_seconds_;
    double save_interval_seconds_;
    string output_file_;
    
public:
    // Два конструктора
    SimulationEngine();  // значения по умолчанию
    SimulationEngine(const ConfigManager& cfg);  // из конфига
    ~SimulationEngine();
    
    void setup();
    void run();
    void save_csv(const string& filename = "");
    void add_spacecraft(double mass, double x, double y, double vx, double vy);
    
    const double get_velocity_one(const double R);
    
    vector<double> times;
    vector<vector<double>> planets_x;
    vector<vector<double>> planets_y;
};

#endif