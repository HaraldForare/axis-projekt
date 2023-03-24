




#define PIN_TIMER_A_DATA_BUS_START 22
#define PIN_TIMER_B_DATA_BUS_START 34




//#define PIN_TIMER_A0_IS_LATCHED
//#define PIN_TIMER_A1_IS_LATCHED
//#define PIN_TIMER_B0_IS_LATCHED


#define PIN_CLEAR_TIMERS 4




void set_8_consecutive_pins_to_input(uint8_t first_pin) {
    for (uint8_t n = 0; n < 8; n += 1) {
        pinMode(first_pin + n, INPUT);
    }
}


uint8_t read_8_consecutive_pins_to_byte(uint8_t first_pin) {
    uint8_t output = 0;

    for (uint8_t n = 0; n < 8; n += 1) {
        if (digitalRead(first_pin + n)) {
            output |= (1 << n);
        }
    }

    return output;
}


void setup_timer_clock_source() {
    // TODO
    // Configure a PWM pin to output raw clock source (look in UC datasheet)
}



void clear_timers() {
    digitalWrite(PIN_CLEAR_TIMERS, LOW);
    digitalWrite(PIN_CLEAR_TIMERS, HIGH);
}





void setup() {

    set_8_consecutive_pins_to_input(PIN_TIMER_A_DATA_BUS_START);
    set_8_consecutive_pins_to_input(PIN_TIMER_B_DATA_BUS_START);



    pinMode(PIN_CLEAR_TIMERS, OUTPUT);
    clear_timers();

}



void loop() {


    /*
    
    * Reset timers
    * Sound peaks will cause the respective timers to start counting
    * Microcontroller will monitor the internal latch states and determine
      if triggering was a fluke or if it actually was a real event.

    * Providing it was a real event. The microcontroller will stop all timers
      at the same time. Timers are stopped by turning off their clock source

    * Timers are read
    
    
    */



}

