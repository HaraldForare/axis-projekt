

import numpy as np


from arrival_time import get_arrival_time
from mic_positions import get_mic_positions
from sound_source import get_random_sound_source_position
from localizer import localize


source_position = get_random_sound_source_position(2, 5)
mic_positions = get_mic_positions()


mic_arrival_times = get_arrival_time(source_position, mic_positions)

source_direction = localize(mic_positions, mic_arrival_times)



if __name__ == "__main__":

    import matplotlib.pyplot as plt

    plt.scatter(mic_positions[:, 0], mic_positions[:, 1])
    plt.scatter(source_position[0], source_position[1], marker="X")
    plt.plot([0, source_direction[0]], [0, source_direction[1]])

    plt.gca().set_aspect(1)
    plt.grid(True)
    plt.show()



