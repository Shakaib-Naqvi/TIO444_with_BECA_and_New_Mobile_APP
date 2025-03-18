// Load certificates
const char* root_ca =
  "-----BEGIN CERTIFICATE-----\n"
  "MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF\n"
  "ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6\n"
  "b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL\n"
  "MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv\n"
  "b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj\n"
  "ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM\n"
  "9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw\n"
  "IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6\n"
  "VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L\n"
  "93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm\n"
  "jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC\n"
  "AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA\n"
  "A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI\n"
  "U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs\n"
  "N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv\n"
  "o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU\n"
  "5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy\n"
  "rqXRfboQnoZsG4q5WTP468SQvvG5\n"
  "-----END CERTIFICATE-----\n";


const char* client_cert =
  "-----BEGIN CERTIFICATE-----\n"
  "MIIDWTCCAkGgAwIBAgIUC/EFqW/3hnn9TzBouCVpVxNg1scwDQYJKoZIhvcNAQEL\n"
  "BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g\n"
  "SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTI1MDMxMTIxMzE0\n"
  "MVoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0\n"
  "ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALapCOlph9/siQQyn8C6\n"
  "kUb3/8vKPUuBqCZUZCSEb534Zhi0x2rIy7xm862G9mQBizmG09IKwsu431/Wq1UV\n"
  "QKE3nrTvrW3vJR5wmZwhORUHoK0RzHK8EKpDDbTAR0arVJnSrF+9BoEx+hAJBu+Q\n"
  "cd7Ds3MsdWleFg8pxmsZqv/Dy4B27WAx/pqdLK73ZI1O/PyAFqHvZMA3KWL9uHfv\n"
  "lKNaJ8Hdi/9ccmYcLll/Hedlx9jKaOTgI5JGhcCvK6F6adNlTJqpkM4Tx0BtOISf\n"
  "aVSGqa8aFScPBNTH0BzWqbSKxNTPZCgh5Kyp8MUiZeQEU7XXchlQ1kMoa8CyQ0Fc\n"
  "HVcCAwEAAaNgMF4wHwYDVR0jBBgwFoAUy5EwFoFzTpwUAsV+0wIy+jio9ZMwHQYD\n"
  "VR0OBBYEFFq6tzOMOOtYZ44Rcu0zaiDMn7GJMAwGA1UdEwEB/wQCMAAwDgYDVR0P\n"
  "AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQC37lqd2TwnuNlHBehJH1xUUS0U\n"
  "18dYrNOtAdPJcs5M5+B+n/AkCWlpJGnABzepmAqNhr7KVQ7neWYEEVoHpTjBdnES\n"
  "ZxD8sXyL9Ie/wL8Lx9jqIZM24EQJWfvnJoL3mL9eEeTMDJ7WDshZfpkqVP4BFkP5\n"
  "KD6Pjl54tFzRgI6fbzyb+Jxm2PFdq9lyCslGho7FO7WSGxqb4lQi71ckkzYuzWhK\n"
  "tW+/7hEeRjuCHQly/5bE+UL6Rv+xCeb4VLYKSMeKkLwbYMHNENU1wmO+zVfYP/9m\n"
  "Xq3hB4w0yFljJogzno+r021jjlFAMT48beI9QffE+e6PpAGIehSA4NJHTN1z\n"
  "-----END CERTIFICATE-----\n";

