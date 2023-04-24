
/*
#include <BasicLinearAlgebra.h>
#include <ElementStorage.h>
using namespace BLA;
*/

#include <Arduino.h>

#define SERIAL_BAUD_RATE 115200

#define STARTUP_POWER_STABILIZATION_DELAY_MILLIS 200

#define PI 3.141592653589793238462643383279502884

#define PIN_COUNTER_A_DATA_BUS_START 22
#define PIN_COUNTER_B_DATA_BUS_START 34

#define PIN_COUNTER_A0_TRIGGERED 6
#define PIN_COUNTER_A1_TRIGGERED 7
#define PIN_COUNTER_B0_TRIGGERED 8

#define PIN_CLEAR_COUNTERS 4
#define PIN_ARM_COUNTERS 5
#define PIN_SAVE_COUNTERS 2

#define PIN_CLK_OUTPUT 10

#define PIN_COUNTERS_GAL 44
#define PIN_COUNTERS_GAU 45
#define PIN_COUNTERS_GBL 46
#define PIN_COUNTERS_GBU 47

#define DEBUG_PIN_TRIGGER_A0 50
#define DEBUG_PIN_TRIGGER_A1 51
#define DEBUG_PIN_TRIGGER_B0 52

// Set this to the time it takes for sound to pass through all microphones (t = s / v)
// Max distance between two microphones
// Exact value will be acquired from CAD (+ a small safety margin)
#define FLUKE_SIGNAL_REJECTION_TIME_MICROS 800
#define ECHO_DISSIPATION_COOLDOWN_MILLIS 200

void set_n_consecutive_pins_to_input(uint8_t n, uint8_t first_pin)
{
    for (uint8_t i = 0; i < n; i += 1)
    {
        pinMode(first_pin + i, INPUT);
    }
}

void set_n_consecutive_pins_to_output(uint8_t n, uint8_t first_pin)
{
    for (uint8_t i = 0; i < n; i += 1)
    {
        pinMode(first_pin + i, OUTPUT);
    }
}

uint8_t read_8_consecutive_pins_to_byte(uint8_t first_pin)
{
    uint8_t output = 0;

    for (uint8_t n = 0; n < 8; n += 1)
    {
        if (digitalRead(first_pin + n))
        {
            output |= (1 << n);
        }
    }

    return output;
}

void setup_counter_clock_source()
{

    // Set counter to known state
    TCCR2A = 0;
    TCCR2B = 0;
    TCNT2 = 0;

    // Enable CTC mode
    TCCR2A |= (1 << WGM21);

    // Enable pin toggle on max value
    TCCR2A |= (1 << COM2A0);

    // Output compare register at which the counter is reset and pin is toggled
    OCR2A = 0;
    OCR2A = 0xFF; // Slower during development

    // Start counter with prescaler = 1
    TCCR2B |= 0b00000001;
}

void DEBUG_pulse_clock_source(uint64_t cycles)
{
    for (uint64_t n = 0; n < cycles; n += 1)
    {
        digitalWrite(PIN_CLK_OUTPUT, HIGH);
        digitalWrite(PIN_CLK_OUTPUT, LOW);
    }
}

void clear_counters()
{
    digitalWrite(PIN_CLEAR_COUNTERS, LOW);
    digitalWrite(PIN_CLEAR_COUNTERS, HIGH);
}

uint8_t get_counters_trigger_state()
{
    uint8_t output = 0;
    if (digitalRead(PIN_COUNTER_A0_TRIGGERED))
    {
        output &= 0b00000001;
    }

    if (digitalRead(PIN_COUNTER_A1_TRIGGERED))
    {
        output &= 0b00000010;
    }

    if (digitalRead(PIN_COUNTER_B0_TRIGGERED))
    {
        output &= 0b00000100;
    }

    return output;
}

class ReadCounters
{
public:
    uint16_t count_A0;
    uint16_t count_A1;
    uint16_t count_B0;

