#include <avr/sleep.h>  // Include the sleep library
#include <TimerOne.h>
#include <math.h>       // Include the math library for logarithmic mapping
#include <avr/power.h>  // Include the power library for clock prescaling

#define SERIAL_OUTPUT_ON false   // Set to true to enable serial output, false to disable
#define TIMER_INTERVAL_MS 10000  // Adjust this value to change the Timer1 interval in milliseconds

float EMA_a = 0.35;        //initialization of EMA alpha
int EMA_S = 0;             //initialization of EMA S
const int potPin = A7;     // Analog pin connected to the potentiometer
const int mosfetPin = 11;  // Digital pin connected to the gate of the MOSFET

void setup() {
  TCCR2B = TCCR2B & B11111000 | B00000010;  // set timer 2 divisor to     8 for PWM frequency of  3921.16 Hz
  analogReference(EXTERNAL);
  pinMode(potPin, INPUT);      // Set potentiometer pin as input
  pinMode(mosfetPin, OUTPUT);  // Set MOSFET pin as output
  Timer1.initialize(TIMER_INTERVAL_MS);
  Timer1.attachInterrupt(looper);  // blinkLED to run every 0.15 seconds
// Initialize serial communication if SERIAL_OUTPUT_ON is true
#if SERIAL_OUTPUT_ON
  Serial.begin(115200);
#endif
  EMA_S = analogRead(potPin);  //set EMA S for t=1
}

void looper(void) {
  // Your main code here
  int potValue = analogRead(potPin);

  EMA_S = (EMA_a * potValue) + ((1 - EMA_a) * EMA_S);  //run the EMA

  // Map the potentiometer value exponentially to PWM value (50-255)
  int pwmValue = expMap(EMA_S, 0, 1023, 5, 255);

  analogWrite(mosfetPin, pwmValue);

// Output the potentiometer value and mapped PWM value to serial monitor if SERIAL_OUTPUT_ON is true
#if SERIAL_OUTPUT_ON
  Serial.print("\npotValue value: ");
  Serial.println(potValue);
  Serial.print("EMA_S value: ");
  Serial.println(EMA_S);
  Serial.print("Mapped PWM value: ");
  Serial.println(pwmValue);
#endif
}

void loop() {
  // Add any necessary code here, if needed
}

// Custom exponential mapping function
int expMap(int x, int in_min, int in_max, int out_min, int out_max) {
  // Ensure that potValue is within the specified range to avoid errors in exponentiation
  x = constrain(x, in_min, in_max);

  // Calculate the exponential scale

  float expScale = pow(x - in_min, 2) / pow(in_max - in_min, 2);

  // Map the exponential scale to the output range
  return (int)(out_min + expScale * (out_max - out_min));
}