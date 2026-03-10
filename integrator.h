//
// Created by ilia on 10-Mar-26.
// Класс с математикой, интегратор рк4
//

#pragma once
#include <functional>

//вектор состояния тела
//координаты и скорость
struct State{
    double x, y, vx, vy, t;
    //контсуткор со списком иницализации
    State(double x=0, double y=0, double vx=0, double vy=0, double t=0)
       : x(x), y(y), vx(vx), vy(vy), t(t) {}
};
//создаем шаблон функции
//возвращет стейт, ей нужен указатель на функциию
//принимает
typedef State (*Deriv)(const State&, double);

//рангекутта
//считаем 4 коэфциента с разными весами
class RK4{

    public:
        //статический метод вычисления состояния
        static State step(const State& s,  double dt, double t, Deriv deriv){
            // k1 = f(t, y)
            State k1 = deriv(s, t);

            // k2 = f(t + dt/2, y + (dt/2)*k1)
            State y2 = {
                s.x + dt/2 * k1.vx,
                s.y + dt/2 * k1.vy,
                s.vx + dt/2 * k1.x,
                s.vy + dt/2 * k1.y,
                t + dt/2
            };
            State k2 = deriv(y2, t + dt/2);

            // k3 = f(t + dt/2, y + (dt/2)*k2)
            State y3 = {
                s.x + dt/2 * k2.vx,
                s.y + dt/2 * k2.vy,
                s.vx + dt/2 * k2.x,
                s.vy + dt/2 * k2.y,
                t + dt/2
            };
            State k3 = deriv(y3, t + dt/2);

            // k4 = f(t + dt, y + dt*k3)
            State y4 = {
                s.x + dt * k3.vx,
                s.y + dt * k3.vy,
                s.vx + dt * k3.x,
                s.vy + dt * k3.y,
                t + dt
            };
            State k4 = deriv(y4, t + dt);

            // Комбинируем: new = y + (dt/6)*(k1 + 2k2 + 2k3 + k4)
            //и делаем новое сотояние
            State new_state;
            new_state.x  = s.x  + (dt/6) * (k1.vx + 2*k2.vx + 2*k3.vx + k4.vx);
            new_state.y  = s.y  + (dt/6) * (k1.vy + 2*k2.vy + 2*k3.vy + k4.vy);
            new_state.vx = s.vx + (dt/6) * (k1.x  + 2*k2.x  + 2*k3.x  + k4.x);
            new_state.vy = s.vy + (dt/6) * (k1.y  + 2*k2.y  + 2*k3.y  + k4.y);
            new_state.t  = t + dt;


            return new_state;
        }
};

