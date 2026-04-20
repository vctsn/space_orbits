#include <cassert>
#include <cmath>
#include "physics.h"

void test_planet_orbit_stability() {
    Sun sun;
    double x0 = 1.496e11; // 1 а.е.
    double y0 = 0;
    double v0 = sqrt(G * Msun / x0);
    Earth earth(x0, y0, 0, v0);

    Gravity_field field;
    vector<Cosmic_bodies*> bodies = {&sun, &earth};
    field.pos_update(bodies);

    double initial_distance = sqrt(earth.getX() * earth.getX() + earth.getY() * earth.getY());

    double dt = 3600;
    int steps = int(10 * 24);
    for (int i = 0; i < steps; ++i) {
        field.pos_update(bodies);
        earth.update(dt, 0, field);
    }

    double final_distance = sqrt(earth.getX() * earth.getX() + earth.getY() * earth.getY());

    // Проверка, что расстояние не слишком изменилось
    assert(std::abs(final_distance - initial_distance) / initial_distance < 0.01);
    std::cout << "Тест орбиты прошел успешно." << std::endl;
}

int main() {
    test_planet_orbit_stability();
    return 0;
}