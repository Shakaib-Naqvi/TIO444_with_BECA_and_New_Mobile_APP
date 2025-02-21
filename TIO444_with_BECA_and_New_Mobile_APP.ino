#define DEBUG
#define MQTT_MAX_PACKET_SIZE 1024

#include "variables.h"

#include <Preferences.h>
#include <ESPmDNS.h>

Preferences preferences;

#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <ArduinoJson.h>

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;


#include "RES_MODBUS_V1_3.h"
#include "GET_from_API.h"
#include "Servo.h"
#include "ServerForWiFiCredentials.h"
#include "OLED_Display.h"

void Pot_Calib(int16_t min, int16_t max) {
  if (min == 0 || max == 0) {

    if (min == 0) {
      myservo.write(servo_close_pos);
      delay(1000);
      int16_t potmin = analogRead(34);
      delay(1000);
      preferences.begin("Pot", false);
      preferences.putInt("Min", potmin);
      preferences.end();
#ifdef DEBUG
      Serial.print("PotMin:");
      Serial.println(potmin);
#endif
    }

    if (max == 0) {
      myservo.write(servo_open_pos);
      delay(1000);
      int16_t potmax = analogRead(34);
      delay(1000);
      preferences.begin("Pot", false);
      preferences.putInt("Max", potmax);
      preferences.end();
#ifdef DEBUG
      Serial.print("potmax:");
      Serial.println(potmax);
#endif
    }
  }
}

void Beep(uint16_t beepDelay, uint8_t numberOfBeeps) {
  for (int i = 0; i < numberOfBeeps; i++) {
    digitalWrite(Buzzer_Pin, HIGH);
    delay(beepDelay);
    digitalWrite(Buzzer_Pin, LOW);
    delay(beepDelay);
  }
}

int16_t ReadPot(int16_t potPin) {

  int minValue = minval;  // at servo_close_pos
  int maxValue = maxval;  // at servo_open_pos


  float potValue = analogRead(potPin);
  int mappedValue = map(potValue, minValue, maxValue, 0, 100);

  if (mappedValue < minValue) {
    minValue = mappedValue;
  } else if (mappedValue > maxValue) {
    maxValue = mappedValue;
  }

#ifdef DEBUG
  Serial.println("Pot Value");
  Serial.println(potValue);
  Serial.println("Mapped Value+");
  Serial.println(mappedValue);
#endif

  return mappedValue;
}

void Extract_by_json(String incomingMessage) {
  StaticJsonDocument<512> doc;

  DeserializationError error = deserializeJson(doc, incomingMessage);
  if (error) {
#ifdef DEBUG
    Serial.print("JSON deserialization failed: ");
    Serial.println(error.c_str());
#endif
    return;
  }

  seasonsw = doc["seasonsw"].as<int>();
  dmptempsp = doc["dmptempsp"].as<int>();
  dampertsw = doc["dampertsw"].as<int>();
  supcfm = doc["supcfm"].as<String>();
  dampstate = doc["dampstate"].as<int>();

  cfm_flag = false;

  if (beca_status != false) {
    if (beca_power != dampertsw) {
      if (dampertsw == 1) {
        beca_power = 1;
        writeSingleRegister(1, 0x00, 1);
      } else {
        beca_power = 0;
        writeSingleRegister(1, 0x00, 0);
      }
    }

    if (beca_mode != seasonsw) {
      if (seasonsw == 1) {
        beca_mode = 1;
        writeSingleRegister(1, 0x02, 1);

      } else {
        beca_mode = 0;
        writeSingleRegister(1, 0x02, 0);
      }
    }

    if (setpointt != dmptempsp) {
      if (dmptempsp >= 5 || dmptempsp < 36) {
        setpointt = dmptempsp;
        writeSingleRegister(1, 0x03, setpointt * 10);
      }
    }
  }

  end_value_app = supcfm.toInt();
  new_cfm = map(end_value_app, 0, 100, servo_close_pos, servo_open_pos);

  //--------Changes made in below line-------
  end_value = end_value_app;

  if (new_cfm != CFM_max) {
    CFM_max = new_cfm;
    MoveServo(CFM_max, 1, servo_delay);
  }
  //------------------------

#ifdef DEBUG
  Serial.println("");
  Serial.println("---------------------------------------------");



  Serial.print("beca_power: ");
  Serial.println(beca_power);
  Serial.print("beca_mode: ");
  Serial.println(beca_mode);
  Serial.print("setpointt: ");
  Serial.println(setpointt);
  Serial.print("CFM_max: ");
  Serial.println(CFM_max);

  Serial.println("Extracted Values:");
  Serial.print("Season Switch: ");
  Serial.println(beca_mode);
  Serial.print("DMP Temp SP: ");
  Serial.println(setpointt);
  Serial.print("Power: ");
  Serial.println(beca_power);
  Serial.print("Supply CFM: ");
  Serial.println(supcfm);
  Serial.print("Time Schedule: ");
  Serial.println(timesch);
  Serial.print("Damper State: ");
  Serial.println(dampstate);
  // Serial.print("Packet ID: ");
  // Serial.println(packet_id);
  Serial.println("------------------------------------------------");
  Serial.println("");

#endif
}

