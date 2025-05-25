#include <Servo.h>
#include "EMGFilters.h"

const int emgPin = A0;           // Analog pin connected to EMG sensor output
const int servoPins[3] = {9, 11, 13};  // Digital pins for 3 servos
const int threshold = 560;       // EMG activation threshold

Servo myServos[3];               // Array of servo objects
EMGFilters emgFilter;

bool servosAt180[3] = {false, false, false}; // Track state of each servo
bool prevAboveThreshold = false;

void setup() {
  Serial.begin(9600);
  
  // Initialize all servos
  for (int i = 0; i < 3; i++) {
    myServos[i].attach(servoPins[i]);
    myServos[i].write(0); // Initialize all servos to 0°
  }

  // Initialize EMG filter
  emgFilter.init(500, true, true, true);
}

void loop() {
  int rawEMG = analogRead(emgPin);
  int filteredEMG = emgFilter.update(rawEMG);
  int envelope = abs(filteredEMG);

  // Output for Serial Plotter
  Serial.print(envelope);
  Serial.print(",");
  Serial.print(0);    // Fixed minimum value
  Serial.print(",");
  Serial.println(1023); // Fixed maximum value

  bool currentAboveThreshold = envelope > threshold;

  // Detect rising edge
  if (currentAboveThreshold && !prevAboveThreshold) {
    // Toggle all servos together (same behavior as original, but for 3 servos)
    for (int i = 0; i < 3; i++) {
      if (servosAt180[i]) {
        myServos[i].write(0);
        servosAt180[i] = false;
      } else {
        myServos[i].write(180);
        servosAt180[i] = true;
      }
      delay(100);
    }
    delay(4000);
  }

  prevAboveThreshold = currentAboveThreshold;

  delay(2); // Small delay for stability
}