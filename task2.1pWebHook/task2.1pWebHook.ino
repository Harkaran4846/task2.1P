#include "DHT.h"
#include "WiFiNINA.h"
#include "secrets.h"
#include "ThingSpeak.h"

// DHT sensor
#define DHTPIN 2     
#define DHTTYPE DHT22   // DHT 22
DHT dht(DHTPIN, DHTTYPE);

// WiFi and ThingSpeak configuration
const char* ssid = SECRET_SSID;                 // network SSID
const char* pass = SECRET_PASS;                 // network password
const unsigned long channelNumber = SECRET_CH_ID;
const char* writeAPIKey = SECRET_WRITE_APIKEY;

WiFiClient client;

void setup() {
  Serial.begin(115200);      // Initialize serial communication
  dht.begin();  // Initialize the DHT sensor
  ThingSpeak.begin(client);  // Initialize ThingSpeak
}

void loop() {
  // Check and reconnect to WiFi if not connected
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting to ");
    Serial.print(ssid);
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("\nConnected!");
  }

  // Read temperature and humidity
  float humidity = dht.readHumidity();
  float temperatureF = dht.readTemperature(true);  // Fahrenheit

  // Check for read errors
  if (isnan(humidity) || isnan(temperatureF)) {
    Serial.println("Error reading from DHT sensor!");
    return;
  }

  // Print sensor values
  Serial.print("surrounding Humidity: ");
  Serial.print(humidity);
  Serial.print("% surrounding Temperature");
  Serial.print(temperatureF);
  Serial.println("°F");

  // Send data to ThingSpeak
  ThingSpeak.setField(1, temperatureF);
  ThingSpeak.setField(2, humidity);
  
  // Write data to ThingSpeak
  int responseCode = ThingSpeak.writeFields(channelNumber, writeAPIKey);
  if (responseCode == 200) 
  {
    Serial.println("Update successful!");
  } 
  else 
  {
    Serial.print("Error updating channel. Code: ");
    Serial.println(responseCode);
  }

  delay(60000); // Wait 60 seconds before the next update
}
