#include <iostream>
#include <vector>
#include <cmath>
#include <functional>
#include <string>
using std::function;
using std::cout;
using std::endl;
using std::vector;
using std::sqrt;



class Space
{
protected:
	static const double Msun;
	static const double G;

public:
	Space() {};
	~Space() {};
};
const double Space::Msun = 1.98847e30;
const double Space::G = 6.67430e-11;



class Cosmic_bodies : public Space
{
private:
	double m; //масса тела
protected:
	struct Coordinate
	{
		double x, y, t;
	};
public:
	Cosmic_bodies(double m) //при создании космического тела нужно задать ему массу
	{
		this->m = m;
	}
	Cosmic_bodies() {}; //конструктор по умолчанию
	~Cosmic_bodies() {};
	double getMass() const
	{
		return m;
	}
	virtual function <Coordinate()> equation_of_motion() = 0;
};



class Gravity_field : public Space
{
private:
	struct Mass //описывает точечную массу в пространстве 
	{
		double x, y, mass;
		Mass(double x, double y, double m) : x(x), y(y), mass(m) {} //при создании объекта структуры нужно задать ее полям параметры 
	};
	struct Point //описывает точку пространства 
	{
		double gx, gy; //компоненты ускорения по осям
		double x, y; //координаты точки
		Point (double x, double y, double gx, double gy) : gx(gx), gy(gy), x(x), y(y) {} //задаем коррдинаты точки и ускорение по осям в этой точке 	
	}; 
	vector<Mass> masses;
public:
	Point calculating_field(double x, double y) //вычисляем поле в точках x, y
	{
		double total_gy = 0;
		double total_gx = 0;
		for (const Mass& m : masses) 
		{
			double rx = x - m.x;
			double ry = y - m.y;
			double r = sqrt(rx * rx + ry * ry);
			total_gx += G * m.mass * rx / (r*r*r);
			total_gy += G * m.mass * ry / (r * r * r);
		}
		return Point(x, y, total_gx, total_gy); //знак gx и gy говорит о направлении
	}

};



class Planet : public Cosmic_bodies
{
public:
	Planet() {}
	~Planet() {}
	function <Coordinate()> equation_of_motion() 
	{
		double r;
		double V;
		double dif_V = (Space::G) * (Space::Msun) / r;
		double dif_r = V;
	}
};



class Spacecraft : public Cosmic_bodies
{
private:
	double m;
public:
	Spacecraft(double m)
	{
		this->m = m;
	}
	~Spacecraft() {};
	function <Coordinate()> equation_of_motion()
	{

	}
};
