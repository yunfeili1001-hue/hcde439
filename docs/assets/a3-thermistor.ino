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

  // Store whether the temperature is below the 140 F threshold as 1 or 0.
  int greenState = (tempF < THRESHOLD_F) ? 1 : 0;
  // Map the selected green state to off (0) or full PWM brightness (255).
  int greenBrightness = map(greenState, 0, 1, 0, 255);

  // Keep green fully on whenever the calculated temperature is below 140 F.
  if (tempF < THRESHOLD_F) {
    // Turn the green LED fully on with PWM 255.
    analogWrite(GREEN_PIN, greenBrightness);
    // Turn the red LED off.
    analogWrite(RED_PIN, 0);
  }
  // Keep red fully on at or above 140 F.
  else {
    // Turn the red LED fully on with PWM 255.
    analogWrite(RED_PIN, 255 - greenBrightness);
    // Turn the green LED off.
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
