

import math
import random
import numpy as np


from arrival_time import get_arrival_time
from mic_positions import get_mic_positions
from sound_source import get_random_sound_source_position
from localizer import localize



def array_to_complex(array):
    assert len(array) == 2
    return array[0] + 1j*array[1]


mic_positions = get_mic_positions()


clock_time = 1/16E6


time_errors = list()
angle_errors = list()


for _ in range(10000):
    source_position = get_random_sound_source_position(2, 5)
    mic_arrival_times = get_arrival_time(source_position, mic_positions)

    time_error_std = random.random() * 500 * clock_time
    time_error = np.random.normal(scale=time_error_std, size=len(mic_arrival_times))
    mic_arrival_times += time_error

    retrieved_direction = array_to_complex(localize(mic_positions, mic_arrival_times))
    true_direction = array_to_complex(source_position / np.linalg.norm(source_position))

    retrieved_angle = np.angle(retrieved_direction)
    true_angle = np.angle(true_direction)

    diff = np.unwrap(np.array((retrieved_angle, true_angle)))
    angle_error = math.fabs(diff[0] - diff[1])
    angle_error = angle_error / (2*np.pi) * 360

    time_errors.append(time_error)
    angle_errors.append(angle_error)


time_errors = np.stack(time_errors)
#abs_time_errors = np.sqrt(np.sum(np.square(time_errors), axis=1))
avg_abs_time_error = np.mean(np.abs(time_errors), axis=1)


angle_errors = np.array(angle_errors)




import matplotlib.pyplot as plt

plt.scatter(avg_abs_time_error / clock_time, angle_errors, s=1)
plt.xlabel(f"Total absolute time error [clock cycles @ {1E-6/clock_time} MHz]")
plt.ylabel("Angle error [degrees]")
plt.grid(True)
plt.show()


#breakpoint()





