#include "engine/engine.h"
#include <iostream>

int main() {
    SimulationEngine engine;
    engine.setup();
    
    engine.add_spacecraft(1000, 1.496e11*1.00001, 0, 1000, 5000); 
    engine.run();
    
    engine.save_csv("../data/simulation.csv");
    
    return 0;
}