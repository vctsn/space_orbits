import matplotlib.pyplot as plt
import pandas as pd

# Читаем данные из общего файла
data = pd.read_csv('data/simulation.csv')

# Рисуем
plt.figure(figsize=(12, 12))

# Список планет
planets = ['Mercury', 'Venus', 'Earth', 'Mars', 'Jupiter', 'Saturn', 'Uranus', 'Neptune', "sat"]
colors = ['gray', 'orange', 'blue', 'red', 'brown', 'gold', 'lightblue', 'darkblue', 'black']

# Рисуем орбиту каждой планеты
for planet, color in zip(planets, colors):
    plt.plot(data[f'{planet}_x'], data[f'{planet}_y'], 
             color=color, label=planet, linewidth=1)

# Солнце
plt.plot(0, 0, 'y*', markersize=20, label='Солнце')

plt.xlabel('x (млн км)')
plt.ylabel('y (млн км)')
plt.title('Орбиты планет Солнечной системы')
plt.grid(True, alpha=0.3)
plt.legend()
plt.axis('equal')
plt.show()