    ReadCounters()
    {

        // Save counter states to their internal registers (freeze count)
        digitalWrite(PIN_SAVE_COUNTERS, HIGH);

        // Start reading data
        digitalWrite(PIN_COUNTERS_GAL, LOW);
        uint16_t counter_A0_lower_byte = read_8_consecutive_pins_to_byte(PIN_COUNTER_A_DATA_BUS_START);
        uint16_t counter_B0_lower_byte = read_8_consecutive_pins_to_byte(PIN_COUNTER_B_DATA_BUS_START);
        digitalWrite(PIN_COUNTERS_GAL, HIGH);

        digitalWrite(PIN_COUNTERS_GAU, LOW);
        uint16_t counter_A0_upper_byte = read_8_consecutive_pins_to_byte(PIN_COUNTER_A_DATA_BUS_START);
        uint16_t counter_B0_upper_byte = read_8_consecutive_pins_to_byte(PIN_COUNTER_B_DATA_BUS_START);
        digitalWrite(PIN_COUNTERS_GAU, HIGH);

        digitalWrite(PIN_COUNTERS_GBL, LOW);
        uint16_t counter_A1_lower_byte = read_8_consecutive_pins_to_byte(PIN_COUNTER_A_DATA_BUS_START);
        digitalWrite(PIN_COUNTERS_GBL, HIGH);

        digitalWrite(PIN_COUNTERS_GBU, LOW);
        uint16_t counter_A1_upper_byte = read_8_consecutive_pins_to_byte(PIN_COUNTER_A_DATA_BUS_START);
        digitalWrite(PIN_COUNTERS_GBU, HIGH);

        /*Serial.print("A0: ");
        DEBUG_print_byte(counter_A0_upper_byte);
        DEBUG_print_byte(counter_A0_lower_byte);
        Serial.print("\n");*/

        this->count_A0 = (counter_A0_upper_byte << 8) | counter_A0_lower_byte;
        this->count_A1 = (counter_A1_upper_byte << 8) | counter_A1_lower_byte;
        this->count_B0 = (counter_B0_upper_byte << 8) | counter_B0_lower_byte;

        digitalWrite(PIN_SAVE_COUNTERS, LOW);
    }

    void DEBUG_print()
    {
        Serial.print("Counts:\n    A0: ");
        Serial.print(this->count_A0);
        Serial.print("\n    A1: ");
        Serial.print(this->count_A1);
        Serial.print("\n    B0: ");
        Serial.print(this->count_B0);
        Serial.print("\n");
    }
};

void DEBUG_print_bit(uint8_t bit)
{
    if (bit)
    {
        Serial.print("1");
    }
    else
    {
        Serial.print("0");
    }
}

void DEBUG_print_byte(uint8_t byte)
{
    for (int8_t n = 7; n >= 0; n -= 1)
    {
        DEBUG_print_bit(byte & (1 << n));
    }
}

void DEBUG_print_io()
{

    Serial.println("======[ DEBUG IO ]======");

    Serial.print("Counter data buses\n    A: ");
    DEBUG_print_byte(read_8_consecutive_pins_to_byte(PIN_COUNTER_A_DATA_BUS_START));
    Serial.print("\n    B: ");
    DEBUG_print_byte(read_8_consecutive_pins_to_byte(PIN_COUNTER_B_DATA_BUS_START));
    Serial.print("\n\n");

    Serial.print("Counting triggered\n    A0: ");
    DEBUG_print_bit(digitalRead(PIN_COUNTER_A0_TRIGGERED));
    Serial.print("\n    A1: ");
    DEBUG_print_bit(digitalRead(PIN_COUNTER_A1_TRIGGERED));
    Serial.print("\n    B0: ");
    DEBUG_print_bit(digitalRead(PIN_COUNTER_B0_TRIGGERED));
    Serial.print("\n\n");

    Serial.print("Counting trigger\n    A0: ");
    DEBUG_print_bit(digitalRead(DEBUG_PIN_TRIGGER_A0));
    Serial.print("\n    A1: ");
    DEBUG_print_bit(digitalRead(DEBUG_PIN_TRIGGER_A1));
    Serial.print("\n    B0: ");
    DEBUG_print_bit(digitalRead(DEBUG_PIN_TRIGGER_B0));
    Serial.print("\n\n");

    Serial.print("Counter control\n    CLEAR: ");
    DEBUG_print_bit(digitalRead(PIN_CLEAR_COUNTERS));
    Serial.print("\n    ARM:   ");
    DEBUG_print_bit(digitalRead(PIN_ARM_COUNTERS));
    Serial.print("\n    SAVE:  ");
    DEBUG_print_bit(digitalRead(PIN_SAVE_COUNTERS));
    Serial.print("\n\n");

    Serial.print("Clock\n    SOURCE: ");
    DEBUG_print_bit(digitalRead(PIN_CLK_OUTPUT));
    Serial.print("\n\n");

    Serial.print("========================\n\n");
}

