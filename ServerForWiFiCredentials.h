#include <WiFi.h>
// #include <WebServer.h>
#include <ESPAsyncWebServer.h>


AsyncWebServer server(80);

// Create an instance of the web server on port 80
// WebServer server(80);

String ssid;
String password;
String uid;
String api;
int8_t wifi_channel = 0;

//////////////////HMI Config HTML/////////////////////////////////
const char wifimanager[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<title>BITA Zonemaster</title>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    html {
    font-family: Arial, Helvetica, sans-serif; 
    display: inline-block; 
    text-align: center;
    }

    h1 {
      font-size: 1.8rem; 
      color: white;
    }

    p { 
      font-size: 1.4rem;
    }

    .topnav { 
      overflow: hidden; 
      background-color: #0A1128;
    }

    body {  
      margin: 0;
    }

    .content { 
      padding: 5%;
    }

    .card-grid { 
      max-width: 800px; 
      margin: 0 auto; 
      display: grid; 
      grid-gap: 2rem; 
      grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
    }

    .card { 
      background-color: white; 
      box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5);
    }

    .card-title { 
      font-size: 1.2rem;
      font-weight: bold;
      color: #034078
    }

    input[type=submit] {
      border: none;
      color: #FEFCFB;
      background-color: #034078;
      padding: 15px 15px;
      text-align: center;
      text-decoration: none;
      display: inline-block;
      font-size: 16px;
      width: 100px;
      margin-right: 10px;
      border-radius: 4px;
      transition-duration: 0.4s;
      }

    input[type=submit]:hover {
      background-color: #1282A2;
    }

    input[type=text], input[type=number], select {
      width: 50%;
      padding: 12px 20px;
      margin: 18px;
      display: inline-block;
      border: 1px solid #ccc;
      border-radius: 4px;
      box-sizing: border-box;
    }

    label {
      font-size: 1.2rem; 
    }
    .value{
      font-size: 1.2rem;
      color: #1282A2;  
    }
    .state {
      font-size: 1.2rem;
      color: #1282A2;
    }
    button {
      border: none;
      color: #FEFCFB;
      padding: 15px 32px;
      text-align: center;
      font-size: 16px;
      width: 100px;
      border-radius: 4px;
      transition-duration: 0.4s;
    }
    .button-on {
      background-color: #034078;
    }
    .button-on:hover {
      background-color: #1282A2;
    }
    .button-off {
      background-color: #858585;
    }
    .button-off:hover {
      background-color: #252524;
    } 
    * {
      box-sizing: border-box;
    }
    .column {
      float: left;
      width: 50%;
      padding: 10px;
    }

    /* Clear floats after the columns */
    .row:after {
      content: "";
      display: table;
      clear: both;
    }
  </style>
</head>
<body>
  <div class="topnav">
    <h1>BITA RMS</h1>
  </div>
  <div class="content">
    <div class="card-grid">
      <div class="card">
        <form action="/wifi_param" method="POST">
          <p>
            <label for="ssid">SSID</label>
            <input type="text" id ="ssid" name="ssid"><br>
            <label for="pass">Password</label>
            <input type="text" id ="pass" name="pass"><br>
            <input type ="submit" value ="Submit">
          </p>
        </form>
      </div>
    </div>
  </div>
</body>
</html>)rawliteral";

////////////////////////Wifi Manager////////////////////////////

const char *PARAM_INPUT_1 = "ssid";
const char *PARAM_INPUT_2 = "pass";




// void handlegetinfo() {

//   String deviceinfo = "{";
//   deviceinfo += "\"mac_address\":\"" + macaddress + "\",";
//   deviceinfo += "\"slave_mac_address\":\"\",";
//   deviceinfo += "\"type\":" + String(device_type) + ",";
//   deviceinfo += "\"vers\":\"" + devversion + "\",";
//   deviceinfo += "\"id\":\"" + myID + "\",";
//   deviceinfo += "\"disconnection_counter\":" + String(disconnection_counter) + ",";
//   deviceinfo += "\"device_timeout_counter\":" + String(switchblecounter) + ",";
//   deviceinfo += "\"wifi_channel\":" + String(wifi_channel) + ",";
//   deviceinfo += "\"wifissid\":\"" + ssid + "\",";
//   deviceinfo += "\"wifipassword\":\"" + password + "\"";
//   deviceinfo += "}";

// #ifdef DEBUG
//   Serial.print("deviceinfo:");
//   Serial.println(deviceinfo);
// #endif

//   server.send(200, "application/json", deviceinfo);
// }



