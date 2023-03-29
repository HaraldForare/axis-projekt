




#define PIN_TIMER_A_DATA_BUS_START 22
#define PIN_TIMER_B_DATA_BUS_START 34




//#define PIN_TIMER_A0_IS_LATCHED
//#define PIN_TIMER_A1_IS_LATCHED
//#define PIN_TIMER_B0_IS_LATCHED


#define PIN_CLEAR_TIMERS 4

#define PIN_CLK_OUTPUT 10



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
    /*
        We are using timer/counter 2.

        It is an 8-bit timer.


    

    */

    // Set timer to known state
    TCCR2A = 0;	TCCR2B = 0;	TCNT2 = 0;


    /*
    // Enable Fast PWM with OCR2A as top
    TCCR2A |= (1 <<  WGM21) | (1 <<  WGM20);
    TCCR2B |= (1 << WGM22);
    */

    // Enable CTC mode
    TCCR2A |= (1 << WGM21);


    // Enable pin toggle on max value
    TCCR2A |= (1 << COM2A0);


    // Output compare register at which the timer is reset and pin is toggled
    OCR2A = 0;
    OCR2A = 0xFF;  // Slower during development

    // Set proper pin to output
    pinMode(PIN_CLK_OUTPUT, OUTPUT);


    // Start timer with prescaler = 1
    TCCR2B |= 0b00000001;
    //TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20);  // Put prescaler = 1024 for debugging


}



void clear_timers() {
    digitalWrite(PIN_CLEAR_TIMERS, LOW);
    digitalWrite(PIN_CLEAR_TIMERS, HIGH);
}





void setup() {

    set_8_consecutive_pins_to_input(PIN_TIMER_A_DATA_BUS_START);
    set_8_consecutive_pins_to_input(PIN_TIMER_B_DATA_BUS_START);


    setup_timer_clock_source();

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

