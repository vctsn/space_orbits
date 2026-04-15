#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
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
    double scale = 1;
    Point Centre{ 600, 300 }; //����� ����
public:
    Graphic_object () {}
    virtual void Draw(sf::RenderWindow& window) = 0; //����� ��������� ���������� 
};

class Sun : public Graphic_object
{
public:
    Sun() { color = sf::Color::Yellow; }
    void Draw(sf::RenderWindow& window)
    {
        sf::CircleShape Sun(10);
        Sun.setOrigin(sf::Vector2f(10, 10));
        Sun.setPosition(sf::Vector2f(Centre.x, Centre.y));
        Sun.setFillColor(color);
        window.draw(Sun);
    }
};

class Planet : public Graphic_object
{
protected:
    sf::Time pause = sf::milliseconds(10);
    vector<Point> trajectory;
    sf::Clock animation_clock;
    int current_index = 0; //������ ������� ��������� (������� �����)
public:
    Planet(std::ifstream& file)
    {
        std::string header;
        std::getline(file, header); //���������� ���������
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
    void Draw(sf::RenderWindow& window)
    {
        sf::CircleShape p(3);
        p.setFillColor(color);
        p.setPosition(sf::Vector2f(trajectory[current_index].x * scale + Centre.x, trajectory[current_index].y * scale + Centre.y));
        p.setOrigin(3, 3);
        window.draw(p);
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

    std::ifstream file1("../data/earth_orbit.csv");
    if (!file1.is_open()) { std::cout << "ошибка открытия файла(1)"; }

    std::ifstream file2("../data/mars_orbit.csv");
    if (!file2.is_open()) { std::cout << "ошибка открытия файла(2)"; }
    sf::Music music;
    if (!music.openFromFile("/mnt/c/space_orbits-main/Smeshariki_-_Ot_vinta_48225827.mp3")) {
        std::cout << "Ошибка загрузки музыки" << std::endl;
        // можно продолжить без музыки
    }
    music.setLoop(true);      // зациклить
    music.play();

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
        }

        sun.Draw(window);

        for (unsigned int i = 0; i < earth.Get_trajectory(); i++)
        {
            earth.Draw(window);
            mars.Draw(window);
            earth.Update();
            mars.Update();
        }
        window.display();
    }

    
    return 0;
}