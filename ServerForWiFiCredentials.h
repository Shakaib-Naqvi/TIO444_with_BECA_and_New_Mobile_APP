#include <WiFi.h>
#include <WebServer.h>

// Create an instance of the web server on port 80
WebServer server(80);

String ssid;
String password;
String uid;
String api;
int8_t wifi_channel = 0;

void handlegetinfo() {

  String deviceinfo = "{";
  deviceinfo += "\"mac_address\":\"" + macaddress + "\",";
  deviceinfo += "\"slave_mac_address\":\"\",";
  deviceinfo += "\"type\":" + String(device_type) + ",";
  deviceinfo += "\"vers\":\"" + devversion + "\",";
  deviceinfo += "\"id\":\"" + myID + "\",";
  deviceinfo += "\"disconnection_counter\":" + String(disconnection_counter) + ",";
  deviceinfo += "\"device_timeout_counter\":" + String(switchblecounter) + ",";
  deviceinfo += "\"wifi_channel\":" + String(wifi_channel) + ",";
  deviceinfo += "\"wifissid\":\"" + ssid + "\",";
  deviceinfo += "\"wifipassword\":\"" + password + "\"";
  deviceinfo += "}";

#ifdef DEBUG
  Serial.print("deviceinfo:");
  Serial.println(deviceinfo);
#endif

  server.send(200, "application/json", deviceinfo);
}

void handleWifiConfig() {
  if (server.hasArg("ssid") && server.hasArg("password")) {

    ssid = server.arg("ssid");
    password = server.arg("password");

    // Save the data to non-volatile storage
    preferences.begin("esp32", false);
    preferences.putString("ssid", ssid);
    preferences.putString("password", password);
    preferences.end();

    // Create a response message indicating success
    String response = "Settings have been successfully saved!";

    // Send the response back to the client
    server.send(200, "text/plain", response);
    // Print the saved data to the serial monitor for debugging
#ifdef DEBUG
    Serial.println("Settings saved:");
    Serial.println("SSID: " + ssid);
    Serial.println("Password: " + password);
#endif

    // connect_to_wifi();
    // Restart the ESP32 after saving
    ESP.restart();
  } else {
    server.send(400, "text/plain", "Please provide ssid, password parameters.");
#ifdef DEBUG
    Serial.println("Please provide ssid, password parameters.");
#endif
  }
}

void Access_Point() {

  WiFi.mode(WIFI_AP);
  WiFi.softAP(devicename.c_str(), "bitahomes", 0, 1);  // Set your desired SSID and password
  Serial.println("AP Lauched with ssid and password:");
  Serial.println(devicename);
  Serial.println("bitahomes");

  IPAddress IP = WiFi.softAPIP();

#ifdef DEBUG
  Serial.print("AP IP address: ");
  Serial.println(IP);
#endif
}

bool connect_to_wifi() {

  WiFi.mode(WIFI_MODE_APSTA);
  WiFi.setHostname(hostname.c_str());

  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
  // Connect to the provided Wi-Fi network

  unsigned long start_time = millis();

  WiFi.begin(ssid.c_str(), password.c_str());

#ifdef DEBUG
  Serial.println("Saved settings:");
  Serial.println("SSID: " + ssid);
  Serial.println("Password: " + password);
  // Serial.println("UID: " + uid);
  // Serial.println("API: " + api);
  Serial.print("Connecting to WiFi..");
#endif

  while (WiFi.status() != WL_CONNECTED) {

    if (millis() - start_time >= 20000) {
      // Timeout reached, stop trying to connect
#ifdef DEBUG
      Serial.println();
      Serial.println("Failed to connect to WiFi within 20 Seconds.");
#endif
      Access_Point();
      return false;  // Exit the function or handle the failure case
    }
    delay(50);

#ifdef DEBUG
    Serial.print(".");
#endif
  }
  // Print the IP address of the ESP32 once connected to Wi-Fi
  myIP = WiFi.localIP().toString();

#ifdef DEBUG
  Serial.println();
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("connected with IP: ");
  Serial.println(myIP);
#endif

  wifi_channel = WiFi.channel();
  WiFi.softAP(devicename.c_str(), "bitahomes", wifi_channel, 1);
  
  Serial.println("AP Lauched with ssid and password:");
  Serial.println(devicename);
  Serial.println("bitahomes");
  return true;
}

void setup_wifi_credentials() {

  // Start preferences storage
  preferences.begin("esp32", false);
  // Get the saved ssid and password
  ssid = preferences.getString("ssid", "");
  password = preferences.getString("password", "");
  // uid = preferences.getString("uid", "");
  // api = preferences.getString("api", "");
  preferences.end();

#ifdef DEBUG
  Serial.print("ssid: ");
  Serial.println(ssid);
#endif


  ssid = "BITA HOMES";
  password = "xttok2fb";
  
  if (ssid.length() > 0 && password.length() > 0) {
    connect_to_wifi();
#ifdef DEBUG
    Serial.println("Credentials Found");
#endif
  }  //
  else {
    // Set up the ESP32 as an access point
#ifdef DEBUG
    Serial.println("Credentials Not Found");
    Serial.println("AP Mode");
#endif
    Access_Point();
  }

  // server.on("/getdeviceinfo", HTTP_GET, handlegetinfo);
  // server.on("/wifi-config", HTTP_POST, handleWifiConfig);
  // server.begin();

#ifdef DEBUG
  Serial.println("Server Begin");
#endif
}

// void handleDelete() {
//   // Delete stored credentials from preferences

//   // Add Status Flag

//   preferences.remove("ssid");
//   preferences.remove("password");
//   preferences.remove("uid");
//   preferences.remove("api");

//   // Restart ESP32
//   ESP.restart();
// }
