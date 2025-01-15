#include <WiFi.h>
#include <HTTPClient.h>

#include "time.h"

// const char* ssid = "BITA HOMES";
// const char* password = "xttok2fb";
String Name = "ZoneMaster";
String myID = "00000000000";
String substrin1 = myID.substring(2, 6);
String substrin2 = myID.substring(9, 11);
String hostname = Name + substrin1 + substrin2;
String devicename = "ZMB-AAA017";
String savedatacommand = "";
int device_type = 3;
String devversion = "1.0";
String macaddress = "";
long disconnection_counter = 300;  //Once phone is connected to BLE, keep the connection for 300 seconds.
long switchblecounter = 3;         //Try WIFI 10 times before turning on BLE


String dataarray = "";

uint8_t seasonsw = 0;
int8_t dmptemp = 24;
int8_t dmptempsp = 24;
int8_t prevdmptempsp = 24;
uint8_t dampertsw = 0;
uint8_t prevdampertsw = 1;
uint8_t start_value;
uint8_t end_value;

// String supcfm = "0-50";

String supcfm = String(start_value) + "-" + String(end_value);
String retcfm = "0-50";
String timesch = "hoursch=24|daysch=7";
bool timeschen = 0;
bool dampstate = 0;
String alarm_string = "0";
String timenow = "010120120000";
String myIP = "";
long packet_sequence = 0;
String lastcmd;

String keys[] = { "seasonsw", "dmptemp", "dmptempsp", "dampertsw", "supcfm", "retcfm", "hoursch", "timeschen", "dampstate", "alarm", "timenow", "ip_address", "packet_id" };
uint8_t numKeys = sizeof(keys) / sizeof(keys[0]);  // Get the number of keys

const char* ntpServer = "time.google.com";
const long gmtOffset_sec = 18000;
const int daylightOffset_sec = 0;

uint8_t mm = 1;      //month
uint8_t dd = 1;      //date
uint16_t yy = 2020;  //year
uint8_t hh = 12;     //hour
uint8_t mins = 0;    //minute
uint8_t ss = 10;     //second
String month = "Jan";
uint8_t weekDay;
uint8_t current_hour;
bool days_in_num[7];
bool hours_num[24];


// server.on("/getinfo", HTTP_GET, [](AsyncWebServerRequest *request) {
//     Serial.println("Get Info Called");


//     String deviceinfo;
//     DynamicJsonDocument jsonBufferinfo(200);
//     jsonBufferinfo["mac_address"] = macaddress;
//     jsonBufferinfo["slave_mac_address"] = ""; // Update if necessary
//     jsonBufferinfo["type"] = device_type;
//     jsonBufferinfo["vers"] = devversion;
//     jsonBufferinfo["id"] = myID;
//     jsonBufferinfo["disconnection_counter"] = disconnection_counter;
//     jsonBufferinfo["device_timeout_counter"] = switchblecounter;
//     jsonBufferinfo["wifi_channel"] = wifi_channel;
//     jsonBufferinfo["wifissid"] = ssid;
//     jsonBufferinfo["wifipassword"] = password;
//     serializeJson(jsonBufferinfo, deviceinfo);

//     request->send(200, "application/json", deviceinfo);
//   });









void updateTimeNow() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
#ifdef DEBUG
    Serial.println("Failed to obtain time");
#endif
    return;
  }
  // Format the time into WMMDDYYHHMMSS
  char buffer[16];

  // Convert weekday to numeric value (1=Monday, 2=Tuesday, ...)
  weekDay = timeinfo.tm_wday - 1;  // tm_wday is 0=Sunday, 1=Monday, ..., 6=Saturday
  current_hour = timeinfo.tm_hour;

  // Format the string with leading zeroes if needed
  snprintf(buffer, sizeof(buffer), "%02d%02d%02d%02d%02d%02d",
           timeinfo.tm_mday,
           timeinfo.tm_mon + 1,     // Month is 0-based, so add 1
           timeinfo.tm_year % 100,  // Year is years since 1900, so take the last two digits
           timeinfo.tm_hour,
           timeinfo.tm_min,
           timeinfo.tm_sec);

  timenow = String(buffer);
  mm = timeinfo.tm_mon + 1;
  dd = timeinfo.tm_mday;
  yy = timeinfo.tm_year % 100;
  hh = timeinfo.tm_hour;
  mins = timeinfo.tm_min;
  ss = timeinfo.tm_sec;
