#include "WiFiEsp.h"
#include "WiFiEspClient.h"
#include "SoftwareSerial.h"
#include "PubSubClient.h"

// Emulate Serial interface on pins 6/7 if not present
SoftwareSerial Serial1(6, 7); // RX, TX

// Wifi SSID config
char ssid[] = "ssid name";            // your network SSID (name)
char pass[] = "password";        // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status

//MQTT broker IP (my machine's IP)
IPAddress server(192,168,0,100); // ESP8266 192.168.0.103

//Initialize the client object
WiFiEspClient espClient;
PubSubClient client(espClient);

//sample logic
int temp = 23;
char scale = 'A';
char tempS[10];

void setup()
{
  // initialize serial for debugging
  Serial.begin(115200);
  // initialize serial for ESP module
  Serial1.begin(9600);
  // initialize ESP module
  WiFi.init(&Serial1);

  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }

  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
  }

  //You're connected now
  Serial.println("You're connected to the network");

  //connect to MQTT server
  client.setServer(server, 1883);
  client.setCallback(callback);
}

//print any message received for subscribed topic
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  scale = (char)payload[0];
}

void loop()
{
  // put your main code here, to run repeatedly:
  if (!client.connected()) {
    reconnect();
  }

  client.loop();
  
  client.subscribe("inTopic");

  if (scale == 'A') {
    temp = 1;  
  }
  else if (scale == 'B') {
    temp = 10;  
  }
  else if (scale == 'C') {
    temp = 100;  
  }
  sprintf (tempS, "%03i", temp);
  client.publish("outTopic",tempS);
  
  
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect, just a name to identify the client
    if (client.connect("arduinoClient")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic","hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

