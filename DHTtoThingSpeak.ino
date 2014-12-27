/*

Sketch to submit readings from an Arduino Ethernet to ThingSpeak
Billy Abbott - billy@cowfish.org.uk
http://cowfish.org.uk

*/

#include <SPI.h>
#include <Ethernet.h>
#include <dht.h>

// MAC address for the Arduino
byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

void setup() {
  Serial.begin(9600);

  // Start ethernet
  Serial.println("Getting IP");
  Ethernet.begin(mac);
  Serial.println(Ethernet.localIP());
}

// DHT sensor setup
#define dht_dpin 8
dht DHT;

// DHT data
int temperature=0;
int humidity=0;

// ThingSpeak data
char server[]  = "api.thingspeak.com";      // IP Address for the ThingSpeak API
String apiKey = "ENTER YOUR API KEY HERE";    // Write API Key for a ThingSpeak Channel

// Check variables
long lastCheck=0;
const int updateInterval = 10000;           // Time interval in milliseconds to update ThingSpeak   

// Our ethernet client
EthernetClient client;

void loop() {
  long now=millis();
  if (now-lastCheck>updateInterval) {

    // read data from the DHT
    DHT.read11(dht_dpin);
    temperature=DHT.temperature;
    humidity=DHT.humidity;
    lastCheck=now;

    // build data strings
    String tempData="field1=";
    String humData="field2=";
    tempData+=temperature;
    humData+=humidity;
    Serial.println(tempData);
    Serial.println(humData);
    
    //connect to ThingSpeak and write the data
    if (client.connect(server,80)) {
      Serial.print("Connected: ");
      Serial.println(server);
      
      client.println("POST /update HTTP/1.1");
      client.println("Host: api.thingspeak.com");
      client.println("Connection: close");
      client.println("X-THINGSPEAKAPIKEY: "+apiKey);
      client.println("Content-Type: application/x-www-form-urlencoded");
      client.print("Content-Length: ");
      client.println(tempData.length()+humData.length()+1);
      client.println("");
      
      client.print(tempData);
      client.print("&"); 
      client.print(humData); 

      client.stop();
    } else {
      Serial.println("Failed to connect"); 
    }
  }
}

