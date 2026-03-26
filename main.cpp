#include "engine/engine.h"

int main() {
    SimulationEngine engine;
    
    engine.setup();
    engine.run();
    
    engine.save_csv("../data/earth_orbit.csv", engine.get_earth_x(), 
                    engine.get_earth_y(), "Земля");
    engine.save_csv("../data/mars_orbit.csv", engine.get_mars_x(), 
                    engine.get_mars_y(), "Марс");
    
    return 0;
}