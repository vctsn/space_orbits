#include <SFML/Graphics.hpp>
#include <vector>
#include <fstream>
#include <iostream>
using std::vector;

class Point
{
private:
    double value_x, value_y;
public:
    Point(double x, double y) : value_x(x), value_y(y) {}
    void Draw(Point& point, double scale, sf::RenderWindow& window, double x_sun, double y_sun) //метод отрисовки каждой точки траектории 
    {
        sf::CircleShape p(3);
        p.setFillColor(sf::Color::Blue);
        p.setPosition(sf::Vector2f(point.value_x*scale + x_sun, point.value_y*scale + y_sun));
        p.setOrigin(3, 3);
        window.draw(p);
    }
};

int main()
{

    vector<Point> points;
    std::ifstream file("C:\\Users\\1\\source\\repos\\SPACE\\data\\earth_orbit.csv");

    if (!file.is_open()) { std::cout << "Open error" << std::endl; }

    std::string header;
    std::getline(file, header);

    double t, x, y;
    char comma;
    while (file >> t >> comma >> x >> comma >> y)
    {
        Point point(x, y);
        points.push_back(point);
    }
    file.close();
    std::cout << "Points: " << points.size() << std::endl; //загружено точек

    sf::RenderWindow window(sf::VideoMode(1200, 600), "Orbits");
    window.clear(sf::Color::White);
    sf::CircleShape Sun(10);
    Sun.setOrigin(sf::Vector2f(10, 10));
    Sun.setPosition(sf::Vector2f(600, 300));
    Sun.setFillColor(sf::Color::Yellow);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        window.clear();
        window.draw(Sun);
        for (int i = 0; i < points.size(); i++) { points[i].Draw(points[i], 1e-9, window, 600, 300); }
        window.display();
    }

    return 0;
}