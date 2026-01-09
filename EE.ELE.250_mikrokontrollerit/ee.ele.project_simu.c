#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdint.h>

#define MIN_FREQ 50
#define MAX_FREQ 1000

#define SOUND_PIN PB1
#define LED_PIN   PD3
#define SWITCH_PIN PD2

#define SWITCH_IS_ON (!(PIND & (1<<SWITCH_PIN)))

// UART variables
volatile int16_t uart_adj = 0;       // fine adjustment via +/-
volatile int16_t uart_set_value = 0; // exact frequency set via UART
volatile uint8_t uart_updated = 0;

// Frequency and previous ADC value
volatile int16_t frequency = MIN_FREQ;
volatile int16_t adc_old = 0;

// ---------------------------------------------------------
// UART initialization
// ---------------------------------------------------------
void UART_Init(void) {
	uint16_t ubrr = F_CPU/16/9600-1;
	UBRR0H = ubrr>>8;
	UBRR0L = ubrr;
	UCSR0B = (1<<TXEN0)|(1<<RXEN0)|(1<<RXCIE0);
	UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);
}

void UART_SendText(const char *s) {
	while(*s) {
		while(!(UCSR0A & (1<<UDRE0)));
		UDR0 = *s++;
	}
}

// ---------------------------------------------------------
// UART interrupt for fine adjustment (+/-) and exact frequency input
// ---------------------------------------------------------
ISR(USART_RX_vect) {
	static char buf[6];
	static uint8_t idx = 0;

	char c = UDR0;

	// Fine adjustment always applies
	if(c == '+') uart_adj += 10;
	else if(c == '-') uart_adj -= 10;

	// Number input for exact frequency
	else if(c >= '0' && c <= '9') {
		if(idx < sizeof(buf)-1) {
			buf[idx++] = c;
			buf[idx] = 0; // null terminate
		}
	}
	// Enter key signals end of input
	else if(c == '\r' || c == '\n') {
		uart_set_value = atoi(buf); // convert string to integer
		if(uart_set_value < MIN_FREQ) uart_set_value = MIN_FREQ;
		if(uart_set_value > MAX_FREQ) uart_set_value = MAX_FREQ;
		uart_updated = 1;
		idx = 0;
		buf[0] = 0;
	}
	else { // ignore other characters
		idx = 0;
		buf[0] = 0;
	}
}

// ---------------------------------------------------------
// ADC initialization and read
// ---------------------------------------------------------
void ADC_Init(void) {
	ADMUX = (1<<REFS0); // AVcc as reference
	ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0); // prescaler 128
}

uint16_t ADC_Read(void) {
	ADCSRA |= (1<<ADSC);
	while(ADCSRA & (1<<ADSC));
	return ADC;
}

// ---------------------------------------------------------
// Sound generation using Timer1
// ---------------------------------------------------------
void Sound_SetFreq(uint16_t f) {
	if(f < MIN_FREQ) f = MIN_FREQ;
	if(f > MAX_FREQ) f = MAX_FREQ;

	uint32_t ocr = F_CPU / (2UL * 8 * f) - 1;
	if(ocr > 0xFFFF) ocr = 0xFFFF;

	OCR1A = (uint16_t)ocr;
	TCCR1A = (1<<COM1A0);
	TCCR1B = (1<<WGM12)|(1<<CS11);
}

void Sound_Off(void) { TCCR1B = 0; }

// ---------------------------------------------------------
// LED PWM using Timer2
// ---------------------------------------------------------
void LED_PWM_Init(void) {
	TCCR2A = (1<<WGM21)|(1<<WGM20)|(1<<COM2B1);
	TCCR2B = (1<<CS22);
	OCR2B = 0;
}

// ---------------------------------------------------------
// External interrupt
// ---------------------------------------------------------
ISR(INT0_vect) {}

// ---------------------------------------------------------
// Power down function
// ---------------------------------------------------------
void power_low(void) {
	EICRA &= ~((1<<ISC01)|(1<<ISC00));
	EIFR = (1<<INTF0);
	EIMSK |= (1<<INT0);

	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sleep_enable();
	sei();
	sleep_cpu();

	sleep_disable();
	EIMSK &= ~(1<<INT0);
}

// ---------------------------------------------------------
// Main loop
// ---------------------------------------------------------
int main(void) {
	// Pin setup
	DDRB |= (1<<SOUND_PIN);
	DDRD |= (1<<LED_PIN);
	DDRD &= ~(1<<SWITCH_PIN);
	PORTD |= (1<<SWITCH_PIN);

	UART_Init();
	ADC_Init();
	LED_PWM_Init();
	sei();

	Sound_Off();
	OCR2B = 0;

	uint16_t timer_ms = 0;
	uint8_t led_level = 0;
	int8_t led_dir = 1;

	UART_SendText("UART OK\r\n");

	while(1) {
		// ---------------- Power off if switch released ----------------
		if(!SWITCH_IS_ON) {
			Sound_Off();
			OCR2B = 0;
			_delay_ms(10);
			power_low();
			_delay_ms(50);
			continue;
		}

		// ---------------- Read ADC ----------------
		int16_t adc_value = ADC_Read();
		int16_t adc_freq = (int32_t)adc_value * (MAX_FREQ-MIN_FREQ)/1023 + MIN_FREQ;

		// ---------------- Determine base frequency ----------------
		int16_t base_freq = adc_freq;

		// UART exact frequency has priority if potentiometer not moved significantly
		if(uart_updated) {
			if(abs(adc_freq - adc_old) <= 3) {
				base_freq = uart_set_value;
				} else {
				uart_updated = 0; // ADC takes over if potentiometer moved
			}
		}

		adc_old = adc_freq;

		// ---------------- Apply fine adjustment (+/-) on top of base ----------------
		frequency = base_freq + uart_adj;

		// Clamp frequency
		if(frequency < MIN_FREQ) frequency = MIN_FREQ;
		if(frequency > MAX_FREQ) frequency = MAX_FREQ;

		// ---------------- Apply sound ----------------
		Sound_SetFreq(frequency);

		// ---------------- LED fading blinking effect ----------------
		led_level += led_dir;
		if(led_level == 255 || led_level == 0) led_dir = -led_dir;
		OCR2B = led_level;

		// ---------------- Periodic UART output ----------------
		timer_ms += 10;
		if(timer_ms >= 2000) {
			char buf[16];
			itoa(frequency, buf, 10);
			UART_SendText("\r\nfrequency is ");
			UART_SendText(buf);
			UART_SendText(" Hz\r\n");
			timer_ms = 0;
		}

		_delay_ms(10);
	}
}