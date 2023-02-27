


import numpy as np


def get_mic_positions(radius: float = 0.1):
    angles = np.linspace(0, 4/3*np.pi, 3)
    return radius*np.column_stack((np.cos(angles), np.sin(angles)))






if __name__ == "__main__":

    mic_positions = get_mic_positions()

    import matplotlib.pyplot as plt

    plt.scatter(mic_positions[:, 0], mic_positions[:, 1])
    plt.grid(True)
    plt.show()




