#define _USE_MATH_DEFINES
#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <fstream>
#include <random>
#include <chrono>

using namespace std;

const double Msun = 1.98847e30;
const double G = 6.67430e-11;
const double EarthRadius = 6.371e6;
const double MarsRadius = 3.3895e6;
const double JupiterRadius = 6.9911e7;
const double VenusRadius = 6.0518e6;

double deg2rad(double deg) { return deg * M_PI / 180.0; }

struct Body {
    double mass;
    double x, y, vx, vy;
};

void compute_accelerations(const vector<Body>& bodies, vector<double>& ax, vector<double>& ay) {
    int n = bodies.size();
    fill(ax.begin(), ax.end(), 0.0);
    fill(ay.begin(), ay.end(), 0.0);
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            double dx = bodies[j].x - bodies[i].x;
            double dy = bodies[j].y - bodies[i].y;
            double r2 = dx*dx + dy*dy;
            if (r2 < 1e-20) continue;
            double r = sqrt(r2);
            double f = G * bodies[i].mass * bodies[j].mass / (r2 * r);
            double fx = f * dx;
            double fy = f * dy;
            ax[i] += fx / bodies[i].mass;
            ay[i] += fy / bodies[i].mass;
            ax[j] -= fx / bodies[j].mass;
            ay[j] -= fy / bodies[j].mass;
        }
    }
}

void update_position(Body& body, double dt, double ax, double ay) {
    body.x += body.vx * dt + 0.5 * ax * dt * dt;
    body.y += body.vy * dt + 0.5 * ay * dt * dt;
}

void update_velocity(Body& body, double dt, double ax, double ay) {
    body.vx += 0.5 * ax * dt;
    body.vy += 0.5 * ay * dt;
}

vector<Body> generate_asteroids(int N, double earth_x, double earth_y, double earth_vx, double earth_vy,
                                 double moon_radius = 3.844e8, double vel_disp = 10000.0) {
    mt19937 gen(chrono::steady_clock::now().time_since_epoch().count());
    uniform_real_distribution<> angle_dist(0, 2 * M_PI);
    uniform_real_distribution<> vel_dist(-vel_disp, vel_disp);
    vector<Body> asts(N);
    for (int i = 0; i < N; ++i) {
        double angle = angle_dist(gen);
        double dx = moon_radius * cos(angle);
        double dy = moon_radius * sin(angle);
        double x = earth_x + dx;
        double y = earth_y + dy;
        double vx = earth_vx + vel_dist(gen);
        double vy = earth_vy + vel_dist(gen);
        asts[i] = {1e12, x, y, vx, vy};
    }
    return asts;
}

struct Config {
    string name;
    bool remove_mars;
    bool remove_jupiter;
    bool remove_venus;
};

vector<Body> create_planets(const Config& cfg) {
    double earth_deg = 0.0;
    double mars_deg = 60.0;
    double jupiter_deg = 120.0;
    double venus_deg = 180.0;
    vector<Body> bodies;
    bodies.push_back({Msun, 0.0, 0.0, 0.0, 0.0});
    double r_earth = 1.496e11;
    double earth_x = r_earth * cos(deg2rad(earth_deg));
    double earth_y = r_earth * sin(deg2rad(earth_deg));
    double earth_vx = -29800 * sin(deg2rad(earth_deg));
    double earth_vy =  29800 * cos(deg2rad(earth_deg));
    bodies.push_back({5.9722e24, earth_x, earth_y, earth_vx, earth_vy});
    if (!cfg.remove_mars) {
        double r_mars = 2.279e11;
        double mars_x = r_mars * cos(deg2rad(mars_deg));
        double mars_y = r_mars * sin(deg2rad(mars_deg));
        double mars_vx = -24100 * sin(deg2rad(mars_deg));
        double mars_vy =  24100 * cos(deg2rad(mars_deg));
        bodies.push_back({6.4171e23, mars_x, mars_y, mars_vx, mars_vy});
    }
    if (!cfg.remove_jupiter) {
        double r_jup = 7.785e11;
        double jup_x = r_jup * cos(deg2rad(jupiter_deg));
        double jup_y = r_jup * sin(deg2rad(jupiter_deg));
        double jup_vx = -13070 * sin(deg2rad(jupiter_deg));
        double jup_vy =  13070 * cos(deg2rad(jupiter_deg));
        bodies.push_back({1.898e27, jup_x, jup_y, jup_vx, jup_vy});
    }
    if (!cfg.remove_venus) {
        double r_ven = 1.082e11;
        double ven_x = r_ven * cos(deg2rad(venus_deg));
        double ven_y = r_ven * sin(deg2rad(venus_deg));
        double ven_vx = -35020 * sin(deg2rad(venus_deg));
        double ven_vy =  35020 * cos(deg2rad(venus_deg));
        bodies.push_back({4.8675e24, ven_x, ven_y, ven_vx, ven_vy});
    }
    return bodies;
}

