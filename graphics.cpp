#include <SFML/Graphics.hpp>
#include <vector>
#include <array>
#include <fstream>
#include <iostream>

using std::vector;
using std::array;

struct Point 
{ 
    double x, y;
    Point(double x, double y) : x(x), y(y) {}
};


class Graphic_object
{
protected:
    sf::Color color;
    Point Centre{ sf::VideoMode::getDesktopMode().width/2.0, sf::VideoMode::getDesktopMode().height/2.0 }; //����� ����
public:
    Graphic_object () {}
    virtual void Draw(sf::RenderWindow& window) = 0; //����� ��������� ����������
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
};

class Planet : public Graphic_object
{
protected:
    sf::Time pause = sf::milliseconds(10);
    vector<Point> trajectory;
    sf::Clock animation_clock;
    int current_index = 0; //������ ������� ��������� (������� �����)
public:
    Planet() {};
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

struct Area //����� �������� ������, ���������� ������� �������� �������
{
    int x0, y0, x, y; //�0, �0 - ����� ������� ����, �, � - ������ ������
    Planet planet; //�������, ������� ����� �������� � ����������� �� �������
};

int main()
{
    setlocale(LC_ALL, "Russian");

    std::ifstream file1("../data/earth_orbit.csv");
    if (!file1.is_open()) { std::cout << "�� ������� ������� file1"; }

    std::ifstream file2("../data/mars_orbit.csv");
    if (!file2.is_open()) { std::cout << "�� ������� ������� file2"; }

    Earth earth(file1);
    Mars mars(file2);
    Sun sun;

    sf::RenderWindow window1(sf::VideoMode(sf::VideoMode::getDesktopMode().width, sf::VideoMode::getDesktopMode().height), L"��������� ����");

    //�������� ����
    sf::Texture texture;
    texture.loadFromFile("F.jpg");

    sf::Sprite sprite;
    sprite.setTexture(texture);
    sf::Vector2u tSize = texture.getSize();
    sf::Vector2u wSize = window1.getSize();
    sprite.setScale(static_cast<float> (wSize.x) / tSize.x, static_cast<float> (wSize.y) / tSize.y);
    
    //�������� ������
    sf::Font font;
    font.loadFromFile("G.ttf");
    if (!font.loadFromFile("G.ttf")) std::cout << "������ ��� �������� �����";
    else std::cout << "���� ������� ��������";

    sf::Text text(L"�������� ������� ��� ������:", font, 80);
    text.setFillColor(sf::Color::Black);
    text.setPosition(100, 70);

    sf::FloatRect bounds = text.getGlobalBounds();
    sf::RectangleShape rect(sf::Vector2f(bounds.width + 20, bounds.height + 20));
    rect.setPosition(sf::Vector2f(bounds.left - 10, bounds.top - 10));
    rect.setFillColor(sf::Color(255, 255, 255));

    vector<sf::Text> planets = {
        sf::Text(L"��������", font, 50),
        sf::Text(L"������", font, 50),
        sf::Text(L"����", font, 50),
        sf::Text(L"������", font, 50),
        sf::Text(L"������", font, 50),
        sf::Text(L"����", font, 50),
        sf::Text(L"������", font, 50),
    };

    array<Area, 7> transition; //������� ������ ����� ��, ��� � ��������� �������

    float startY = 100 + bounds.height + 40;
    for (unsigned int i = 0; i < planets.size(); i++)
    {
        planets[i].setFillColor(sf::Color::White);
        int Y = startY + i * 85;
        planets[i].setPosition(95, Y);

        sf::FloatRect boundsP = planets[i].getGlobalBounds();
        if (i == 2)
        {
            Area area{ 95, Y, 95 + boundsP.width, Y + bounds.height, mars };
            transition[i] = area;
        }
    }

    Planet draw_planet;

    //����
    while (window1.isOpen())
    {
        sf::Event event;
        while (window1.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) window1.close();
            if (event.type = sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    for (auto& i : transition)
                    {
                        if (i.x0 <= event.mouseButton.x && event.mouseButton.x <= i.x && i.y0 <= event.mouseButton.y && event.mouseButton.y <= i.y)
                        {
                            draw_planet = i.planet;
                            window1.close();
                        }
                    }
                }
            }
        }
        window1.clear();
        window1.draw(sprite);
        window1.draw(rect);
        window1.draw(text);

        float startY = 100 + bounds.height + 40;
        for (unsigned int i = 0; i < planets.size(); i++)
        {
            window1.draw(planets[i]);
        }
        window1.display();
    }

    //�������� ����
    sf::RenderWindow window2(sf::VideoMode(sf::VideoMode::getDesktopMode().width, sf::VideoMode::getDesktopMode().height), L"������");
    while (window2.isOpen())
    {
        sf::Event event;
        while (window2.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window2.close();
        }

        window2.clear();

        sun.Draw(window2);
        //earth.Draw(window2);
        //mars.Draw(window2);
        //earth.Update();
        //mars.Update();
        draw_planet.Draw(window2);
        draw_planet.Update();
        
        window2.display();
    }
    return 0;
}