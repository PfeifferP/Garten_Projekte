bool is_first_boot(){
    
}

void loadWiFiconfig(){
  config.begin(wifi_namespace);
  bool doesExist = config.isKey("ssid");
  if (doesExist == false) {
    /*
       If doesExist is false, we will need to create our
        namespace key(s) and store a value into them.
   */

   // Insert your "first time run" code to create your keys & assign their values below here.
    }
    else {
       String wifi_ssid = config.getString("ssid");
    }
}