void setup()
{
    delay(STARTUP_POWER_STABILIZATION_DELAY_MILLIS);
    Serial.begin(SERIAL_BAUD_RATE);

    set_n_consecutive_pins_to_input(8, PIN_COUNTER_A_DATA_BUS_START);
    set_n_consecutive_pins_to_input(8, PIN_COUNTER_B_DATA_BUS_START);

    digitalWrite(PIN_COUNTERS_GAL, HIGH);
    digitalWrite(PIN_COUNTERS_GAU, HIGH);
    digitalWrite(PIN_COUNTERS_GBL, HIGH);
    digitalWrite(PIN_COUNTERS_GBU, HIGH);

    pinMode(PIN_COUNTERS_GAL, OUTPUT);
    pinMode(PIN_COUNTERS_GAU, OUTPUT);
    pinMode(PIN_COUNTERS_GBL, OUTPUT);
    pinMode(PIN_COUNTERS_GBU, OUTPUT);

    pinMode(DEBUG_PIN_TRIGGER_A0, OUTPUT);
    pinMode(DEBUG_PIN_TRIGGER_A1, OUTPUT);
    pinMode(DEBUG_PIN_TRIGGER_B0, OUTPUT);

    // setup_counter_clock_source();
    pinMode(PIN_CLK_OUTPUT, OUTPUT);

    pinMode(PIN_ARM_COUNTERS, OUTPUT);
    digitalWrite(PIN_ARM_COUNTERS, LOW);

    pinMode(PIN_CLEAR_COUNTERS, OUTPUT);
    digitalWrite(PIN_CLEAR_COUNTERS, HIGH);

    pinMode(PIN_SAVE_COUNTERS, OUTPUT);
    digitalWrite(PIN_SAVE_COUNTERS, LOW);

    ReadCounters read_counters(); // Initialize counters properly
    // read_counters.DEBUG_print();

    digitalWrite(PIN_ARM_COUNTERS, HIGH);

    Serial.println("Setup finished");
}

void loop()
{
    // TODO: Reset counters and re-arm

    // Wait for any counter to become active
    /*    if (!get_counters_trigger_state())
        {
            return;
        }

        uint64_t trigger_time = micros();

        Serial.println("First pulse detected!");

        while (true)
        {
            if (micros() >= (trigger_time + FLUKE_SIGNAL_REJECTION_TIME_MICROS))
            {
                Serial.println("Timed out while waiting for the other two pulses");
                return;
            }

            if (get_counters_trigger_state() & 0b00000111)
            {
                Serial.println("Got three pulses!");
                break;
            }
        }
        */
    ReadCounters read_counters = ReadCounters();

    /*    double arrival_times[3] = {
            (double)read_counters.count_A0,
            (double)read_counters.count_A1,
            (double)read_counters.count_B0};*/

    // ONLY FOR DEBUG!!!!
    double arrival_times[3] = {
        0.0,
        10160.0,
        10160.0};

    {
        double mean_arrival_time = 0.0;
        for (uint8_t n = 0; n < 3; n += 1)
        {
            mean_arrival_time += arrival_times[n];
        }

        mean_arrival_time /= 3.0;

        for (uint8_t n = 0; n < 3; n += 1)
        {
            arrival_times[n] -= mean_arrival_time;
        }
    }

    // Make sure atleast one arrival time is different, otherwise we get divide by zero and shouldn't continue
    {
        double max_difference = 0.0;
        for (uint8_t n = 0; n < 3; n += 1)
        {
            for (uint8_t k = 0; k < 3; k += 1)
            {
                if (n != k)
                {
                    double difference = abs(arrival_times[n] - arrival_times[k]);
                    if (difference > max_difference)
                    {
                        max_difference = difference;
                    }
                }
            }
        }

        if (max_difference <= 0.0)
        {
            Serial.print("Max difference between numbers was only ");
            Serial.print(max_difference);
            Serial.print(", which is too small. Aborting here to avoid divide by zero \n");
            return;
        }
    }

    double mic_vectors[3][2];
    for (uint8_t n = 0; n < 3; n += 1)
    {
        double angle = ((double)n) * 2.0 / 3.0 * PI;
        mic_vectors[n][0] = cos(angle);
        mic_vectors[n][1] = sin(angle);
    }

    double projections[3][2];
    for (uint8_t n = 0; n < 3; n += 1)
    {
        for (uint8_t k = 0; k < 2; k += 1)
        {
            projections[n][k] = arrival_times[n] * mic_vectors[n][k];
        }
    }

    double source_direction[2] = {0.0, 0.0};
    for (uint8_t n = 0; n < 3; n += 1)
    {
        for (uint8_t k = 0; k < 2; k += 1)
        {
            source_direction[k] += projections[n][k];
        }
    }
    double angle;
    {
        double norm = sqrt(sq(source_direction[0]) + sq(source_direction[1]));
        source_direction[0] /= norm;
        source_direction[1] /= norm;
        // Serial.println(source_direction[0]);
        // Serial.println(source_direction[1]);
        angle = atan2(source_direction[1], source_direction[0]);
    }
    /*Serial.print("Sound direction: {x: ");
    Serial.print(source_direction[0]);
    Serial.print(", y: ");
    Serial.print(source_direction[1]);
    Serial.print("}\n");
    Serial.println(angle + PI);*/

    // Send angle to be read by raspberry PI via USB
    // Serial.print("Angle: ");
    Serial.println(angle + PI);

    delay(ECHO_DISSIPATION_COOLDOWN_MILLIS);
    delay(10000);
}
