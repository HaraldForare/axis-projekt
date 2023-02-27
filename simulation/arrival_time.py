

import numpy as np


def get_arrival_time(source_position, mic_positions, speed_of_sound = 343):
    return np.sqrt(np.sum(np.square(source_position - mic_positions), axis=1)) / speed_of_sound


