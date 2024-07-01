
// ESP32 Super MINI (makerGO C3)
// LOLin S3

#include <Preferences.h>

Preferences preferences;

void setup() {
  Serial.begin(115200);
  Serial.println();

  preferences.begin("memory", false);

  // Remove all preferences under the opened namespace
  //preferences.clear();

  // Or remove the counter key only
  //preferences.remove("counter");

  // Get the counter value, if the key does not exist, return a default value of 0
  // Note: Key name is limited to 15 chars.
  unsigned int counter = preferences.getUInt("counter", 0);

  // Increase counter by 1
  counter++;

  if (counter > 20) {
    counter = 0;
  }

  // Print the counter to Serial Monitor
  Serial.printf("Current counter value: %u\n", counter);

  // Store the counter to the Preferences
  preferences.putUInt("counter", counter);

  // Close the Preferences
  preferences.end();

  // Wait 10 seconds
  Serial.println("Restarting in 10 seconds...");
   delay(5000);

  // Restart ESP
   ESP.restart();
}

void loop() {

}