#include <avr/io.h>
#include <util/atomic.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void irled_init(void);
volatile uint32_t timer = 0;
volatile uint8_t check_me = 0;

int main( void ) {
	// Sensor 1
	DDRB &= ~(1<<PB0);
	// Sensor 2
	DDRB &= ~(1<<PB1);

	DDRA |= (1<<PA0)|(1<<PA1);

	irled_init();
	sei();

	uint8_t sen0_count_high = 0;
	uint8_t sen0_broken = 0;

	uint8_t sen1_count_high = 0;
	uint8_t sen1_broken = 0;

	uint8_t last_state = 0;
	uint8_t current_state = 0;

	uint8_t led_event = 0;
	uint8_t wait_for_reset = 0;

	#define MAX_CYCLE_COUNT 20
	#define BROKEN_THRESHOLD 15
	uint8_t cycle = 0;

	uint32_t last_time_sensor_check = 0;
	uint32_t local_now;

	while (1) {
		/*
			PORTA ^= (1<<PA0);
			_delay_ms(100);
			PORTA ^= (1<<PA1);
			_delay_ms(100);
		*/

		ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { local_now = timer; }
		while (check_me) {
			led_event = 1;
			// Check sensor 0
				if ( PINB & (1<<PB0)) {
					// Sensor 0 active
					sen0_count_high++;
				}
				// Check sensor 1
				if ( PINB & (1<<PB1)) {
					// Sensor 1 active
					sen1_count_high++;
				}
				_delay_us(10);
		}

		if(led_event) {
			led_event = 0;
			if (sen0_count_high >= 20) {
				sen0_broken = 1;
			} else {
				sen0_broken = 0;
			}
			
			if (sen1_count_high >= 20) {
				sen1_broken = 1;
			} else {
				sen1_broken = 0;
			}

			if (wait_for_reset && sen0_broken == 0 && sen1_broken == 0) {
				wait_for_reset = 0;
			}
		}

		// Set current step
		if (!wait_for_reset) {
			if (sen0_broken == 1 && sen1_broken == 0) {
				current_state = 64;
			} else if (sen0_broken == 0 && sen1_broken == 1) {
				current_state = 32;
			} else if (sen0_broken == 1 && sen1_broken == 1) {
				current_state = 96;
			} else {
				current_state = 0;
			}
		}

		if (last_state == current_state) {
			//Nothing happens
		} else if (last_state == 0 && current_state == 64 && !wait_for_reset) {
			//Somebody walks in (from 0|0 to 1|0)
			PORTA ^= (1<<PA0);
			_delay_ms(300);
			wait_for_reset = 1;
		} else if (last_state == 0 && current_state == 32 && !wait_for_reset) {
			//Somebody walks out (from 0|0 to 0|1)
			PORTA ^= (1<<PA1);
			_delay_ms(300);
			wait_for_reset = 1;
		}

		if (last_state != current_state) { last_state = current_state; }

		sen0_count_high = 0;
		sen1_count_high = 0;


/*
			if (last_time_sensor_check != local_now) {
				// Check sensor 0
				if ( PINB & (1<<PB0)) {
					// Sensor 0 active
					sen0_count_high++;
				}

				// Check sensor 1
				if ( PINB & (1<<PB1)) {
					// Sensor 1 active
					sen1_count_high++;
				}

				if (cycle >= MAX_CYCLE_COUNT) {
					if (sen1_count_high >= BROKEN_THRESHOLD) {
						// Sensor was at least 15 out of 20 samples on high
						sen1_broken = 1;
					} else {
						sen1_broken = 0;
					}
					sen1_count_high = 0;

					if (sen0_count_high >= BROKEN_THRESHOLD) {
						// Sensor was at least 15 out of 20 samples on high
						sen0_broken = 1;
					} else {
						sen0_broken = 0;
					}
					sen0_count_high = 0;

					// Set current step
					if (sen0_broken == 1 && sen1_broken == 0) {
						current_state = 64;
					} else if (sen0_broken == 0 && sen1_broken == 1) {
						current_state = 32;
					} else if (sen0_broken == 1 && sen1_broken == 1) {
						current_state = 96;
					} else {
						current_state = 0;
					}

					if (last_state == current_state) {
						//Nothing happens
					} else if (last_state == 0 && current_state == 64) {
						//Somebody walks in (from 0|0 to 1|0)
						PORTA ^= (1<<PA0);
					} else if (last_state == 0 && current_state == 32) {
						//Somebody walks out (from 0|0 to 0|1)
						PORTA ^= (1<<PA1);
					}

					if (last_state != current_state) { last_state = current_state; }

					cycle = 0;
				}

				last_time_sensor_check = local_now;
			}
		}
*/
	}
}

ISR( TIM0_COMPA_vect ) {
	static uint8_t counter = 0;
	if (counter <= 23*2) {
		if (counter >= 4*2) {
			check_me = 1;
		}
		PORTA ^= (1<<PA7);
		counter++;
	} else {
		check_me = 0;
		PORTA &= ~(1<<PA7);
		if (counter == 42*2) {
			counter = 0;
			// global timer increment
			timer++;
		} else {
			counter++;
		}
	}

	return;
}

void irled_init( void ) {
	DDRA |= (1<<PA7);
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		TCCR0A = (1<<WGM01);
		TCCR0B = (1<<CS00);
		OCR0A = 104;

		TIMSK0 |= (1<<OCIE0A);
	}
}