void publishJson() {
  StaticJsonDocument<512> doc;

  doc["seasonsw"] = String(beca_mode);
  doc["dmptemp"] = String(temp_by_beca);
  doc["dmptempsp"] = String(setpointt);
  doc["dampertsw"] = String(beca_power);
  doc["supcfm"] = String(end_value);

  // doc["timenow"] = timenow;
  if (dampertsw == 1) {
    doc["dampstate"] = "Open";
  } else {
    doc["dampstate"] = "Close";
  }
  doc["mac_address"] = macaddress;
  doc["ip_address"] = myIP;
  doc["ssid"] = ssid;
  doc["password"] = password;
  doc["comment"] = "From Damper";
  // doc["dampstate"] = "open";



  char jsonBuffer[512];
  size_t n = serializeJson(doc, jsonBuffer);

  if (n >= sizeof(jsonBuffer)) {
#ifdef DEBUG
    Serial.println("JSON buffer overflow!");
#endif
    return;
  }
  client.publish(device_topic_p.c_str(), jsonBuffer, true);

  // {"seasonsw":"0","dmptemp":"28","dmptempsp":"20","dampertsw":"1","supcfm":"80","dampstate":"Open","mac_address":"C8:F0:9E:DF:C3:A0","ip_address":"192.168.18.165","ssid":"BITA DEV","password":"xttok2fb"}
#ifdef DEBUG
  Serial.println("");
  Serial.println(jsonBuffer);
  Serial.println("JSON message published");
#endif
}

void callback(char* topic, byte* message, unsigned int length) {
  String messageTemp;

  for (int i = 0; i < length; i++) {
    messageTemp += (char)message[i];
  }


  if (String(topic) == device_topic_s) {  // /KRC2/ZMB-AAA018/1
#ifdef DEBUG
    Serial.println("Message Received on: ");
    Serial.println(String(topic));
#endif
    Extract_by_json(messageTemp);
  }
}

void reconnect() {
  if (WiFi.status() != WL_CONNECTED) {
#ifdef DEBUG
    Serial.println("Wi-Fi is not connected. Attempting to reconnect...");
#endif
    return;
  }
  if (!client.connected()) {
#ifdef DEBUG
    Serial.print("Attempting MQTT connection...");
#endif
    if (client.connect(devicename.c_str())) {
#ifdef DEBUG
      Serial.println("connected");
#endif
      client.subscribe(device_topic_s.c_str());
    } else {
#ifdef DEBUG
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 10 milliseconds");
#endif
    }
  }
}


