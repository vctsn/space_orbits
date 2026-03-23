import matplotlib.pyplot as plt
import pandas as pd

# Читаем данные
earth = pd.read_csv('build/earth_orbit.csv')
mars = pd.read_csv('build/mars_orbit.csv')

# Рисуем
plt.figure(figsize=(10, 10))
plt.plot(earth['x'], earth['y'], 'b-', label='Земля', linewidth=1)
plt.plot(mars['x'], mars['y'], 'r-', label='Марс', linewidth=1)
plt.plot(0, 0, 'y*', markersize=20, label='Солнце')
plt.xlabel('x (м)')
plt.ylabel('y (м)')
plt.title('Орбиты Земли и Марса')
plt.grid(True, alpha=0.3)
plt.legend()
plt.axis('equal')
plt.show()