#ifdef DEBUG
  Serial.print("Sending timenow: ");
  Serial.println(timenow);
#endif
  switch (mm) {
    case 1:
      month = "Jan";
      break;
    case 2:
      month = "Feb";
      break;
    case 3:
      month = "Mar";
      break;
    case 4:
      month = "Apr";
      break;
    case 5:
      month = "May";
      break;
    case 6:
      month = "Jun";
      break;
    case 7:
      month = "Jul";
      break;
    case 8:
      month = "Aug";
      break;
    case 9:
      month = "Sep";
      break;
    case 10:
      month = "Oct";
      break;
    case 11:
      month = "Nov";
      break;
    case 12:
      month = "Dec";
    default:
      month = "Jan";
      break;
  }
}

void parseJson(const String& json) {

  // payload: {"status":200,"device_id":"16092100124","msg":"Data updated successfully","lastcmd":"seasonsw=0","packet_id":7}

  // Extract values (manual parsing)

  // Locate the 'lastcmd' key
  int startIndex = json.indexOf("\"lastcmd\":");

  if (startIndex != -1) {
    int valueStart = startIndex + String("\"lastcmd\":\"").length();
    int valueEnd = json.indexOf("\"", valueStart);

    // Extract the value as a substring
    lastcmd = json.substring(valueStart, valueEnd);

// Print the extracted value
#ifdef DEBUG
    Serial.println("Deserialized value:");
    Serial.print("lastcmd: ");
    Serial.println(lastcmd);
#endif
  } else {
#ifdef DEBUG
    Serial.println("Field 'lastcmd' not found in JSON");
#endif
  }
}


void hour_day_datetime_now(String hours_ch, String days_ch, String time_now) {
  int8_t length_of_hours = 0;

  for (uint8_t i = 0; i < hours_ch.length(); i++) {
    if (hours_ch.charAt(i) == ',') {
      length_of_hours++;
    }
  }
  length_of_hours = length_of_hours + 1;
  int8_t hours_start = 0;
  int8_t hours_end = hours_ch.indexOf(',');

  for (uint8_t i = 0; i < 24; i++) {
    hours_num[i] = false;
  }

  for (uint8_t i = 0; i < length_of_hours; i++) {
    if (hours_end == -1) {
      // Handle the last element in the string
      hours_end = hours_ch.length();
    }
    int8_t hoursnum = hours_ch.substring(hours_start, hours_end).toInt();
    hours_start = hours_end + 1;
    hours_end = hours_ch.indexOf(',', hours_start);
    hours_num[hoursnum] = true;
  }

  int8_t length_of_days_ch = 1;
  for (uint8_t i = 0; i < 7; i++) {
    days_in_num[i] = false;
  }
  // days_in_num[] = { false, false, false, false, false, false, false };
  //  { 0 = Monday, 1 = Tuesday, 2 = Wednesday, 3 = Thursday, 4 = Friday, 5 = Saturday, 6 = Sunday }

  // Calculate the number of days in the string
  length_of_days_ch = 1;
  for (uint8_t i = 0; i < days_ch.length(); i++) {
    if (days_ch.charAt(i) == ',') {
      length_of_days_ch++;
    }
  }

  int8_t day_start = 0;
  int8_t day_end = days_ch.indexOf(',');

  for (int8_t i = 0; i < length_of_days_ch; i++) {
    if (day_end == -1) {
      // Handle the last element in the string
      day_end = days_ch.length();
    }
    int8_t daynum = days_ch.substring(day_start, day_end).toInt();
    day_start = day_end + 1;
    day_end = days_ch.indexOf(',', day_start);
    days_in_num[daynum] = true;
  }

  preferences.begin("timeenable", false);
  for (int8_t i = 0; i < 24; i++) {
    String hours_key = "hours_key" + String(i);
    preferences.putBool(hours_key.c_str(), hours_num[i]);
  }
  for (int8_t i = 0; i < 7; i++) {
    String days_key = "days_key" + String(i);
    preferences.putBool(days_key.c_str(), days_in_num[i]);
  }
  preferences.end();
}

