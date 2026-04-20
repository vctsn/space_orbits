Команда для сборки всего чуда

$

cd ..
g++ -o build/simulation main.cpp engine/engine.cpp physics/physics.cpp
cd build/
./simulation
$
и чудо графики  g++ -o build/visualisation   graphics.cpp  -lsfml-graphics -lsfml-window -lsfml-system
