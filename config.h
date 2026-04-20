// config.h (максимально простой)
#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <fstream>
#include <iostream>
#include "json.hpp"

using json = nlohmann::json;

class ConfigManager {
private:
    json data;
    
public:
    ConfigManager(const std::string& path = "config.json") {
        std::ifstream file(path);
        if (file.is_open()) {
            file >> data;
            std::cout << "Config loaded" << std::endl;
        } else {
            std::cout << "Using defaults" << std::endl;
        }
    }
    
    double dt() const { return data.value("dt_seconds", 120.0); }
    double total_days() const { return data.value("total_days", 5840.0); }
    double save_interval_hours() const { return data.value("save_interval_hours", 24.0); }
    std::string output_file() const { return data.value("output_file", "data/simulation.csv"); }
    bool spacecraft_enabled() const { return data.value("enabled", true); }
    double spacecraft_mass() const { return data.value("mass_kg", 1000.0); }
    double spacecraft_x() const { return data.value("pos_x", 1.496e11); }
    double spacecraft_y() const { return data.value("pos_y", 0.0); }
    double spacecraft_vx() const { return data.value("vx", 1000.0); }
    double spacecraft_vy() const { return data.value("vy", 5000.0); }
};

#endif