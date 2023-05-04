

import numpy as np


def localize(mic_positions, arrival_times):
    arrival_times = arrival_times - np.mean(arrival_times)
    source_direction = -np.sum(np.column_stack((arrival_times, arrival_times)) * mic_positions, axis=0)
    source_direction /= np.linalg.norm(source_direction)
    return source_direction



