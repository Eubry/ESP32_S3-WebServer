/*
 * TimerExample
 * 
 * Demonstrates the cTime class for precise timing operations.
 * 
 * This example uses the ESP32's hardware timer to create
 * a 1-second interval timer with microsecond precision.
 */

#include <counter.h>

cTime myTimer;
int count = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("TimerExample - 1 second interval");
}

void loop() {
  // Wait for 1 second (1,000,000 microseconds)
  myTimer.wait(1000000);
  
  // Check if timer just started
  if (myTimer.start()) {
    Serial.println("Timer started!");
  }
  
  // Check if 1 second has elapsed
  if (myTimer.finish()) {
    count++;
    Serial.print("1 second elapsed! Count: ");
    Serial.println(count);
    
    // Get exact elapsed time
    int64_t elapsed = myTimer.get();
    Serial.print("Elapsed time (microseconds): ");
    Serial.println((long)elapsed);
  }
  
  // Optional: Display progress every 100ms
  static int64_t lastPrint = 0;
  int64_t current = myTimer.get();
  if (current - lastPrint > 100000) { // 100ms
    Serial.print("Progress: ");
    Serial.print((current * 100) / 1000000);
    Serial.println("%");
    lastPrint = current;
  }
}
