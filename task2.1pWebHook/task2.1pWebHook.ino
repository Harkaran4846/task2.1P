#include "DHT.h"             // Include DHT sensor library
#include "WiFiNINA.h"        // Include WiFi library for Arduino WiFi Shield
#include "secrets.h"         // Include file with WiFi and ThingSpeak credentials
#include "ThingSpeak.h"      // Include ThingSpeak library for IoT data transmission

// DHT sensor configuration
#define DHTPIN 2             // Define the pin where the DHT sensor is connected
#define DHTTYPE DHT22        // Define the type of DHT sensor (DHT22 model)
DHT dht(DHTPIN, DHTTYPE);    // Initialize the DHT sensor

// WiFi and ThingSpeak configuration
const char* ssid = SECRET_SSID;             // Network SSID from secrets.h file
const char* pass = SECRET_PASS;             // Network password from secrets.h file
const unsigned long channelNumber = SECRET_CH_ID; // ThingSpeak channel ID
const char* writeAPIKey = SECRET_WRITE_APIKEY;    // ThingSpeak write API key

WiFiClient client;        // Create a client object for WiFi connection

void setup() {
  Serial.begin(115200);      // Start serial communication at 115200 baud rate
  dht.begin();               // Initialize the DHT sensor
  ThingSpeak.begin(client);  // Initialize ThingSpeak with WiFi client
}

void loop() {
  // Check WiFi connection, reconnect if disconnected
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting to ");
    Serial.print(ssid);     // Display the SSID being connected to
    WiFi.begin(ssid, pass); // Attempt to connect to WiFi
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);           // Wait for connection
      Serial.print(".");    // Indicate ongoing connection attempt
    }
    Serial.println("\nConnected!"); // Confirm successful connection
  }

  // Read data from DHT sensor
  float humidity = dht.readHumidity();                // Read humidity
  float temperatureF = dht.readTemperature(true);     // Read temperature in Fahrenheit

  // Check for read errors
  if (isnan(humidity) || isnan(temperatureF)) {
    Serial.println("Error reading from DHT sensor!"); // Print error message if sensor fails
    return;                                          // Exit loop if reading fails
  }

  // Print the sensor values to the serial monitor
  Serial.print("surrounding Humidity: ");
  Serial.print(humidity);
  Serial.print("% surrounding Temperature: ");
  Serial.print(temperatureF);
  Serial.println("°F");

  // Send data to ThingSpeak
  ThingSpeak.setField(1, temperatureF);    // Set temperature as Field 1
  ThingSpeak.setField(2, humidity);        // Set humidity as Field 2
  
  // Write data to ThingSpeak and check for success
  int responseCode = ThingSpeak.writeFields(channelNumber, writeAPIKey);
  if (responseCode == 200) {
    Serial.println("Update successful!");  // Print success message
  } else {
    Serial.print("Error updating channel. Code: ");
    Serial.println(responseCode);          // Print error code if update fails
  }

  delay(60000); // Wait 60 seconds before the next update
}
