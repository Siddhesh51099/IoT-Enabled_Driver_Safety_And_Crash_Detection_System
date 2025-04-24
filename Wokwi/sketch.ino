#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <math.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// OLED Display Setup
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// MPU6050 Accelerometer Setup
Adafruit_MPU6050 mpu;

// WiFi Credentials
const char* ssid = "Wokwi-GUEST";
const char* password = "";
const char* thingSpeakApiKey = "OVXZPKJJZ1DU9U62";

// Twilio Credentials
const char* accountSid = "AC3c813e8d880d0280029f10ed31030ead";
const char* authToken  = "f9b8077bce87558975c6f589c9e6dbac"; 
const char* twilioNumber = "+18445412757";
const char* destinationNumber = "+18777804236";

// Sensor Pins
const int seatbeltSwitch = 2;   // Seatbelt Switch
const int alcoholSensor = 34;   // Potentiometer for Alcohol Detection
const int buzzerPin = 25;       // Buzzer Alert
const int ledPin = 26;          // LED Alert
const int crashButtonPin = 23;  // Crash Detection Push Button

bool crashState = false;
bool lastCrashButtonState = HIGH;

void setup() {
    Serial.begin(115200);
    Wire.begin();

    // Initialize OLED Display
    if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
      Serial.println("SSD1306 allocation failed");
      while (1);
    }

    // Initialize MPU6050
    if (!mpu.begin()) {
      Serial.print("MPU6050 not found!");
      while (1);
    }

    // Configure Pins
    pinMode(seatbeltSwitch, INPUT_PULLUP);
    pinMode(alcoholSensor, INPUT);
    pinMode(buzzerPin, OUTPUT);
    pinMode(ledPin, OUTPUT);
    pinMode(crashButtonPin, INPUT_PULLUP);

    // Connect to WiFi
    Serial.print("Connecting to WiFi...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
    }
    Serial.print("WiFi Connected!");

    oled.clearDisplay();
    oled.setTextSize(1);
    oled.setTextColor(SSD1306_WHITE);
}

void loop() {
    int seatbelt = digitalRead(seatbeltSwitch);
    float alcoholLevel = (analogRead(alcoholSensor) * 0.5) / 4095;
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    float accel = sqrt(a.acceleration.x * a.acceleration.x + 
                       a.acceleration.y * a.acceleration.y + 
                       a.acceleration.z * a.acceleration.z);
    int crashButtonState = digitalRead(crashButtonPin);

    if (lastCrashButtonState == HIGH && crashButtonState == LOW) {
      crashState = !crashState;
      delay(50);
    }
    lastCrashButtonState = crashButtonState;
    
    Serial.println("\nSafety Checks:");
    Serial.print("Seatbelt: "); Serial.println(seatbelt == LOW ? "Fastened" : "Unfastened");
    Serial.print("Alcohol Level: "); Serial.print(alcoholLevel); Serial.println(" %");
    Serial.print("Speed: "); Serial.print(accel); Serial.println(" mph");

    // Display Status on OLED
    oled.clearDisplay();
    oled.setTextSize(1);
    oled.setCursor(0, 0);
    oled.println("Safety Checks:");
    oled.print("Seatbelt: "); oled.println(String(seatbelt == LOW ? "Fastened" : "Unfastened"));
    oled.print("Alcohol Level: "); oled.print(String(alcoholLevel)); oled.println(" %");
    oled.print("Speed: "); oled.print(String(accel)); oled.println(" mph");
    oled.println("");

    if (seatbelt == HIGH || alcoholLevel > 0.08 || accel > 30 || (accel == 0 && crashState)) {
      if (seatbelt == HIGH) oled.println("Fasten Seatbelt!");
      if (alcoholLevel > 0.08) oled.println("High Alcohol Level!");
      if (accel > 30) oled.println("Speed Limit Exceeded!");
      if (accel == 0 && crashState) oled.println("Crash Detected!");
      digitalWrite(buzzerPin, HIGH);
      tone(buzzerPin, 2000, 500);
      digitalWrite(ledPin, HIGH);
    } else {
      oled.println("Safe to Drive!");
      digitalWrite(buzzerPin, LOW);
      digitalWrite(ledPin, LOW);
    }

    bool crashDetected = (accel == 0 && crashState);

    float currentLatitude, currentLongitude;
    getSimulatedGPS(currentLatitude, currentLongitude);

    if (crashDetected) {
      sendToThingSpeak(seatbelt, alcoholLevel, accel, crashDetected, currentLatitude, currentLongitude);
    } else {
      sendToThingSpeak(seatbelt, alcoholLevel, accel, crashDetected, 0.0, 0.0);
    }

    if (crashDetected) {
      String message = "\nCrash Detected!\nLatitude: " +
                      String(currentLatitude, 6) + "\nLongitude: " +
                      String(currentLongitude, 6);
      Serial.println(message);
      sendSMS(message);      
    }
    
    oled.display();
    delay(1000);
}

void getSimulatedGPS(float &latitude, float &longitude) {
  float baseLat = 43.0387;
  float baseLon = -76.1337;
  
  // Generating small random offsets
  latitude = baseLat + (random(-100, 100) / 10000.0);
  longitude = baseLon + (random(-100, 100) / 10000.0);
}

void sendSMS(String message) {
  WiFiClientSecure client;
  client.setInsecure();
  
  HTTPClient https;
  // Construct the URL for sending SMS via Twilio's API
  String url = "https://api.twilio.com/2010-04-01/Accounts/";
  url += accountSid;
  url += "/Messages.json";
  
  // Begin HTTPS connection with authorization
  https.begin(client, url);
  https.setAuthorization(accountSid, authToken); // HTTPClient can handle basic auth
  
  // Prepare POST parameters
  String postData = "To=" + String(destinationNumber) +
                    "&From=" + String(twilioNumber) +
                    "&Body=" + message;
  https.addHeader("Content-Type", "application/x-www-form-urlencoded");
  
  int httpResponseCode = https.POST(postData);
  
  if (httpResponseCode > 0) {
    Serial.printf("SMS sent!);
  } else {
    Serial.printf("Error sending SMS: %s\n", https.errorToString(httpResponseCode).c_str());
  }
  https.end();
}

void sendToThingSpeak(int seatbelt, float alcohol, float accel, int crashDetected, float latitude, float longitude) {
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      String url = "http://api.thingspeak.com/update?api_key=" + String(thingSpeakApiKey) +
                    "&field1=" + String(seatbelt) +
                    "&field2=" + String(alcohol) +
                    "&field3=" + String(accel) +
                    "&field4=" + String(crashDetected) +
                    "&field5=" + String(latitude) +
                    "&field6=" + String(longitude)         ;     
      http.begin(url);
      int httpResponseCode = http.GET();
      http.end();
    }
}