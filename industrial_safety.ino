#include <WiFi.h>
#include <DHT.h>

// WiFi credentials
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

// Sensor Pins
#define DHTPIN 4
#define DHTTYPE DHT11
#define GAS_SENSOR 34
#define FLAME_SENSOR 35
#define LDR_SENSOR 32
#define BUZZER 25

DHT dht(DHTPIN, DHTTYPE);

WiFiServer server(80);

void setup() {
  Serial.begin(115200);

  pinMode(GAS_SENSOR, INPUT);
  pinMode(FLAME_SENSOR, INPUT);
  pinMode(LDR_SENSOR, INPUT);
  pinMode(BUZZER, OUTPUT);

  dht.begin();

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected!");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  float temp = dht.readTemperature();
  int gasValue = analogRead(GAS_SENSOR);
  int flameValue = digitalRead(FLAME_SENSOR);
  int ldrValue = analogRead(LDR_SENSOR);

  Serial.print("Temp: "); Serial.println(temp);
  Serial.print("Gas: "); Serial.println(gasValue);
  Serial.print("Flame: "); Serial.println(flameValue);
  Serial.print("LDR: "); Serial.println(ldrValue);

  // Threshold conditions
  if (gasValue > 2000 || flameValue == 0 || temp > 50) {
    digitalWrite(BUZZER, HIGH);
  } else {
    digitalWrite(BUZZER, LOW);
  }

  WiFiClient client = server.available();

  if (client) {
    String response = "<h1>Industrial Safety Monitoring</h1>";
    response += "<p>Temperature: " + String(temp) + " C</p>";
    response += "<p>Gas Value: " + String(gasValue) + "</p>";
    response += "<p>Flame Status: " + String(flameValue) + "</p>";
    response += "<p>LDR Value: " + String(ldrValue) + "</p>";

    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println();
    client.println(response);

    delay(1);
    client.stop();
  }

  delay(2000);
}
