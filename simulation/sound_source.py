


import random
import math
import numpy as np


def get_random_sound_source_position(min_distance: float = 2.0, max_distance: float = 50.0):
    angle = random.random() * 2*math.pi
    distance = random.random() * (max_distance**2 - min_distance**2) + min_distance**2
    distance = distance**0.5
    return np.array((distance * math.cos(angle), distance * math.sin(angle)))







if __name__ == "__main__":

    X = list()
    Y = list()

    for _ in range(50000):
        x, y = get_random_sound_source_position(5, 30)
        X.append(x)
        Y.append(y)


    import matplotlib.pyplot as plt
    plt.scatter(X, Y, marker=".", s=1)
    plt.grid(True)
    plt.show()