const char* client_key =
  "-----BEGIN RSA PRIVATE KEY-----\n"
  "MIIEowIBAAKCAQEAtqkI6WmH3+yJBDKfwLqRRvf/y8o9S4GoJlRkJIRvnfhmGLTH\n"
  "asjLvGbzrYb2ZAGLOYbT0grCy7jfX9arVRVAoTeetO+tbe8lHnCZnCE5FQegrRHM\n"
  "crwQqkMNtMBHRqtUmdKsX70GgTH6EAkG75Bx3sOzcyx1aV4WDynGaxmq/8PLgHbt\n"
  "YDH+mp0srvdkjU78/IAWoe9kwDcpYv24d++Uo1onwd2L/1xyZhwuWX8d52XH2Mpo\n"
  "5OAjkkaFwK8roXpp02VMmqmQzhPHQG04hJ9pVIaprxoVJw8E1MfQHNaptIrE1M9k\n"
  "KCHkrKnwxSJl5ARTtddyGVDWQyhrwLJDQVwdVwIDAQABAoIBAHrieWZeYtTY0s0K\n"
  "KcOFQFtYWLSHWHlFvxQaTkzq9BR4mmcgp9BFShtzv5gMZhKdn0aSWErEhox70Xsu\n"
  "dpGE/Lf5LUJYxHpjGrvB0PXiu/5T5VrJ0JuXvjZtafkiKlF2zjG2M9Us3AVq0+qZ\n"
  "yBq/OHw/eKiRTmQWsgx9dEl1OT9bGtdD/bh6e155mdVaSATpsB/KYhDMftz4Kh3/\n"
  "DYNoM8dqAbNFxbftTrgGsrgC/T38WoKm1AI5z1QHNrtczsN8bVhU6yuWtU2dGQWD\n"
  "s7hNgpHYvbgEIhTNgvf2vT4RviMtoAQ0EDqXZqwEsZFAczNSDc/BmQrio9YXHznE\n"
  "bEhA41ECgYEA8qL2/MSccNqKQcSq66HkiltNTjRdmoUdToyTdHEEnHzyBSSzg6Lv\n"
  "z7kDQD9dNfT+TLq81jL9um/FOuefCxt6kIpVZJRL8z+94qsBZZiJ+0IvWoNBubob\n"
  "AvOnzD0yfaxEhXHtAE3nKfKMXgi0AU73d4EpjgWYWZxXaaliJM5cpHUCgYEAwLhw\n"
  "aHhrh0+0Y6JnhZIGDPzOq+OS1kmxLCXre2qgGJAkDjzsA/wdcf/92FvxW6kwZkFb\n"
  "9E3pmfp2LLiwkO6Kc7qM9yc09Fu/qCjise5Fd12PV7a4oYONvIUzimC1sbkT6V9m\n"
  "bESOftUhUJpBVVYRWQHye2L0kP4gR6JCiCqfERsCgYAtGAR3LcM1ZihT2M07RbdH\n"
  "z3gqlKjg0uSDeLTe6zJEMyR3uD50tI+FN4lXI2+bW5D3ia0W0hs9zxAExo9UbSL2\n"
  "Qf9k1frXln0f51A3JYZfYAmU9Nf+QIxMnCQPXUBJAv8pHedCKzhPH3je8RcjNx3e\n"
  "4+5pKrkJznigdo568K9fEQKBgFWXFEUxhf/4RBMj43oM2icWd+sbDPGilM8YoDaV\n"
  "qjh+e6TfJaq3Y5RnrqNSYiTlRRuE14PuvlqmQ6mk9LXJWy/+n/B8NyZ3QO08C0Ie\n"
  "ojdbE/hOrDz/Igmh1rwUK12c5tz0g5Z99BMcMMmNWIq/yMCQ/tIRprBmTIvD4mx7\n"
  "EV4VAoGBAK86mTS3m2wcbM3qJChAKQbzaPHcV3K/5sQj4z93T2T8DxB4t7S9S1EO\n"
  "9W+ikfL2/7XxOncxupMlHjdqg/L8fcWUNBMSwu5TJ9G1VLuM0pVFBirbqbpe7rsd\n"
  "5x+ZtAkt7jbkshteXxWYGR/P5aoi1dhDW+ZW5FUQM3Ztp5bZ2N24\n"
  "-----END RSA PRIVATE KEY-----\n";



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
  if (doc.containsKey("seasonsw")) {
    seasonsw = doc["seasonsw"].as<int>();
  }
  if (doc.containsKey("dmptempsp")) {
    dmptempsp = doc["dmptempsp"].as<int>();
  }
  if (doc.containsKey("dampertsw")) {
    dampertsw = doc["dampertsw"].as<int>();
  }
  if (doc.containsKey("supcfm")) {
    supcfm = doc["supcfm"].as<String>();
  }
  if (doc.containsKey("dampstate")) {
    dampstate = doc["dampstate"].as<int>();
  }

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

    espClient.setCACert(root_ca);
    espClient.setCertificate(client_cert);
    espClient.setPrivateKey(client_key);

    client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

#ifdef DEBUG
    Serial.print("Attempting AWS MQTT connection...");
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

