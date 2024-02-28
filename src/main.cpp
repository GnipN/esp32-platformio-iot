#include <Arduino.h>
#include <WiFi.h>  // Replace with your network library if using a different connection method
#include <PubSubClient.h>



const char* ssid = "virus";
const char* password = "1234asdf";
const char* mqtt_server = "192.168.74.251";

void callback(char* topic, byte* payload, unsigned int length);
void reconnectWiFi() ;

WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);

long lastMsg = 0;
String lastLEDstatus = "OFF";

void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  mqtt.setServer(mqtt_server, 1883);
  mqtt.setCallback(callback);

  pinMode(GPIO_NUM_9, OUTPUT);
}

void callback(char* topic, byte* payload, unsigned int length) {
  String topicStr = String(topic); // Convert char* to String
  String payLoadStr; // Convert byte* to String
  for (int i = 0; i < length; i++) {
    payLoadStr += (char)payload[i];
  }
  Serial.print("Message arrived [");
  Serial.print(topicStr);
  Serial.print("] ");
  Serial.println(payLoadStr);

  if( topicStr == "LED" ) {
    lastLEDstatus = payLoadStr;
    if (lastLEDstatus == "1") {
      digitalWrite(GPIO_NUM_9, HIGH);
    }else if (lastLEDstatus == "0"){
      digitalWrite(GPIO_NUM_9, LOW);
    }
  }
  Serial.println();
}

void reconnect() {
  while (!mqtt.connected()) {
    if (WiFi.status() != WL_CONNECTED) {  
        reconnectWiFi();  
    }else{
    if (mqtt.connect("ESP32Client")) {
      Serial.println("Connected to MQTT broker");
      mqtt.subscribe("myTopic");
      mqtt.subscribe("LED");
    } else {
      Serial.print("Failed to connect, rc=");
      Serial.print(mqtt.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
  }
}

void loop() {
   if (WiFi.status() != WL_CONNECTED) {  
        reconnectWiFi();  
    }else{
  if (!mqtt.connected()) {
    reconnect();
  }
  mqtt.loop();

  // Publish a message every 5 seconds
  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;
    mqtt.publish("myTopic", "Hello from ESP32");
    int sensorValue = analogRead(A0); // read analog input pin 0
    mqtt.publish("webChart", String(sensorValue).c_str());
  }

  
}
}

//ถ้าไวไฟหลุด จะเรียกฟังก์ชันนี้ จนกว่าจะต่อใหม่ได้
void reconnectWiFi() {  
    Serial.print("Reconnecting");
    WiFi.mode(WIFI_STA);  
    WiFi.begin(ssid, password);  
    while (WiFi.status() != WL_CONNECTED) {  
        delay(1000);  
        Serial.print(".");
    }  
    Serial.println("Connected!");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}  




// #include <Arduino.h>
// #include <WiFi.h>

// // put your WiFi credentials here
// const char* ssid = "wifianme";
// const char* password = "password";

// void reconnectWiFi();

// void setup() {
//   // put your setup code here, to run once:
//   Serial.begin(9600);  // initialize serial communication at 9600 bits per second
//   pinMode(LED_BUILTIN, OUTPUT);

//   // connect to WiFi
//   WiFi.begin(ssid, password);
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(1000);
//     Serial.println("Connecting to WiFi...");
//     digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
//   }
//   // set LED to indicate successful connection
//   digitalWrite(LED_BUILTIN, HIGH);
//   Serial.println("Connected to WiFi");

// }

// void loop() {
//   // put your main code here, to run repeatedly:
//    if (WiFi.status() != WL_CONNECTED) {  
//       reconnectWiFi();  
//   }

//   // read the input on analog pin 0:
//   int sensorValue = analogRead(A0); // read analog input pin 0
//   Serial.print(sensorValue, DEC); // prints the value read
//   Serial.print(" \n"); // prints a space between the numbers
//   delay(1000); // wait 100ms for next reading
// }

// void reconnectWiFi() {  
//     Serial.print("Reconnecting");
//     WiFi.mode(WIFI_STA);  
//     WiFi.begin(ssid, password);  
//     while (WiFi.status() != WL_CONNECTED) {  
//         delay(1000);  
//         Serial.print(".");
//         digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
//     }
//     // set LED to indicate successful connection
//   digitalWrite(LED_BUILTIN, HIGH);  
//     Serial.println("Connected!");
// }




