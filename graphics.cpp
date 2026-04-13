#include <SFML/Graphics.hpp>
#include <vector>
#include <fstream>
#include <iostream>
using std::vector;

struct Point 
{ 
    double x, y;
    Point(double x, double y) : x(x), y(y) {}
};

class Graphic_object
{
protected:
    sf::Color color;
    Point Centre{ 600, 300 }; //центр окна
public:
    Graphic_object () {}
    virtual void Draw(sf::RenderWindow& window) = 0; //метод отрисовки траектории 
    virtual void Resize()
    {
        Centre.x *= sf::VideoMode::getDesktopMode().width / 1200;
        Centre.y *= sf::VideoMode::getDesktopMode().height / 600;
    }
};

class Sun : public Graphic_object
{
public:
    Sun() 
    { 
        color = sf::Color::Yellow; 
    }
    void Draw(sf::RenderWindow& window)
    {
        sf::CircleShape Sun(10);
        Sun.setOrigin(sf::Vector2f(10, 10));
        Sun.setPosition(sf::Vector2f(Centre.x, Centre.y));
        Sun.setFillColor(color);
        window.draw(Sun);
    }
    void Resize() { Graphic_object::Resize(); }
};

class Planet : public Graphic_object
{
protected:
    sf::Time pause = sf::milliseconds(10);
    vector<Point> trajectory;
    sf::Clock animation_clock;
    int current_index = 0; //хранит текущее положение (текущую точку)
public:
    Planet(std::ifstream& file)
    {
        std::string header;
        std::getline(file, header); //пропускаем заголовок
        double t, x, y;
        char comma;
        while (file >> t >> comma >> x >> comma >> y)
        {
            Point point(x, y);
            trajectory.push_back(point);
        }
        file.close();
    }
    unsigned int Get_trajectory() { return trajectory.size(); }
    void Update()
    {
        if (animation_clock.getElapsedTime() >= pause)
        {
            current_index = (current_index + 1) % trajectory.size();
            animation_clock.restart();
        }
    }
    void DrawFullTrajectory(sf::RenderWindow& window)
    {
        for (int i = 0; i < current_index; i++)
        {
            sf::CircleShape p(1);
            p.setFillColor(color);
            p.setPosition(sf::Vector2f(trajectory[i].x + Centre.x, trajectory[i].y + Centre.y));
            p.setOrigin(1, 1);
            window.draw(p);
        }
    }
    void Draw(sf::RenderWindow& window)
    {
        DrawFullTrajectory(window);
        sf::CircleShape p(3);
        p.setFillColor(color);
        p.setPosition(sf::Vector2f(trajectory[current_index].x + Centre.x, trajectory[current_index].y + Centre.y));
        p.setOrigin(3, 3);
        window.draw(p);
    }
    void Resize()
    {
        for (auto& p : trajectory)
        {
            p.x *= sf::VideoMode::getDesktopMode().width/1200;
            p.y *= sf::VideoMode::getDesktopMode().height/600;
        }
    }
};

class Earth : public Planet
{
public:
    Earth(std::ifstream& file) : Planet(file) { color = sf::Color::Blue; }
};

class Mars : public Planet
{
public:
    Mars(std::ifstream& file) : Planet(file) { color = sf::Color::Red; }
};


int main()
{
    setlocale(LC_ALL, "Russian");

    std::ifstream file1("C:\\Users\\1\\Desktop\\project\\space_orbits\\data\\earth_orbit.csv");
    if (!file1.is_open()) { std::cout << "Ќе удалось открыть file1"; }

    std::ifstream file2("C:\\Users\\1\\Desktop\\project\\space_orbits\\data\\mars_orbit.csv");
    if (!file2.is_open()) { std::cout << "Ќе удалось открыть file2"; }

    Earth earth(file1);
    Mars mars(file2);
    Sun sun;

    sf::RenderWindow window(sf::VideoMode(1200, 600), "Orbits");
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::Resized)
            {
                earth.Resize();
                mars.Resize();
                sun.Resize();
            }
        }

        window.clear();
        
        sun.Draw(window);

        earth.Draw(window);
        mars.Draw(window); 
        earth.Update();
        mars.Update();
        
        window.display();
    }

    
    return 0;
}