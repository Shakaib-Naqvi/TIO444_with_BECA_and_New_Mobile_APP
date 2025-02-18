const char* mqtt_server = "192.168.18.112";
// const char* mqtt_server = "192.168.18.112";


String Name = "ZoneMaster";
String myID = "00000000000";
String substrin1 = myID.substring(2, 6);
String substrin2 = myID.substring(9, 11);
String hostname = Name + substrin1 + substrin2;
String devicename = "ZMB-AAA016";
String savedatacommand = "";
int device_type = 3;
String devversion = "1.0";
String macaddress = "";

String device_topic_s = "/test/" + devicename + "/1"; // subscribe topic
String device_topic_p = "/KRC/" + devicename; // publish topic

const int Buzzer_Pin = 25;

int16_t servo_open_pos = 135;
int16_t servo_close_pos = 38;
int16_t last_pos_servo;
int8_t servo_delay = 35;
bool beca_status = false;
bool prev_beca_status = 1;
bool setpoint_flag = true;

unsigned long cfm_duration = 0;
unsigned long cfm_button_time = 0;
unsigned long previousMillis = 0;
unsigned long previousMillis_1 = 0;
unsigned long previousMillis_2 = 0;
unsigned long previousMillis_3 = 0;
unsigned long previousMillis_4 = 0;
unsigned long wait_update_time = 0;
unsigned long wait_time = 0;
bool show_time = false;
const uint16_t interval = 2000;
bool beca_power = false;
uint8_t beca_mode;
int16_t temp_by_beca;
int16_t setpointt;
uint16_t last_setpoint = 0;
int8_t cfmbutton = 0;
int8_t lst_cfmbutton = 0;
int8_t bp = 1;
int16_t cfm;
bool cfm_flag = false;
uint16_t save_setpointt = 0;
int8_t CFM_max = servo_open_pos;
int8_t new_cfm = servo_open_pos;
int8_t CFM_min = servo_close_pos;
int16_t minval, maxval;
bool update_from_pref = true;



String dataarray = "";

uint8_t seasonsw = 0;
int8_t dmptemp = 24;
int8_t dmptempsp = 24;
int8_t prevdmptempsp = 24;
uint8_t dampertsw = 0;
uint8_t prevdampertsw = 1;
uint8_t start_value;
uint8_t end_value = 10;
uint8_t end_value_app = 10;

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

// String keys[] = { "seasonsw", "dmptemp", "dmptempsp", "dampertsw", "supcfm", "retcfm", "hoursch", "timeschen", "dampstate", "alarm", "timenow", "ip_address", "packet_id" };
// uint8_t numKeys = sizeof(keys) / sizeof(keys[0]);  // Get the number of keys

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


enum displaystate {
  datee,
  timee
};
displaystate display_state = datee;

String data = "";

