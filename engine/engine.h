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
    
    vector<double> times;
    vector<double> earth_x, earth_y;
    vector<double> mars_x, mars_y;
    
    double dt_;
    double total_seconds_;
    
public:
    SimulationEngine();
    ~SimulationEngine();
    
    void setup();
    void run();
    void save_csv(const string& filename, const vector<double>& x, 
                  const vector<double>& y, const string& name);
    
    // Геттеры для доступа к данным
    const vector<double>& get_times() const { return times; }
    const vector<double>& get_earth_x() const { return earth_x; }
    const vector<double>& get_earth_y() const { return earth_y; }
    const vector<double>& get_mars_x() const { return mars_x; }
    const vector<double>& get_mars_y() const { return mars_y; }
};

#endif