void setup() {

#ifdef DEBUG
  Serial.begin(115200);
#endif

  display.begin(OLED_Address, true);  // Address 0x3C default

  display.clearDisplay();
  display_on_OLED(&FreeMonoBold9pt7b, 2, 37, 43, "BITA");

  display.fillRect(0, 0, display.width(), display.height() / 16, SH110X_WHITE);
  display.fillRect(0, 15 * (display.height() / 16), display.width(), display.height() / 16, SH110X_WHITE);
  display.drawBitmap(1, 19, BITA_LOGO, 32, 32, SH110X_WHITE);
  display.display();

  Int_Servo();

  Wire.begin(18, 19);

  preferences.begin("Pot", false);
  minval = preferences.getInt("Min", 0);
  maxval = preferences.getInt("Max", 0);
  preferences.end();

  Pot_Calib(minval, maxval);

  int last_pot_value = ReadPot(34);
  last_pos_servo = map(last_pot_value, 0, 100, servo_close_pos, servo_open_pos);
  pinMode(Buzzer_Pin, OUTPUT);
  digitalWrite(Buzzer_Pin, LOW);

  MoveServo(servo_open_pos, 1, servo_delay);
  delay(1000);
  MoveServo(servo_close_pos, 1, servo_delay);

  RS485Serial.begin(MODBUS_BAUD_RATE, SERIAL_8N1, RS485_RX_PIN, RS485_TX_PIN);

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  updateTimeNow();


  setup_wifi_credentials();
  macaddress = WiFi.macAddress();

  server.on("/configure", HTTP_GET, [](AsyncWebServerRequest* request) {
    AsyncWebServerResponse* response = request->beginResponse(200, "text/html", wifimanager);

    // Add CORS headers
    response->addHeader("Access-Control-Allow-Origin", "*");
    response->addHeader("Access-Control-Allow-Methods", "GET, POST");
    response->addHeader("Access-Control-Allow-Headers", "Content-Type");

    // Send the response with headers
    request->send(response);
  });

  // server.on("/getinfo", HTTP_GET, [](AsyncWebServerRequest* request) {
  //   StaticJsonDocument<256> jsonDoc;
  //   jsonDoc["mac_address"] = macaddress;
  //   jsonDoc["ip_address"] = myIP;
  //   jsonDoc["ssid"] = ssid;
  //   jsonDoc["password"] = password;

  //   String jsonString;
  //   serializeJson(jsonDoc, jsonString);

  //   AsyncWebServerResponse* response = request->beginResponse(200, "application/json", jsonString);

  //   response->addHeader("Access-Control-Allow-Origin", "*");
  //   response->addHeader("Access-Control-Allow-Methods", "GET, POST");
  //   response->addHeader("Access-Control-Allow-Headers", "Content-Type");

  //   request->send(response);
  // });

  // server.on("/configure", HTTP_GET, [](AsyncWebServerRequest* request) {
  //   request->send_P(200, "text/html", wifimanager);
  // });

  server.on("/wifi_param_by_app", HTTP_OPTIONS, [](AsyncWebServerRequest* request) {
    AsyncWebServerResponse* response = request->beginResponse(200);
    response->addHeader("Access-Control-Allow-Origin", "*");
    response->addHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    response->addHeader("Access-Control-Allow-Headers", "Content-Type");
    request->send(response);
  });

  server.on(
    "/wifi_param_by_app", HTTP_POST, [](AsyncWebServerRequest* request) {}, NULL, [](AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total) {
      StaticJsonDocument<512> doc;
      DeserializationError error = deserializeJson(doc, data, len);

      if (error) {
        Serial.print("JSON Parsing Error: ");
        Serial.println(error.c_str());
        request->send(400, "application/json", "{\"status\":\"error\",\"message\":\"Invalid JSON\"}");
        return;
      }

      const char* ssid = doc["ssid"];
      const char* password = doc["password"];

      if (!ssid || !password || strlen(ssid) == 0 || strlen(password) == 0) {
        request->send(400, "application/json", "{\"status\":\"error\",\"message\":\"SSID or password missing\"}");
        return;
      }

      Serial.println("Received JSON Body:");
      Serial.println("SSID: " + String(ssid));
      Serial.println("Password: " + String(password));

      preferences.begin("wifi_params", false);
      preferences.putString("ssid", ssid);
      preferences.putString("password", password);
      preferences.end();

      StaticJsonDocument<128> responseDoc;
      responseDoc["status"] = "success";
      responseDoc["message"] = "WiFi parameters saved. Restarting.";
      String responseBody;
      serializeJson(responseDoc, responseBody);
      request->send(200, "application/json", responseBody);

      delay(1000);
      ESP.restart();
    });

  server.on("/wifi_param", HTTP_POST, [](AsyncWebServerRequest* request) {
    int params = request->params();

#ifdef DEBUG
    Serial.println("Received POST request to /wifi_param");
    Serial.print("Number of parameters: ");
    Serial.println(params);
    String body = "";
    for (int i = 0; i < params; i++) {
      AsyncWebParameter* p = request->getParam(i);
      if (p->isPost()) {
        body += p->name() + "=" + p->value() + "&";
      }
    }
    if (body.length() > 0) {
      body.remove(body.length() - 1);
    }
    Serial.println("POST Body: " + body);
#endif

    for (int i = 0; i < params; i++) {

#ifdef DEBUG
      Serial.println();
      Serial.print("params:");
      Serial.println(params);
#endif

      AsyncWebParameter* p = request->getParam(i);
      if (p->isPost()) {
        Serial.println(p->name());

        if (p->name() == PARAM_INPUT_1) {
          ssid = p->value().c_str();
#ifdef DEBUG
          Serial.print("SSID set to: ");
          Serial.println(ssid);
#endif
        }
        if (p->name() == PARAM_INPUT_2) {
          password = p->value().c_str();
#ifdef DEBUG
          Serial.print("Password set to: ");
          Serial.println(password);
#endif
        }
      }
    }
    if (ssid.length() > 0 && password.length() > 0) {
      request->send(200, "text/plain", "Done. ESP will restart, connect to your router.");

      preferences.begin("wifi_params", false);

      preferences.putString("ssid", ssid);
      preferences.putString("password", password);

      preferences.end();

      delay(1000);
      ESP.restart();
    } else {
      request->send(200, "text/plain", "Try Again. SSID or Password Invalid.");
    }
  });

  server.begin();

#ifdef DEBUG
  Serial.println("Server Begin");
#endif

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);


  beca_check();

  if (beca_power == 1) {
    dampertsw = 1;
    dampstate = 1;
  } else {
    dampertsw = 0;
    dampstate = 0;
  }
  dmptempsp = setpointt;
  dmptemp = temp_by_beca;



  // publishJson();

  // client.setKeepAlive(60);
  // client.setSocketTimeout(30);
  // client.setBufferSize(1024);

