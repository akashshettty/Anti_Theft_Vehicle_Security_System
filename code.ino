#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>

// WiFi & ThingSpeak
const char* ssid = "Moto";
const char* password = "12345678";
const char* host = "api.thingspeak.com";
String apiKey = "1H1Q4Q02156JN3T1";

// Motor Pins (L298N)
#define MOTOR_IN1 D1  // GPIO5
#define MOTOR_IN2 D2  // GPIO4
#define MOTOR_ENA D4  // GPIO2 (Fixed from D3)

// GSM (SIM900A) on D6 (RX), D5 (TX)
SoftwareSerial sim900a(D6, D5);

// GPS (NEO-6M) on D7 (RX), D8 (TX)
SoftwareSerial gpsSerial(D7, D8);
TinyGPSPlus gps;

// System
const String PHONE_NUMBER = "+916363699022"; // Use verified number
unsigned long lastUpdate = 0;
bool motorRunning = false;

void setup() {
  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);
  pinMode(MOTOR_ENA, OUTPUT);

  Serial.begin(115200);
  sim900a.begin(9600);
  gpsSerial.begin(9600);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");

  delay(3000);

  // Optional: Check GSM signal strength
  sim900a.println("AT+CSQ");
  delay(1000);
  while (sim900a.available()) {
    Serial.println(sim900a.readString());
  }

  start_motor();
  delay(5000); // Wait for GPS to stabilize

  float lat, lon;
  get_gps_location(lat, lon);
  send_sms("ALERT: VEHICLE STARTED\nGPS: Lat: " + String(lat, 6) + ", Lon: " + String(lon, 6));
  send_to_thingspeak(1, lat, lon);

  lastUpdate = millis();
}

void loop() {
  check_sms();

  // Send updates every 90 seconds if running
  if (motorRunning && (millis() - lastUpdate >= 90000)) {
    float lat, lon;
    get_gps_location(lat, lon);
    send_sms("ALERT VEHICLE MOVED LOCATION UPDATE:\nGPS: Lat: " + String(lat, 6) + ", Lon: " + String(lon, 6));
    send_to_thingspeak(1, lat, lon);
    lastUpdate = millis();
  }
}

// Motor control
void start_motor() {
  digitalWrite(MOTOR_IN1, HIGH);
  digitalWrite(MOTOR_IN2, LOW);
  digitalWrite(MOTOR_ENA, HIGH);
  motorRunning = true;
}

void stop_motor() {
  digitalWrite(MOTOR_IN1, LOW);
  digitalWrite(MOTOR_ENA, LOW);
  motorRunning = false;
  float lat, lon;
  get_gps_location(lat, lon);
  send_sms("ALERT: VEHICLE STOPPED\nGPS: Lat: " + String(lat, 6) + ", Lon: " + String(lon, 6));
  send_to_thingspeak(0, lat, lon);
}

// GPS function (returns lat/lon as floats)
void get_gps_location(float &lat, float &lon) {
  unsigned long start = millis();
  lat = 0.0;
  lon = 0.0;
  while (millis() - start < 3000) {
    while (gpsSerial.available()) {
      gps.encode(gpsSerial.read());
    }
    if (gps.location.isValid()) {
      lat = gps.location.lat();
      lon = gps.location.lng();
      return;
    }
  }
}

// ✅ Fixed SMS Function
void send_sms(String message) {
  Serial.println("Sending SMS...");

  sim900a.println("AT+CMGF=1");  // Set SMS to text mode
  delay(1000);

  sim900a.print("AT+CMGS=\"" + PHONE_NUMBER + "\"");
  sim900a.write(13);  // Carriage return
  delay(1000);

  sim900a.print(message);
  delay(500);

  sim900a.write(26);  // CTRL+Z to send
  delay(5000);

  // GSM module response
  Serial.println("GSM Response:");
  while (sim900a.available()) {
    Serial.write(sim900a.read());
  }
}

// SMS Listener
void check_sms() {
  if (sim900a.available()) {
    String response = sim900a.readString();
    if (response.indexOf("STOP") != -1) {
      stop_motor();
    }
  }
}

// ThingSpeak update: status in field 1, latitude in field 2, longitude in field 3
void send_to_thingspeak(int status, float lat, float lon) {
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("ThingSpeak connection failed");
    return;
  }
  String url = "/update?api_key=" + apiKey +
               "&field1=" + String(status) +
               "&field2=" + String(lat, 6) +
               "&field3=" + String(lon, 6);
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
  delay(1000);
  while (client.available()) {
    String line = client.readStringUntil('\n');
    Serial.println(line);
  }
  client.stop();
}