// // Function to extract values for specific keys from lastcmd field
// String extractValues(const String& lastcmd, String keys[], int8_t numKeys) {
//   String result = "";
//   String value_by_app = "";

//   for (uint8_t i = 0; i < numKeys; i++) {

//     String key = keys[i];
//     String key_with_equals = key + "=";
//     int8_t keyStart = lastcmd.indexOf(key_with_equals);

//     if (keyStart != -1) {
//       int8_t valueStart = lastcmd.indexOf('=', keyStart);

//       if (valueStart != -1) {
//         int8_t valueEnd = lastcmd.indexOf(';', valueStart);

//         if (valueEnd == -1) {
//           valueEnd = lastcmd.length();  // Value extends to the end if no semicolon
//         }

//         value_by_app = lastcmd.substring(valueStart + 1, valueEnd);
//         result += key + " = " + value_by_app + "\n";
//         update_from_pref = true;

//         if (key == "seasonsw") {
//           seasonsw = value_by_app.toInt();
//           // if (prevseasonsw != seasonsw) {
//           if (seasonsw == 1) {
//             if (beca_mode != 1) {
//               writeSingleRegister(1, 0x02, 1);
//             }
//             // prevseasonsw = seasonsw;
//           } else if (seasonsw == 0) {
//             if (beca_mode != 0) {
//               writeSingleRegister(1, 0x02, 0);
//             }
//             // prevseasonsw = seasonsw;
//           }
//           // }
//         } else if (key == "dmptempsp") {
//           dmptempsp = value_by_app.toInt();
//           if (prevdmptempsp != dmptempsp) {
//             if (dmptempsp > 5 && dmptempsp < 35) {
//               writeSingleRegister(1, 0x03, dmptempsp * 10);
//             } else if (dmptempsp >= 35) {
//               writeSingleRegister(1, 0x03, 35 * 10);
//             } else if (dmptempsp <= 5) {
//               writeSingleRegister(1, 0x03, 5 * 10);
//             }
//             prevdmptempsp = dmptempsp;
//           }
//         } else if (key == "dampertsw") {
//           dampertsw = value_by_app.toInt();  // 0

// #ifdef DEBUG
//           Serial.print("Damper state changed to: ");
//           Serial.println(dampertsw);
// #endif

//           if (dampertsw == 0 && beca_power != 0) {
//             writeSingleRegister(1, 0x00, 0);
//           } else if (dampertsw == 1 && beca_power != 1) {
//             writeSingleRegister(1, 0x00, 1);
//           } else if (dampertsw == 0 && beca_power == 0) {
//             writeSingleRegister(1, 0x00, 0);
//           }
//         } else if (key == "supcfm") {
//           supcfm = value_by_app;
//           int dash_index = value_by_app.indexOf("-");
//           if (cfm_flag != true) {
//             if (dash_index != -1) {
//               start_value = value_by_app.substring(0, dash_index).toInt();
//               end_value = value_by_app.substring(dash_index + 1).toInt();
//               // CFM_min = map(start_value, 0, 100, servo_close_pos, servo_open_pos);
//               CFM_max = map(end_value, 0, 100, servo_close_pos, servo_open_pos);
//               preferences.begin("CFM", false);
//               preferences.putInt("cfm_min", start_value);
//               preferences.putInt("cfm_max", end_value);
//               preferences.end();
//               supcfm = String(start_value) + "-" + String(end_value);
// #ifdef DEBUG
//               Serial.print("supcfm: ");
//               Serial.println(supcfm);
// #endif
//             }
//           }
//         } else if (key == "retcfm") {
//           retcfm = value_by_app;
//         } else if (key == "hoursch") {