#ifdef DEBUG
  Serial.print("Initial Time (WMMDDYYHHMMSS): ");
  Serial.println(timenow);
#endif

  preferences.begin("CFM", false);

  start_value = preferences.getInt("cfm_min", 0);
  end_value = preferences.getInt("cfm_max", 50);
  preferences.end();
  supcfm = String(start_value) + "-" + String(end_value);

  servo_close_pos = map(start_value, 0, 100, servo_close_pos, servo_open_pos);
  CFM_max = map(end_value, 0, 100, servo_close_pos, servo_open_pos);

  preferences.begin("timeenable", false);

  timesch = preferences.getString("timesch", "hoursch=24|daysch=7");
  timeschen = preferences.getBool("timeschen", 0);

  for (uint8_t i = 0; i < 7; i++) {
    String days_key = "days_key" + String(i);
    days_in_num[i] = preferences.getBool(days_key.c_str(), days_in_num[i]);
  }
  for (uint8_t i = 0; i < 24; i++) {
    String hours_key = "hours_key" + String(i);
    hours_num[i] = preferences.getBool(hours_key.c_str(), hours_num[i]);
  }
  preferences.end();

#ifdef DEBUG
  Serial.print("timeschen: ");
  Serial.println(timeschen);
#endif
  Beep(200, 1);
}


