#include "engine/engine.h"

int main() {
    SimulationEngine engine;
    
    engine.setup();
    engine.run();
    
    engine.save_csv("../data/orbits.csv");
    
    return 0;
}