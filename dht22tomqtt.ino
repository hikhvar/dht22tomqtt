/*
 * This sketch is bassed upon the following two examples. Thingsboard showed how to connect a DHT22 via an ESP8266 to an MQTT broker:
 * https://thingsboard.io/docs/samples/esp8266/temperature/
 * Losant showed how to enable the deep sleep mode:
 * https://www.losant.com/blog/making-the-esp8266-low-powered-with-deep-sleep
 */

#include "DHT.h"
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include "configvariables.h"


// DHT
#define DHTPIN 4
#define DHTTYPE DHT22

char mqttBroker[] = MQTT_BROKER;

WiFiClient wifiClient;
IPAddress ip;                    // the IP address of your ESP8266
unsigned long lastSend;

// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE);

PubSubClient client(wifiClient);

int status = WL_IDLE_STATUS;

void setup()
{
  Serial.begin(9600);
  dht.begin();
  delay(10);
  client.setServer( mqttBroker, 1883 );
  
  client.disconnect();
  lastSend = 0;
}

void loop()
{

    if ( !client.connected() ) {
      reconnect();
    }
  
    if ( millis() - lastSend > 60000 ) { // Update and send only after 60 seconds
      getAndSendTemperatureAndHumidityData();
      lastSend = millis();
    }
    client.loop();
  
}

void getAndSendTemperatureAndHumidityData()
{
  Serial.println("Collecting temperature data.");

  // Reading temperature or humidity takes about 250 milliseconds!
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  String temperature = String(t);
  String humidity = String(h);
  String heat_index = String(hic);


  // Just debug messages
  Serial.print( "Sending temperature and humidity : [" );
  Serial.print( temperature ); Serial.print( "," );
  Serial.print( humidity );
  Serial.print( "]   -> " );

  // Prepare a JSON payload string
  String payload = "{";
  payload += "\"temperature\":"; payload += temperature; payload += ",";
  payload += "\"humidity\":"; payload += humidity;  payload += ",";
  payload += "\"heat_index\":"; payload += heat_index;
  payload += "}";

  // Send payload
  char attributes[100];
  payload.toCharArray( attributes, 100 );
  client.publish( MQTT_TOPIC, attributes );
  Serial.println( attributes );

}

void InitWiFi()
{
  Serial.println("Connecting to AP ...");
  // attempt to connect to WiFi network

  WiFi.begin(WIFI_AP, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to AP");
  Serial.println("Got IP");
  ip = WiFi.localIP();
  Serial.println(ip);
}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    status = WiFi.status();
    if ( status != WL_CONNECTED) {
      InitWiFi();
    }
    Serial.print("Connecting to MQTT Broker ...");
    // Attempt to connect (clientId, username, password)
    if ( client.connect("ESP8266 Device", MQTT_TOKEN, NULL) ) {
      Serial.println( "[DONE]" );
    } else {
      Serial.print( "[FAILED] [ rc = " );
      Serial.print( client.state() );
      Serial.println( " : retrying in 5 seconds]" );
      // Wait 5 seconds before retrying
      delay( 5000 );
    }
  }
}
