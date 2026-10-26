// Include the logarithm function used in the thermistor equation.
#include <math.h>

// Connect the green LED through 220 ohms to PWM pin 9.
const int GREEN_PIN = 9;
// Connect the red LED through 220 ohms to PWM pin 10.
const int RED_PIN = 10;
// Read the divider midpoint at analog input A2.
const int SENSOR_PIN = A2;
// Use a 10 kiloohm fixed resistor above the thermistor.
const float FIXED_R = 10000.0;
// Assume the thermistor is 10 kiloohms at 25 degrees Celsius.
const float NOMINAL_R = 10000.0;
// Express the nominal temperature in kelvin: 25 + 273.15.
const float NOMINAL_T = 298.15;
// Assume Beta is 3950 K; check this against the actual sensor specifications.
const float BETA = 3950.0;
// Switch from green to red at a calculated temperature of 140 degrees Fahrenheit.
const float THRESHOLD_F = 140.0;

// Configure the LED outputs and serial communication once.
void setup() {
  // Set the green LED pin to output mode.
  pinMode(GREEN_PIN, OUTPUT);
  // Set the red LED pin to output mode.
  pinMode(RED_PIN, OUTPUT);
  // Start the Serial Monitor connection at 9600 baud.
  Serial.begin(9600);
}

// Continuously read the sensor and update the indicator.
void loop() {
  // Read the 10-bit ADC value from A2, ranging from 0 to 1023.
  int raw = analogRead(SENSOR_PIN);

  // Reject endpoint readings to avoid division by zero or an invalid logarithm.
  if (raw <= 0 || raw >= 1023) {
    // Turn off the green LED for an invalid reading.
    analogWrite(GREEN_PIN, 0);
    // Turn off the red LED for an invalid reading.
    analogWrite(RED_PIN, 0);
    // Report the invalid reading instead of a temperature.
    Serial.println("Sensor reading invalid: check wiring.");
    // Wait half a second before checking again.
    delay(500);
    // Skip the remainder of this loop pass.
    return;
  }

  // Solve the divider equation for the thermistor connected below the midpoint.
  float resistance = FIXED_R * raw / (1023.0 - raw);
  // Apply the Beta equation to calculate inverse temperature in kelvin.
  float inverseT = (1.0 / NOMINAL_T) + log(resistance / NOMINAL_R) / BETA;
  // Convert the calculated kelvin temperature to degrees Celsius.
  float tempC = (1.0 / inverseT) - 273.15;
  // Convert Celsius to Fahrenheit for the threshold comparison.
  float tempF = tempC * 9.0 / 5.0 + 32.0;

  // Use the green indicator below the 140-degree Fahrenheit threshold.
  if (tempF < THRESHOLD_F) {
    // Limit only the green brightness calculation to 120–140 degrees; all values below 140 still select green.
    long boundedTemp = constrain((long)tempF, 120L, 140L);
    // Map 120–140 degrees to green PWM 255–60; colder readings stay at 255.
    int brightness = map(boundedTemp, 120, 140, 255, 60);
    // Set the active green LED brightness with 8-bit PWM.
    analogWrite(GREEN_PIN, brightness);
    // Turn off the red LED while green is active.
    analogWrite(RED_PIN, 0);
  }
  // Use red when the calculated temperature reaches or exceeds 140 degrees.
  else {
    // Limit the red brightness calculation to 140–160 degrees.
    long boundedTemp = constrain((long)tempF, 140L, 160L);
    // Map 140–160 degrees to red PWM 60–255; hotter readings stay at 255.
    int brightness = map(boundedTemp, 140, 160, 60, 255);
    // Set the active red LED brightness with 8-bit PWM.
    analogWrite(RED_PIN, brightness);
    // Turn off the green LED while red is active.
    analogWrite(GREEN_PIN, 0);
  }

  // Label the raw ADC reading.
  Serial.print("ADC: ");
  // Print the sensor reading from A2.
  Serial.print(raw);
  // Label the calculated temperature.
  Serial.print(" | Temperature: ");
  // Print Fahrenheit with one decimal place.
  Serial.print(tempF, 1);
  // Print the temperature unit and indicator label.
  Serial.print(" F | Indicator: ");
  // Print the current indicator color and end the line.
  Serial.println(tempF < THRESHOLD_F ? "GREEN" : "RED");
  // Update the output and serial log every 500 milliseconds.
  delay(500);
}
