void setup_config(){
  settings.begin(settings_namespace, false);
  counter = settings.getUInt("counter", 0);
  // Increase counter by 1
  counter++;
  // Store the counter to the Preferences
  settings.putUInt("counter", counter);
  // Close the Preferences
  settings.end();
}
