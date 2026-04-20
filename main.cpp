#include "engine/engine.h"
#include "config.h"

int main() {
    ConfigManager cfg;
    SimulationEngine engine(cfg);
    
    engine.setup();
    
    if (cfg.spacecraft_enabled()) {
        engine.add_spacecraft(
            cfg.spacecraft_mass(),
            cfg.spacecraft_x(), cfg.spacecraft_y(),
            cfg.spacecraft_vx(), cfg.spacecraft_vy()
        );
    }
    
    engine.run();
    engine.save_csv();
    
    return 0;
}