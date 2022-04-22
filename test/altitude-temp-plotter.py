from time import time
import matplotlib.pyplot as plt
import numpy as np

# CHANGE FILE LOCATION FOR YOUR FILE
dataList = np.genfromtxt('test/altitude.csv', delimiter=',', skip_header=1, usecols=(0, 1, 2, 3))

# Data for plotting
# https://stackoverflow.com/a/30244543
timeX = dataList[:, 0]
altitudeY1 = dataList[:, 3]
temperatureY2= dataList[:, 1]

# change time from millis to seconds 
# https://numpy.org/doc/stable/reference/arrays.nditer.html#modifying-array-values
with np.nditer(timeX, op_flags=['readwrite']) as time:
   for t in time:
       t[...] = t/1000

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
