
#include <HTTPClient.h>

#include "time.h"

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

