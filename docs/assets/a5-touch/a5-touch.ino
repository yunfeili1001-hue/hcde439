// Import the library that measures capacitive touch.
#include <CapacitiveSensor.h>

// Set D2 as the send pin and D4 as the receive pin.
CapacitiveSensor touchSensor(2, 4);

// Use D9 to send PWM to the MOSFET gate.
const int ledPin = 9;

// Use a threshold between the idle and touched sensor readings.
const long touchThreshold = 450;

// Track the current PWM level, starting at zero.
int brightness = 0;

// Change the PWM level by five counts on each loop.
const int fadeStep = 5;

// Run this setup once at startup.
void setup() {
  // Switch D9 from high-impedance input to output. Its reset output latch is LOW.
  pinMode(ledPin, OUTPUT);

  // Set the initial duty cycle to zero.
  analogWrite(ledPin, 0);

  // Open the serial connection at 9600 baud.
  Serial.begin(9600);
}

// Repeat the sensor reading and brightness update.
void loop() {
  // Sum ten raw capacitive sensing samples.
  long touchValue = touchSensor.capacitiveSensorRaw(10);

  // Print the label for the sensor reading.
  Serial.print("Sensor: ");

  // Print the current sensor reading.
  Serial.print(touchValue);

  // Increase brightness for a valid reading above the touch threshold.
  if (touchValue >= 0 && touchValue > touchThreshold) {
    // Stop increasing at the maximum PWM value of 255.
    brightness = min(brightness + fadeStep, 255);
  }
  // Fade down when released or when the sensor reports an error.
  else {
    // Stop decreasing at the minimum PWM value of zero.
    brightness = max(brightness - fadeStep, 0);
  }

  // Update the MOSFET gate duty cycle on D9.
  analogWrite(ledPin, brightness);

  // Print the label for the PWM level.
  Serial.print(" | Brightness: ");

  // Print the PWM level and end the line.
  Serial.println(brightness);

  // Space out PWM updates to make the fade visible.
  delay(15);
}