void loop() {

  /* This code is for time update after every 5 seconds*/
  wait_update_time = millis();
  if (wait_update_time - previousMillis_2 >= 5000) {
    updateTimeNow();
    previousMillis_2 = wait_update_time;
#ifdef DEBUG
    Serial.println("------------------------ Time Updated ----------------------");
#endif
  }

  /*--------------- This code is for Get data from Mobile APP -------------*/
  wait_time = millis();
  if (wait_time - previousMillis_1 >= 5000) {
    previousMillis_1 = wait_time;
    // Serial.print("Stations connected: ");
    // Serial.println(WiFi.softAPgetStationNum());
    if (!client.connected()) {
      reconnect();
    }

    publishJson();

    /* ----------------------------------------------------------------------------------------------- */
  }


  beca_check();

  if (cfmbutton != lst_cfmbutton) {
    if (bp < 11) {
      bp++;
      lst_cfmbutton = cfmbutton;
    } else {
      bp = 1;
      lst_cfmbutton = cfmbutton;
    }

    if (bp == 11 && cfm_flag == false) {
      cfm_flag = true;
      save_setpointt = setpointt * 10;
#ifdef DEBUG
      Serial.print("Save Setpoint: ");
      Serial.println(save_setpointt);
#endif
      cfm_duration = millis();

      preferences.begin("CFM", false);
      end_value = preferences.getInt("cfm_max", cfm);
      preferences.end();
      setpointt = (end_value / 10) + 20;
      if (setpointt > 20) {
        if (setpointt == 30) {
          display.clearDisplay();
          display_on_OLED(&Org_01, 5, 1, 40, "100%");

        } else if (setpointt > 30) {

          display.clearDisplay();
          display_on_OLED(&Org_01, 5, 1, 40, "100%");

          writeSingleRegister(1, 0x03, 30 * 10);
        } else {
          int8_t cfm_perc = setpointt % 10;
          display.clearDisplay();
          display_on_OLED(&Org_01, 5, 10, 40, String(cfm_perc) + "0 %");
        }
      }
#ifdef DEBUG
      Serial.print("setpointt: ");
      Serial.println(setpointt);
#endif

      Beep(50, 3);
      writeSingleRegister(1, 0x03, setpointt * 10);
      bp = 1;

    }  //
    else if (bp == 6 && cfm_flag == true) {
      last_setpoint = save_setpointt / 10;
      setpointt = last_setpoint;
      Beep(1000, 1);
      preferences.begin("CFM", false);
      preferences.putInt("cfm_max", end_value);
      preferences.end();
      supcfm = String(start_value) + "-" + String(end_value);
      bp = 1;
      // CFM_s = cfm_max;
      previousMillis_3 = millis();
      writeSingleRegister(1, 0x03, save_setpointt);
      cfm_flag = false;
    }
  }

  if (cfm_flag == true && millis() - cfm_duration >= 10000) {
    previousMillis_3 = millis();

    cfm_flag = false;
    last_setpoint = save_setpointt / 10;
    setpointt = last_setpoint;
    Beep(250, 5);
    writeSingleRegister(1, 0x03, save_setpointt);
    cfm_duration = millis();
    bp = 1;
  }  //
  else if ((cfm_flag == false && millis() - cfm_button_time >= 20000) && bp != 1) {
    cfm_flag = false;
    Beep(250, 1);
    cfm_button_time = millis();
    bp = 1;
  }

  if (cfm_flag == true && beca_power == true) {

    if (setpointt > 20 && setpointt < 31) {
      end_value = (setpointt - 20) * 10;
      CFM_max = map(end_value, 0, 100, servo_close_pos, servo_open_pos);
      MoveServo(CFM_max, 1, servo_delay);
    }  //
    else if (setpointt < 21) {
      end_value = 10;
      CFM_max = map(cfm, 0, 100, servo_close_pos, servo_open_pos);
      MoveServo(CFM_max, 1, servo_delay);
    }

    unsigned long currentMillis = millis();
    // Check if the value of register[3] has changed
    if (setpointt != last_setpoint) {
      cfm_duration = millis();
      if (setpointt > 20) {
        if (setpointt == 30) {
          display.clearDisplay();
          display_on_OLED(&Org_01, 5, 1, 40, "100%");

        } else if (setpointt > 30) {
          int8_t cfm_perc = setpointt % 10;
          display.clearDisplay();
          display_on_OLED(&Org_01, 5, 1, 40, "100%");
          writeSingleRegister(1, 0x03, 30 * 10);
        } else {

          int8_t cfm_perc = setpointt % 10;
          // Serial.println(cfm_perc);
          display.clearDisplay();
          display_on_OLED(&Org_01, 5, 10, 40, String(cfm_perc) + "0 %");
        }
      } else {  // Write 21 on BECA if get lesser than 21

        display.clearDisplay();
        display_on_OLED(&Org_01, 5, 10, 40, "10 %");

        writeSingleRegister(1, 0x03, 21 * 10);
      }
      last_setpoint = setpointt;
      previousMillis = currentMillis;
    }
  }  //
  else {
    /*-----------------------Main Logic----------------------------------------*/
    if (beca_power == 0) {  // BECA OFF
      dampertsw = 0;
      dampstate = 0;
      MoveServo(servo_close_pos, 1, servo_delay);
      if (show_time == false) {
        display.clearDisplay();
        display_on_OLED(&Org_01, 6, 10, 43, "OFF");
      }


    } else if (beca_power == 1) {
      if (dampertsw != 1) {  // No Auto Mode
        dampertsw = 1;
        // dampstate = 1;
        ControlDamper_wrt_mode();
      }                                          //
      else if (dampertsw == 1) {                 // Auto Mode
        if (timeschen == 1) {                    // Schedule Enable
          if (days_in_num[weekDay] == 1) {       // Matched Day Number
            if (hours_num[current_hour] == 1) {  // Matched Hour Number
              // dampstate = 1;
              uint16_t currentMillis = millis();
              ControlDamper_wrt_mode();
            }       //
            else {  // Hour Doesnot Matched
              dampstate = 0;
              MoveServo(servo_close_pos, 1, servo_delay);
            }
          }       //
          else {  // Day Doesnot Matched
            dampstate = 0;
            MoveServo(servo_close_pos, 1, servo_delay);
          }
        }       //
        else {  // Schedule Disable
          // dampstate = 1;
          ControlDamper_wrt_mode();
        }
      }
    }
  }
  client.loop();
}

