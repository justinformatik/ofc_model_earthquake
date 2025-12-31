import matplotlib.pyplot as plt
import numpy as np
from mpl_toolkits.mplot3d import Axes3D

fig = plt.figure(figsize = (10,8))
ax = fig.add_axes([0,0,1,1], projection='3d')
ax.text2D(0.35, 0.95, "Mersenne Twister", fontsize = 18, transform=ax.transAxes)
x, y, z = np.loadtxt("MersenneTwister.csv", delimiter=',', unpack=True)
ax.plot(x,y,z,'.')
plt.savefig('MT.png')
plt.show()
