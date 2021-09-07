import matplotlib.pyplot as plt
import numpy as np

dataList = np.genfromtxt('test/example.csv', delimiter=',', skip_header=1, usecols=(0, 1, 2, 3, 4))

# Data for plotting
# https://stackoverflow.com/a/30244543
timeX = dataList[:, 0:1]
altitudeY1 = dataList[:, 4:5]
temperatureY2= dataList[:, 1:2]

# Make lines
fig, ax1 = plt.subplots()
ax2 = ax1.twinx()

# Draw Lines
# https://matplotlib.org/stable/api/_as_gen/matplotlib.pyplot.plot.html
ax1.plot(timeX, altitudeY1, '-g')
ax2.plot(timeX, temperatureY2, '-r')

# Add x label
ax1.set_xlabel('time (s)')

# Make the y-axis label, ticks and tick labels match the line color.
ax1.set_ylabel('Altitude (m)', color='g')
ax1.tick_params('y', colors='g')

# Make the y-axis label, ticks and tick labels match the line color.
ax2.set_ylabel('Temperature (C)', color='r')
ax2.tick_params('y', colors='r')

fig.tight_layout()
plt.show()