//           int8_t date_time_now = value_by_app.indexOf("|daysch=");
//           String hoursch = value_by_app.substring(0, date_time_now);

//           // Extract the "daysch" part (everything after "hoursch" but before "datetime_now")
//           String daysch = value_by_app.substring(date_time_now);

//           // Construct the new string
//           timesch = "hoursch=" + hoursch + daysch;

//           preferences.begin("timeenable", false);
//           preferences.putString("timesch", timesch);
//           preferences.end();

//           hoursch = value_by_app.substring(0, value_by_app.indexOf("|"));

// #ifdef DEBUG
//           Serial.print("hoursch:: ");
//           Serial.println(hoursch);
// #endif

//           int8_t days = value_by_app.indexOf("daysch=");
//           daysch;
//           if (days != -1) {
//             daysch = value_by_app.substring(days + 7, value_by_app.indexOf("|date"));
//           }

//           int8_t date_time = value_by_app.indexOf("datetime_now");
//           String datetime_now;
//           if (date_time != -1) {
//             datetime_now = value_by_app.substring(date_time + 13);
// #ifdef DEBUG
//             Serial.print("date_time_when_scheduled:: ");
//             Serial.println(datetime_now);
// #endif
//           }
//           hour_day_datetime_now(hoursch, daysch, datetime_now);
//         } else if (key == "timeschen") {
//           timeschen = value_by_app.toInt();
//           preferences.begin("timeenable", false);
//           preferences.putBool("timeschen", timeschen);

//         } else if (key == "dampstate") {
//           dampstate = value_by_app;
//         } else if (key == "alarm") {
//           alarm_string = value_by_app.toInt();
//         } else if (key == "timenow") {
//           timenow = value_by_app;
//         } else if (key == "packet_id") {
//           packet_sequence = value_by_app.toInt();
//         } else {
//         }
//       }
//     }
//   }

//   // If no keys are found, provide default message
//   if (result.length() == 0) {
//     result = "No values found.";
//   } else {
//     HTTPClient http;
//     String serverpath = "http://209.38.236.253/api/clearlastcmd?id=" + myID;
//     http.begin(serverpath);
//     int code_by_http_get = http.GET();
// #ifdef DEBUG
//     Serial.print("code_by_http_get: ");
//     Serial.println(code_by_http_get);
// #endif
//     if (code_by_http_get > 0) {
//       String payload = http.getString();
// #ifdef DEBUG
//       Serial.print("code_by_http_get: ");
//       Serial.println(code_by_http_get);
//       Serial.print("payload: ");
//       Serial.println(payload);
// #endif
//       parseJson(payload);
//     }
//   }
//   return result;
// }

// void get_api(String server_path) {

//   if (WiFi.status() == WL_CONNECTED) {

//     HTTPClient http;
//     http.begin(server_path);
//     int code_by_http_get = http.GET();

// #ifdef DEBUG
//     Serial.print("code_by_http_get: ");
//     Serial.println(code_by_http_get);
// #endif

//     if (code_by_http_get > 0) {
//       String payload = http.getString();
// #ifdef DEBUG
//       Serial.print("code_by_http_get: ");
//       Serial.println(code_by_http_get);
//       Serial.print("payload: ");
//       Serial.println(payload);
// #endif
//       parseJson(payload);
//       String value_by_get = extractValues(lastcmd, keys, numKeys);
// #ifdef DEBUG
//       Serial.println(value_by_get);
// #endif
//     }  //

