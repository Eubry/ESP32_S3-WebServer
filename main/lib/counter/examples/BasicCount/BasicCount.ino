/*
 * BasicCount Example
 * 
 * Demonstrates basic usage of the cCount class to count events
 * and trigger actions when a goal is reached.
 * 
 * This example counts to 10 and prints a message each time
 * the goal is reached.
 */

#include <counter.h>

cCount myCounter;

void setup() {
  Serial.begin(115200);
  Serial.println("BasicCount Example");
  Serial.println("Counting to 10...");
}

void loop() {
  // Count to 10
  myCounter.count(10);
  
  // Check if we just started (reset to 0)
  if (myCounter.start()) {
    Serial.print("Counter reset. Current count: ");
    Serial.println(myCounter.get());
  }
  
  // Check if goal is reached
  if (myCounter.finish()) {
    Serial.println("Goal reached! Counter will reset.");
  }
  
  // Execute callback when finished
  myCounter.after([]() {
    Serial.println("Callback executed!");
  });
  
  delay(100); // Delay for visibility
}
