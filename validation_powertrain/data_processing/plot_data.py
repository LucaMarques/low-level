import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

database = pd.read_csv('temp15.csv')

plt.figure(1)
plt.title('Temperature')
plt.xlabel('Time (ms)')
plt.ylabel('Degrees')
plt.plot(database['time'], database['temp_obj1'], label='temp obj 1')
plt.plot(database['time'], database['temp_amb1'], label='temp amb 1')
plt.plot(database['time'], database['temp_obj2'], label='temp obj 2')
plt.plot(database['time'], database['temp_amb2'], label='temp amb 2')
plt.legend()
plt.grid()

plt.xlim([database['time'][0], database['time'].iloc[-1]])
plt.ylim([0, 30])
plt.show()