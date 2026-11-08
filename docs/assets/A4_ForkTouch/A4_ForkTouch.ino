// Use the Arduino Servo library to generate control pulses for the SG90.
#include <Servo.h>
// Use the CapacitiveSensor library by Paul Badger and Paul Stoffregen.
#include <CapacitiveSensor.h>

// Create a controller for the SG90 servo.
Servo myServo;
// Drive pin 2 through the 1 MΩ resistor and read the fork at pin 4.
CapacitiveSensor touchSensor(2, 4);

// Send the servo control pulses from pin 12.
const int servoPin = 12;
// Switch the green LED on pin 9 through its 330 Ω series resistor.
const int ledPin = 9;
// Treat a reading above 100 library counts as a touch.
const long threshold = 100;

void setup() {
  // Print sensor readings to the Serial Monitor at 9600 baud.
  Serial.begin(9600);
  // Set pin 9 as a digital output so it can switch the LED.
  pinMode(ledPin, OUTPUT);
  // Enable Servo library control of the signal wire on pin 12.
  myServo.attach(servoPin);
}

void loop() {
  // Sum 30 sensing cycles to obtain one capacitance reading.
  const long sensorValue = touchSensor.capacitiveSensor(30);
  // Print the reading so untouched and touched values can be compared.
  Serial.println(sensorValue);

  // Activate the outputs only when the reading exceeds the threshold.
  if (sensorValue > threshold) {
    // Command a quarter turn from the resting position.
    myServo.write(90);
    // Light the green LED while the fork is touched.
    digitalWrite(ledPin, HIGH);
  } else {
    // Return the servo to the resting position when the fork is released.
    myServo.write(0);
    // Switch off the green LED.
    digitalWrite(ledPin, LOW);
  }

  // Pause 50 ms before sampling again to space readings and serial output.
  delay(50);
}
