


if __name__ == "__main__":
    import numpy as np
    import math


    from sound_source import get_random_sound_source_position
    from mic_positions import get_mic_positions
    from arrival_time import get_arrival_time
    from localizer import localize

    def generate_test_case():
        source_position = get_random_sound_source_position(2, 5)

        source_angle = math.atan2(source_position[1], source_position[0])

        arrival_time = get_arrival_time(source_position, get_mic_positions())
        arrival_time -= np.min(arrival_time)


        loc = localize(get_mic_positions(), arrival_time)

        #rec_angle = math.atan2(loc[1], loc[0])


        mics = ["A0", "A1", "B0"]
        mics = list(zip(mics, arrival_time * 1E6))
        mics.sort(key = lambda m: m[1])


        diffs = np.diff([m[1] for m in mics])
        mics = [m[0] for m in mics]


        print(f"// Test case: {source_angle}")


        for n in range(len(mics)):
            if n:
                print(f"delayMicroseconds({int(round(diffs[n - 1]))});")

            print(f"digitalWrite(DEBUG_PIN_TRIGGER_{mics[n]}, HIGH);")

        print()
        [print(f"digitalWrite(DEBUG_PIN_TRIGGER_{mic}, LOW);") for mic in mics]



    generate_test_case()





