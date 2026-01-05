/*
 * FlagManagement Example
 * 
 * Demonstrates flag management with the cCount class.
 * 
 * This example shows how to use currentFlag and nextFlag
 * to manage state transitions in a simple state machine.
 */

#include <counter.h>

cCount stateCounter;
bool stateA = true;   // Current state
bool stateB = false;  // Next state

void setup() {
  Serial.begin(115200);
  Serial.println("FlagManagement Example");
  Serial.println("State machine with 5-count transitions");
}

void loop() {
  // Count to 5 and toggle between states
  stateCounter.count(5, stateA, stateB);
  
  // Display state changes
  if (stateCounter.finish()) {
    if (stateA) {
      Serial.println("Switched to State A");
    } else if (stateB) {
      Serial.println("Switched to State B");
    }
    
    // Show current count
    Serial.print("Count: ");
    Serial.println(stateCounter.get());
  }
  
  // Perform actions based on current state
  if (stateA) {
    // Actions for State A
    digitalWrite(LED_BUILTIN, HIGH);
  } else if (stateB) {
    // Actions for State B
    digitalWrite(LED_BUILTIN, LOW);
  }
  
  delay(200); // 200ms per count
}
