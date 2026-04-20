#ifndef ENGINE_H
#define ENGINE_H

#include "../physics/physics.h"
#include <vector>
#include <string>

using std::vector;
using std::string;

class SimulationEngine {
private:
    vector<Cosmic_bodies*> bodies_;
    Gravity_field field_;
    
    double dt_;
    double total_seconds_;
    
public:
    SimulationEngine();
    ~SimulationEngine();
    
    void setup();
    void run();
    void save_csv(const string& filename);  // Убрали SimulationEngine::
    
    // Определяем ПКС
    const double get_velocity_one(const double R);
    
    vector<double> times;
    vector<vector<double>> planets_x;
    vector<vector<double>> planets_y;
};

#endif