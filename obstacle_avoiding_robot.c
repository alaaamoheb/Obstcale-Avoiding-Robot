#include <avr/io.h>
#include <util/delay.h>

//defining motor driver pins

#define enA_pin PB2        //pin10 OC1B yellow Enable for motor1

#define in1_pin PB3       //pin11 blue
#define in2_pin PB4     //pin12 green

#define enB_pin PB1		 //pin 9 OC1A white  Enable for motor2

#define in3_pin PB0      //pin8 purple 
#define in4_pin PB5     //pin13 grey 

//defining ultrasonic sensor pins
#define TRIGGER_PIN PD2   // Trigger pin for ultrasonic sensor pin2
#define ECHO_PIN PD3      // Echo pin for ultrasonic sensor pin3

// Function to initialize ultrasonic sensor
void initializeUltrasonicSensor() {
	// Set trigger pin as output : set 1
	DDRD |= (1 << TRIGGER_PIN);
	
	// Set echo pin as input : set 0
	DDRD &= ~(1 << ECHO_PIN);
}

// Function to generate ultrasonic pulse
void generateUltrasonicPulse() {
	// Generate a 10us pulse on trigger pin
	PORTD |= (1 << TRIGGER_PIN); //set trigger pin to high state   
	_delay_us(10);               //delay to ensure that the trigger pulse remains high
	PORTD &= ~(1 << TRIGGER_PIN);//clear the trigger pin - to end the trigger
}

// Function to measure distance using ultrasonic sensor
int measureDistance() {
	generateUltrasonicPulse();
	
	// Measure the duration of the echo pulse
	int duration = 0;  //variable to store in the duration of the echo pulse
	while (!(PIND & (1 << ECHO_PIN))) {} // waits for the start of the echo pulse - wait until it become high
	while (PIND & (1 << ECHO_PIN)) {
		duration++;
		_delay_us(10);
	}
	
	// Calculate distance based on the duration
	int distance = duration * 0.017;  // Speed of sound is approximately 0.017 cm/us
	
	return distance;
}

void setup() {
	// Set all the motor control pins as outputs
	DDRB |= (1 << enA_pin) | (1 << in1_pin) | (1 << in2_pin);
	DDRB |= (1 << enB_pin) | (1 << in3_pin) | (1 << in4_pin);
	
	// Turn off motors - Initial state
	PORTB &= ~((1 << in1_pin) | (1 << in2_pin));
	PORTB &= ~((1 << in3_pin) | (1 << in4_pin));
	
  // Initialize ultrasonic sensor
	initializeUltrasonicSensor();
}

void loop() {
	// Measure distance
	int distance = measureDistance();
	
	// Check if distance is less than the threshold (20 cm)
	if (distance > 20) {
		// Move the motors forward
		PORTB |= (1 << in1_pin);
		PORTB &= ~(1 << in2_pin);
		PORTB |= (1 << in3_pin);
		PORTB &= ~(1 << in4_pin);
		
		// Set motors speed
		OCR1A = 150;
		OCR1B = 150;
		} else {
		// Stop the motors
		PORTB &= ~((1 << in1_pin) | (1 << in2_pin));
		PORTB &= ~((1 << in3_pin) | (1 << in4_pin));
		
		// Set motors speed to zero
		OCR1A = 0;
		OCR1B = 0;
	}
	
	_delay_ms(100);
}



int main() {
	// Set up PWM for Timer/Counter1
	TCCR1A |= (1 << COM1A1) | (1 << COM1B1) | (1 << WGM10);   // Enable PWM on OC1A (PB1_pin9) and OC1B (PB2_pin10), Phase Correct PWM mode
	TCCR1B |= (1 << CS11);   // Set prescaler to 8,that the counter will increment every 8 clock cycles. So, PWM frequency = F_CPU / (N * 510) , counter's maximum value (510).
	
	setup();
	
	while (1) {
		loop();
	}
}