// void handleRoot() {
//   server.send(200, "text/html", R"rawliteral(
//         <html>
//         <form action="/wifi" method="POST">
//             SSID: <input type="text" name="ssid"><br>
//             Password: <input type="password" name="password"><br>
//             <input type="submit" value="Connect">
//         </form>
//         </html>)rawliteral");
// }

// void handleWiFi() {
//   String ssid = server.arg("ssid");
//   String password = server.arg("password");

//   Serial.println("ssid: ");
//   Serial.println(ssid);
//   Serial.println("password: ");
//   Serial.println(password);

//   // WiFi.begin(ssid.c_str(), password.c_str());
//   server.send(200, "text/plain", "Attempting to connect to Wi-Fi...");
//   ESP.restart();

// }



// void handleWifiCredentials() {
//   // Check if the request contains the body
//   if (server.hasArg("plain")) {
//     String body = server.arg("plain");
//     Serial.println("Received request body: " + body); // Debugging

//     // Deserialize JSON
//     DynamicJsonDocument doc(256);
//     DeserializationError error = deserializeJson(doc, body);

//     if (error) {
//       Serial.print("JSON Deserialization Error: ");
//       Serial.println(error.c_str()); // Debugging error details
//       server.send(400, "application/json", "{\"status\":\"invalid_json\"}");
//       return;
//     }

//     // Extract SSID and Password
//     String ssid = doc["ssid"];
//     String password = doc["password"];
//     Serial.println("Parsed SSID: " + ssid);       // Debugging
//     Serial.println("Parsed Password: " + password); // Debugging (Avoid logging sensitive data in production)

//     if (ssid.isEmpty() || password.isEmpty()) {
//       Serial.println("Error: Missing SSID or Password"); // Debugging
//       server.send(400, "application/json", "{\"status\":\"missing_parameters\"}");
//       return;
//     }

//     // Attempt to connect to Wi-Fi
//     Serial.println("Connecting to Wi-Fi...");


//   } else {
//     Serial.println("Error: Missing request body"); // Debugging
//     server.send(400, "application/json", "{\"status\":\"missing_body\"}");
//   }
// }

// void handleWifiCredentials() {
//   if (server.hasArg("plain")) {
//     String body = server.arg("plain");
//     Serial.println("Received body: " + body);
//     server.send(200, "application/json", "{\"status\":\"success\"}");
//   } else {
//     server.send(400, "application/json", "{\"status\":\"no_body\"}");
//   }
// }


// void handleWifiConfig() {
//   if (server.hasArg("plain")) {
//     String body = server.arg("plain");

//     StaticJsonDocument<200> doc;
//     DeserializationError error = deserializeJson(doc, body);

//     if (error) {
//       server.send(400, "application/json", "{\"message\":\"Invalid JSON\"}");
//       return;
//     }

//     String newSSID = doc["ssid"];
//     String newPassword = doc["password"];
// #ifdef DEBUG
//     Serial.println("Received SSID: " + newSSID);
//     Serial.println("Received Password: " + newPassword);
// #endif

//     preferences.begin("esp32", false);
//     preferences.putString("ssid", newSSID);
//     preferences.putString("password", newPassword);
//     preferences.end();



// #ifdef DEBUG
//     Serial.println("Settings saved:");
//     Serial.println("SSID: " + newSSID);
//     Serial.println("Password: " + newPassword);
// #endif

//     // WiFi.begin(newSSID.c_str(), newPassword.c_str());

//     server.send(200, "application/json", "{\"message\":\"Wi-Fi credentials updated successfully\"}");

//     ESP.restart();
//   } else {
//     server.send(400, "application/json", "{\"message\":\"No data received\"}");
//   }
// }

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

  // if (!MDNS.begin("esp32wifi")) {  //device_name.local
  //     return ;
  //   }

  //   MDNS.addService("http", "tcp", 80);
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

  // if (!MDNS.begin("esp32wifi")) {  //device_name.local
  //   return true;
  // }

  // MDNS.addService("http", "tcp", 80);
  return true;
}

void setup_wifi_credentials() {

  // Start preferences storage
  preferences.begin("wifi_params", false);
  // Get the saved ssid and password
  ssid = preferences.getString("ssid", "BITA DEV");
  password = preferences.getString("password", "xttok2fb");
  // uid = preferences.getString("uid", "");
  // api = preferences.getString("api", "");
  preferences.end();

#ifdef DEBUG
  Serial.print("Devicename: ");
  Serial.println(devicename);
  Serial.print("ssid: ");
  Serial.println(ssid);
  Serial.print("password: ");
  Serial.println(password);
#endif


  // ssid = "BITA DEV";
  // password = "xttok2fb";

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

#ifdef DEBUG
  Serial.println("Server Begin");
#endif
}