int main() {
    const int N_asteroids = 100;
    const double dt = 300.0;
    const double total_time = 5 * 365.25 * 24 * 3600;

    vector<Config> configs = {
        {"All planets", false, false, false},
        {"No Mars",     true,  false, false},
        {"No Jupiter",  false, true,  false},
        {"No Venus",    false, false, true},
        {"Only Earth",  true,  true,  true}
    };

    ofstream stats("impact_stats.csv");
    stats << "Config,ImpactsOnEarth,ImpactsOnMars,ImpactsOnJupiter,ImpactsOnVenus,Missed\n";

    ofstream traj_file("trajectories.csv");
    traj_file << "time,id,x,y\n";

    for (const Config& cfg : configs) {
        cout << "\n=== " << cfg.name << " ===\n";
        vector<Body> planets = create_planets(cfg);
        int earth_idx = 1;
        int mars_idx = -1, jupiter_idx = -1, venus_idx = -1;
        for (int i = 2; i < (int)planets.size(); ++i) {
            if (planets[i].mass == 6.4171e23) mars_idx = i;
            if (planets[i].mass == 1.898e27)  jupiter_idx = i;
            if (planets[i].mass == 4.8675e24) venus_idx = i;
        }

        double earth0_x = planets[earth_idx].x;
        double earth0_y = planets[earth_idx].y;
        double earth0_vx = planets[earth_idx].vx;
        double earth0_vy = planets[earth_idx].vy;

        vector<Body> asteroids = generate_asteroids(N_asteroids, earth0_x, earth0_y, earth0_vx, earth0_vy);
        vector<bool> alive(N_asteroids, true);

        int n_planets = planets.size();
        vector<double> ax(n_planets), ay(n_planets);

        int hits_earth = 0, hits_mars = 0, hits_jupiter = 0, hits_venus = 0;
        double last_save = 0;

        for (double t = 0; t < total_time; t += dt) {
            for (int i = 0; i < n_planets; ++i) {
                update_position(planets[i], dt, ax[i], ay[i]);
            }
            compute_accelerations(planets, ax, ay);
            for (int i = 0; i < n_planets; ++i) {
                update_velocity(planets[i], dt, ax[i], ay[i]);
            }

            for (int i = 0; i < N_asteroids; ++i) {
                if (!alive[i]) continue;
                update_position(asteroids[i], dt, 0.0, 0.0);
                update_velocity(asteroids[i], dt, 0.0, 0.0);
            }

            vector<Body> all = planets;
            all.insert(all.end(), asteroids.begin(), asteroids.end());
            vector<double> aall(all.size()), ayall(all.size());
            compute_accelerations(all, aall, ayall);
            int idx = n_planets;
            for (int i = 0; i < N_asteroids; ++i) {
                if (alive[i]) {
                    update_velocity(asteroids[i], dt, aall[idx], ayall[idx]);
                    idx++;
                }
            }

            for (int i = 0; i < N_asteroids; ++i) {
                if (!alive[i]) continue;
                double dx = asteroids[i].x - planets[earth_idx].x;
                double dy = asteroids[i].y - planets[earth_idx].y;
                if (sqrt(dx*dx + dy*dy) < EarthRadius) {
                    hits_earth++;
                    alive[i] = false;
                    continue;
                }
                if (mars_idx != -1) {
                    dx = asteroids[i].x - planets[mars_idx].x;
                    dy = asteroids[i].y - planets[mars_idx].y;
                    if (sqrt(dx*dx + dy*dy) < MarsRadius) {
                        hits_mars++;
                        alive[i] = false;
                        continue;
                    }
                }
                if (jupiter_idx != -1) {
                    dx = asteroids[i].x - planets[jupiter_idx].x;
                    dy = asteroids[i].y - planets[jupiter_idx].y;
                    if (sqrt(dx*dx + dy*dy) < JupiterRadius) {
                        hits_jupiter++;
                        alive[i] = false;
                        continue;
                    }
                }
                if (venus_idx != -1) {
                    dx = asteroids[i].x - planets[venus_idx].x;
                    dy = asteroids[i].y - planets[venus_idx].y;
                    if (sqrt(dx*dx + dy*dy) < VenusRadius) {
                        hits_venus++;
                        alive[i] = false;
                        continue;
                    }
                }
            }

            if (cfg.name == "All planets" && t - last_save >= 3600.0) {
                last_save = t;
                traj_file << t << ",-1," << planets[earth_idx].x << "," << planets[earth_idx].y << "\n";
                for (int i = 0; i < N_asteroids; ++i) {
                    if (alive[i]) {
                        traj_file << t << "," << i << "," << asteroids[i].x << "," << asteroids[i].y << "\n";
                    }
                }
            }
        }

        int missed = 0;
        for (bool a : alive) if (a) missed++;
        stats << cfg.name << "," << hits_earth << "," << hits_mars << ","
              << hits_jupiter << "," << hits_venus << "," << missed << "\n";
        cout << "Earth: " << hits_earth << ", Mars: " << hits_mars
             << ", Jupiter: " << hits_jupiter << ", Venus: " << hits_venus
             << ", Missed: " << missed << "\n";
    }
    stats.close();
    traj_file.close();
    cout << "\nStatistics saved to impact_stats.csv\n";
    cout << "Trajectories saved to trajectories.csv\n";
    return 0;
}