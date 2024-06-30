import numpy as np
from matplotlib import pyplot as plt

r = np.array([1.0, -2.0])
angle = np.pi / 180 * 340
v = np.array([np.cos(angle), np.sin(angle)])

a = np.array([1, 4])

e = (a @ v)/(v @ v) * v
er = (r @ v)/(v @ v) * v

print(np.atan2(v[1], v[0]) * 180 / np.pi)
print(np.atan2(e[1], e[0]) * 180 / np.pi)

plt.plot([0, r[0]], [0, r[1]], label="r")
plt.plot([r[0] - v[0], r[0] + v[0]], [r[1] - v[1], r[1] + v[1]], label="v offset")
plt.plot([0, a[0]], [0, a[1]], label="a")
plt.plot([0, e[0]], [0, e[1]], label="e")

plt.plot([0, a[0] - e[0]], [0, a[1] - e[1]], label="e2")
plt.plot([0, r[0] - er[0]], [0, r[1] - er[1]], label="er")

plt.xlim(-5, 5)
plt.ylim(-5, 5)
plt.grid(True)
plt.gca().set_aspect("equal")

plt.legend()

plt.show()

