

import numpy as np


speed_of_sound_mps = 343


mic_radius = 0.01

angles = np.linspace(0, 4/3*np.pi, 3)

mic_positions = mic_radius * np.column_stack((np.cos(angles), np.sin(angles)))



source_location = np.array([5, 9])


arrival_times = list()
for (mic_x, mic_y) in mic_positions:
    arrival_times.append(
        ((source_location[0] - mic_x)**2 + (source_location[1] - mic_y)**2)**0.5 / speed_of_sound_mps
    )

    del mic_x, mic_y

arrival_times = np.array(arrival_times)


avg_arrival_time = np.mean(arrival_times)

#arrival_time_differences = arrival_times - avg_arrival_time

arrival_time_vectors = arrival_times - avg_arrival_time


sum_ = np.sum(np.abs(arrival_time_vectors) * np.e**(1j*angles))


breakpoint()