//     else {
// #ifdef DEBUG
//       Serial.println("Error on HTTP request");
// #endif
//     }
//     http.end();
//   }
// }

// void makestring() {
//   dataarray = "";

//   dmptemp = temp_by_beca;
//   if (beca_mode == 0 || beca_mode == 2) {
//     seasonsw = 0;
//   } else {
//     seasonsw = 1;
//   }

//   // Constructing JSON for dataarray
//   dataarray += "{";
//   dataarray += "\"seasonsw\":" + String(seasonsw) + ",";
//   dataarray += "\"dmptemp\":\"" + String(dmptemp) + "\",";
//   dataarray += "\"dmptempsp\":\"" + String(setpointt) + "\",";
//   dataarray += "\"dampertsw\":" + String(beca_power) + ",";
//   dataarray += "\"supcfm\":\"" + String(supcfm) + "\",";
//   dataarray += "\"retcfm\":\"" + String(retcfm) + "\",";
//   dataarray += "\"timesch\":\"" + String(timesch) + "\",";
//   dataarray += "\"timeschen\":" + String(timeschen) + ",";
//   dataarray += "\"dampstate\":" + String(dampstate) + ",";
//   dataarray += "\"alarm\":\"" + alarm_string + "\",";
//   dataarray += "\"timenow\":\"" + String(timenow) + "\",";
//   dataarray += "\"ip_address\":\"" + myIP + "\",";
//   dataarray += "\"packet_id\":" + String(packet_sequence) + "";
//   dataarray += "}";
// #ifdef DEBUG
//   Serial.print("Data Array");
//   Serial.println(dataarray);
// #endif
// }

// void Extract_by_json(String incomingMessage) {
//   // Allocate memory for JSON document
//   StaticJsonDocument<512> doc;

//   // Deserialize the JSON message
//   DeserializationError error = deserializeJson(doc, incomingMessage);
//   if (error) {
//     Serial.print("JSON deserialization failed: ");
//     Serial.println(error.c_str());
//     return;
//   }

//   // Extract values into separate variables
//   bool seasonsw = doc["seasonsw"];
//   const char* dmptemp = doc["dmptemp"];
//   float dmptempsp = doc["dmptempsp"];
//   bool dampertsw = doc["dampertsw"];
//   const char* supcfm = doc["supcfm"];
//   const char* retcfm = doc["retcfm"];
//   const char* timesch = doc["timesch"];
//   int timeschen = doc["timeschen"];
//   int dampstate = doc["dampstate"];
//   const char* alarm = doc["alarm"];
//   const char* timenow = doc["timenow"];
//   const char* ip_address = doc["ip_address"];
//   int packet_id = doc["packet_id"];

//   // Print extracted values
//   Serial.println("Extracted Values:");
//   Serial.print("Season Switch: ");
//   Serial.println(seasonsw ? "true" : "false");
//   Serial.print("DMP Temp: ");
//   Serial.println(dmptemp);
//   Serial.print("DMP Temp SP: ");
//   Serial.println(dmptempsp);
//   Serial.print("Damper Switch: ");
//   Serial.println(dampertsw ? "true" : "false");
//   Serial.print("Supply CFM: ");
//   Serial.println(supcfm);
//   Serial.print("Return CFM: ");
//   Serial.println(retcfm);
//   Serial.print("Time Schedule: ");
//   Serial.println(timesch);
//   Serial.print("Time Schedule Enabled: ");
//   Serial.println(timeschen);
//   Serial.print("Damper State: ");
//   Serial.println(dampstate);
//   Serial.print("Alarm: ");
//   Serial.println(alarm);
//   Serial.print("Time Now: ");
//   Serial.println(timenow);
//   Serial.print("IP Address: ");
//   Serial.println(ip_address);
//   Serial.print("Packet ID: ");
//   Serial.println(packet_id);
// }