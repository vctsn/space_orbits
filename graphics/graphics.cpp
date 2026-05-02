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
    Point Centre{ sf::VideoMode::getDesktopMode().width/2.0, sf::VideoMode::getDesktopMode().height/2.0 }; //центр окна
public:
    Graphic_object () {}
    virtual void Draw(sf::RenderWindow& window) {}; //метод отрисовки траектории
    virtual void Update() {};
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
    int current_index = 0; //хранит текущее положение (текущую точку)
public:
    Planet() {};
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


class Window
{
protected:
    sf::Font font;
    unsigned int x_window = sf::VideoMode::getDesktopMode().width; //ширина окна
    unsigned int y_window = sf::VideoMode::getDesktopMode().height; //высота окна
public:
    ~Window() {};
    Window() 
    {
        //Сoздние текста
        if (!font.loadFromFile("G.ttf"))
            std::cout << "Ошибка при открытии файла шрифта" << std::endl;
        else
            std::cout << "Файл шрифта успешно загружен" << std::endl;
    };
};

class Menu : public Window
{
private:
    struct Area //здесь хранится облать, занимаемая текстом названия планеты
    {
        int x0, y0, x, y; //х0, у0 - левый верхний угол, х, у - правый нижний
        int num; //число планет в системе
    };
    sf::RenderWindow window{ sf::VideoMode(x_window, y_window), L"Начальное меню" };
    sf::Texture texture;
    sf::Sprite sprite;
    array<Area, 5> transition;
    sf::Text text; //заголовок  
    sf::FloatRect bounds;
    float x_head = x_window / 15.0;
    float y_head = y_window / 15.0;
    sf::RectangleShape rect;
    vector<sf::Text> button;
public:
    Menu()
    {
        //Создаем заголовок 
        text = sf::Text(L"Выберите диапазон скоростей астероидов (км/c):", font, 60);
        text.setFillColor(sf::Color::Black);
        text.setPosition(x_head, y_head);

        //Cоздаем рамку для заголовка 
        bounds = text.getGlobalBounds();
        rect = sf::RectangleShape(sf::Vector2f(bounds.width + x_head / 6.0, bounds.height + y_head / 6.0));
        rect.setPosition(sf::Vector2f(bounds.left - x_head / 12.0, bounds.top - x_head / 12.0));
        rect.setFillColor(sf::Color(255, 255, 255));

        //Создаем кнопки
        button = {
            sf::Text(L"20 - 40", font, 70),
            sf::Text(L"40 - 90", font, 70),
            sf::Text(L"90 - 150", font, 70),
        };

        float currentX = x_head;
        for (unsigned int i = 0; i < button.size(); i++)
        {
            button[i].setFillColor(sf::Color::White);
            button[i].setPosition(currentX, 3 * y_head);

            sf::FloatRect boundsP = button[i].getGlobalBounds();
            //далее заполняем параметры кнопок
            transition[i].num = i + 1;
            transition[i].x0 = boundsP.left;
            transition[i].y0 = boundsP.top;
            transition[i].x = boundsP.left + boundsP.width;
            transition[i].y = boundsP.top + boundsP.height;

            currentX += boundsP.width + 130.0f;
        }


        //Создание фона
        if (!texture.loadFromFile("F.jpg"))
            std::cout << "Ошибка при открытии файла F.jpg" << std::endl;

        sprite.setTexture(texture);
        sf::Vector2u tSize = texture.getSize();
        sf::Vector2u wSize = window.getSize();
        sprite.setScale(static_cast<float> (wSize.x) / tSize.x, static_cast<float> (wSize.y) / tSize.y);

    }

    void event_processing()
    {
        while (window.isOpen())
        {
            sf::Event event;
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed) window.close();
                if (event.type == sf::Event::MouseButtonPressed)
                {
                    if (event.mouseButton.button == sf::Mouse::Left)
                    {
                        for (auto& i : transition)
                        {
                            if (i.x0 <= event.mouseButton.x && event.mouseButton.x <= i.x && i.y0 <= event.mouseButton.y && event.mouseButton.y <= i.y)
                            {
                                window.close();
                            }
                        }
                    }
                }
            }
            window.clear();
            window.draw(sprite);
            window.draw(rect);
            window.draw(text);

            float startY = 100 + bounds.height + 40;
            for (unsigned int i = 0; i < button.size(); i++)
            {
                window.draw(button[i]);
            }
            window.display();
        }
    }
};

class General_window : public Window
{
private:
    sf::RenderWindow window{ sf::VideoMode(x_window, y_window), L"Визуализация полета астероидов" };
public:
    General_window() {}
    ~General_window() {}
 
    void event_processing(vector<Graphic_object*>& bodies)
    {
        while (window.isOpen())
        {
            sf::Event event;
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                    window.close();
            }

            window.clear();
            for (auto x : bodies)
            {
                x->Draw(window);
                x->Update();
            }
            window.display();
        }
    }
};
int main()
{
    std::ifstream file1("C:\\Users\\1\\Desktop\\project\\space_orbits\\data\\earth_orbit.csv");
    if (!file1.is_open()) { std::cout << "Не удалось открыть file1"; }

    std::ifstream file2("C:\\Users\\1\\Desktop\\project\\space_orbits\\data\\mars_orbit.csv");
    if (!file2.is_open()) { std::cout << "Не удалось открыть file2"; }
    
    vector<Graphic_object*> bodies;

    Earth earth(file1);
    bodies.push_back(&earth);
    Mars mars(file2);
    bodies.push_back(&mars);
    Sun sun;
    bodies.push_back(&sun);

    

    //Меню
    Menu menu;
    menu.event_processing();

    //Основное окно
    General_window g_window;
    g_window.event_processing(bodies);

}