void beca_check() {
  beca_status = readHoldingRegisters(MODBUS_SLAVE_ID, MODBUS_REGISTER_ADDRESS, MODBUS_REGISTER_COUNT, registers);
  if (prev_beca_status != beca_status) {
    if (prev_beca_status == 0) {
      Beep(100, 2);
      previousMillis_3 = millis();
      // prev_beca_status = beca_status;
    }
  }

  if (beca_status) {
    cfmbutton = registers[1];
    beca_power = registers[0];
    beca_mode = registers[2];
    setpointt = registers[3] / 10;
    temp_by_beca = registers[8] / 10;
  } else {
    cfmbutton = 0;
    beca_power = 0;
    beca_mode = 0;
    setpointt = 0;
    temp_by_beca = 0;

    Beep(150, 5);

    beca_status = readHoldingRegisters(MODBUS_SLAVE_ID, MODBUS_REGISTER_ADDRESS, MODBUS_REGISTER_COUNT, registers);

    while (beca_status == false) {
      beca_status = readHoldingRegisters(MODBUS_SLAVE_ID, MODBUS_REGISTER_ADDRESS, MODBUS_REGISTER_COUNT, registers);

      client.loop();

      display.clearDisplay();
      display_on_OLED(&Org_01, 5, 2, 43, "Error");


      /*--------------- This code is for Get data from Mobile APP -------------*/
      wait_time = millis();
      if (wait_time - previousMillis_1 >= 5000) {
        previousMillis_1 = wait_time;
        if (!client.connected()) {
          reconnect();
        }

        publishJson();

        /* ----------------------------------------------------------------------------------------------- */
      }

      if (dampertsw == 1) {
        beca_power = 1;
        dampertsw = 1;
        dampstate = 1;
        setpointt = 0;
        temp_by_beca = 0;
        MoveServo(CFM_max, 1, servo_delay);
      } else {
        beca_power = 0;
        dampertsw = 0;
        dampstate = 0;
        setpointt = 0;
        temp_by_beca = 0;
        MoveServo(servo_close_pos, 1, servo_delay);
      }

      /*------------------------Update Time---------------------------------------*/

      wait_update_time = millis();
      if (wait_update_time - previousMillis_2 >= 60000) {
        updateTimeNow();
        previousMillis_2 = wait_update_time;
#ifdef DEBUG
        Serial.println("------------------------ Time Updated ----------------------");
#endif
      }

      if (millis() - previousMillis_3 >= 10000 && cfm_flag == false) {
        bool show_flag = true;
        if (show_flag == true && show_time == false) {
          previousMillis_4 = millis();
          show_flag = false;
        }
        show_time = true;
      }
    }
  }
}

void ControlDamper_wrt_mode() {
  unsigned long currentMillis = millis();
  int16_t x[3] = { 10, 80, 95 };
  int16_t y[3] = { 43, 18, 43 };

  if (setpointt != last_setpoint) {
    previousMillis_3 = millis();

    display.clearDisplay();
    if (setpointt < 10) {
      display_temp_on_OLED(&Org_01, 9, 1, x, y, "0" + String(setpointt), "", "");
    } else {
      display_temp_on_OLED(&Org_01, 9, 1, x, y, String(setpointt), "", "");
    }

    setpoint_flag = true;

    last_setpoint = setpointt;
    previousMillis = currentMillis;
    show_time = false;

  } else if ((currentMillis - previousMillis >= interval && show_time == false)) {
    display.clearDisplay();
    display_temp_on_OLED(&Org_01, 6, 1, x, y, String(temp_by_beca), "o", "C");
    setpoint_flag = false;
  }


  /*---------------------- Cool Mode ---------------------*/
  if (beca_mode == 0 || beca_mode == 2) {
    if (last_setpoint > temp_by_beca && beca_power == true) {
      MoveServo(servo_close_pos, 1, servo_delay);
      dampstate = 0;
    } else if (last_setpoint <= temp_by_beca && beca_power == true) {
      MoveServo(CFM_max, 1, servo_delay);
      dampstate = 1;
    }
  }
  /*---------------------- Heat Mode ---------------------*/
  else if (beca_mode == 1) {
    if (last_setpoint <= temp_by_beca && beca_power == true) {
      MoveServo(servo_close_pos, 1, servo_delay);
      dampstate = 0;
    } else if (last_setpoint > temp_by_beca && beca_power == true) {
      MoveServo(CFM_max, 1, servo_delay);
      dampstate = 1;
    }
  }
}
