#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h> // MQTT library

// Note
// 1. ต้องเปลี่ยน IP address ของ MQTT broker ให้ตรงกับ IP address ของ MQTT broker ที่เราใช้งาน
// 2. ต้องเปลี่ยนชื่อของ WiFi และ password ให้ตรงกับ WiFi ที่เราใช้งาน
// 3. ต้องเปลี่ยนชื่อของ topic ให้ตรงกับ topic ที่เราใช้งาน
// 4. ต้องต่อสายไฟจาก GPIO 5 ไปที่ LED หรืออุปกรณ์ที่ต้องการควบคุม หรือเปลี่ยน GPIO ที่ต้องการควบคุม (หากไม่ต่อวงจรอาจทำให้ ESP32 รีเซ็ตเองเป็นระยะๆ)

// put your WiFi credentials here
const char* ssid = "wifiname";
const char* password = "password";
const char* mqtt_server = "xxx.xxx.xxx.xxx"; // IP address of the MQTT broker

void callback(char* topic, byte* payload, unsigned int length);
void reconnectWiFi();
void reconnectMqtt();


WiFiClient wifiClient; // create a WiFi client
PubSubClient mqtt(wifiClient); // create a MQTT client

long lastMsg = 0;
long lastSendSensorValue = 0;
String lastLEDstatus = "OFF";
String newLEDstatus = "OFF";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);  // initialize serial communication at 9600 bits per second
  pinMode(LED_BUILTIN, OUTPUT);

  // connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  }
  // set LED to indicate successful connection
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println("Connected to WiFi");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  mqtt.setServer(mqtt_server, 1883);
  mqtt.setCallback(callback);

  pinMode(GPIO_NUM_9, OUTPUT);
  // pinMode(5, OUTPUT);

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

  if( topicStr == "home/livingroom/LED1" ) {
    newLEDstatus = payLoadStr;
    if( newLEDstatus != lastLEDstatus ) {
      if (newLEDstatus == "status=on") {
        digitalWrite(GPIO_NUM_9, HIGH);
        // digitalWrite(5, HIGH);
      } else if (newLEDstatus == "status=off") {
        digitalWrite(GPIO_NUM_9, LOW);
        // digitalWrite(5, LOW);
      }
      lastLEDstatus = newLEDstatus;
    }
  }
  Serial.println();
}

void reconnectMqtt()
{
  while (!mqtt.connected())
  {
    if (WiFi.status() != WL_CONNECTED)
    {
      reconnectWiFi();
    }
    else
    {
      if (mqtt.connect("ESP32Client"))
      {
        Serial.println("Connected to MQTT broker");
        // subscribe to the topic "home/livingroom/LED1"
        mqtt.subscribe("home/livingroom/LED1");
      }
      else
      {
        Serial.print("Failed to connect, rc=");
        Serial.print(mqtt.state());
        Serial.println(" try again in 5 seconds");
        delay(5000);
      }
    }
  }
}

void loop() {
  // put your main code here, to run repeatedly:
   if (WiFi.status() != WL_CONNECTED) {  
      reconnectWiFi();  
  }
  // check if MQTT client is connected
  if (!mqtt.connected()) {
    reconnectMqtt();
  }
  mqtt.loop();


  // read the input on analog pin 0:
  int sensorValue = analogRead(A0); // read analog input pin 0
  Serial.print(sensorValue, DEC); // prints the value read
  Serial.print(" \n"); // prints a space between the numbers
  delay(1000); // wait 100ms for next reading

  long now = millis();
  if ((now - lastMsg > 5000) && (sensorValue < 2000)) {
    lastMsg = now;
    mqtt.publish("home/livingroom/LED1", "status=on");
  }else if((now - lastMsg > 5000) && (sensorValue > 2000)){
    lastMsg = now;
    mqtt.publish("home/livingroom/LED1", "status=off");
  }

  // send sensor value to MQTT broker every 5 seconds
  if (now - lastSendSensorValue > 5000) {
    lastSendSensorValue = now;
    String sensorValueStr = "{\"LDR\":" + String(sensorValue)+ "}"; // --> {"LDR":1205}
    String sensorValueStr2 = "{\"LDR\":" + String(sensorValue)+ ",\"temp\":" + String(sensorValue+200) + "}"; // --> {"LDR":1205,"temp":1405}
    mqtt.publish("sensors_values", String(sensorValueStr2).c_str());
  }
}

void reconnectWiFi() {  
    Serial.print("Reconnecting");
    WiFi.mode(WIFI_STA);  
    WiFi.begin(ssid, password);  
    while (WiFi.status() != WL_CONNECTED) {  
        delay(1000);  
        Serial.print(".");
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    }
    // set LED to indicate successful connection
  digitalWrite(LED_BUILTIN, HIGH);  
    Serial.println("Connected!");
}

// #include <Arduino.h>
// #include <WiFi.h>  // Replace with your network library if using a different connection method
// #include <PubSubClient.h>


// const char* ssid = "wifianme";
// const char* password = "password";
// const char* mqtt_server = "192.168.243.251";

// void callback(char* topic, byte* payload, unsigned int length);
// void reconnectWiFi() ;

// WiFiClient wifiClient;
// PubSubClient mqtt(wifiClient);

// long lastMsg = 0;

// void setup() {
//   Serial.begin(9600);
//   WiFi.mode(WIFI_STA);
//   WiFi.begin(ssid, password);

//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }

//   Serial.println("WiFi connected");
//   Serial.println("IP address: ");
//   Serial.println(WiFi.localIP());

//   mqtt.setServer(mqtt_server, 1883);
//   mqtt.setCallback(callback);
// }

// void callback(char* topic, byte* payload, unsigned int length) {
//   Serial.print("Message arrived [");
//   Serial.print(topic);
//   Serial.print("] ");
//   for (int i = 0; i < length; i++) {
//     Serial.print((char)payload[i]);
//   }
//   Serial.println();
// }

// void reconnect() {
//   while (!mqtt.connected()) {
//     if (WiFi.status() != WL_CONNECTED) {  
//         reconnectWiFi();  
//     }else{
//     if (mqtt.connect("ESP32Client")) {
//       Serial.println("Connected to MQTT broker");
//       mqtt.subscribe("myTopic");
//       mqtt.subscribe("myTopic2");
//     } else {
//       Serial.print("Failed to connect, rc=");
//       Serial.print(mqtt.state());
//       Serial.println(" try again in 5 seconds");
//       delay(5000);
//     }
//   }
//   }
// }

// void loop() {
//    if (WiFi.status() != WL_CONNECTED) {  
//         reconnectWiFi();  
//     }else{
//   if (!mqtt.connected()) {
//     reconnect();
//   }
//   mqtt.loop();

//   // Publish a message every 5 seconds
//   long now = millis();
//   if (now - lastMsg > 5000) {
//     lastMsg = now;
//     mqtt.publish("myTopic", "Hello from ESP32");
//   }
// }
// }

// //ถ้าไวไฟหลุด จะเรียกฟังก์ชันนี้ จนกว่าจะต่อใหม่ได้
// void reconnectWiFi() {  
//     Serial.print("Reconnecting");
//     WiFi.mode(WIFI_STA);  
//     WiFi.begin(ssid, password);  
//     while (WiFi.status() != WL_CONNECTED) {  
//         delay(1000);  
//         Serial.print(".");
//     }  
//     Serial.println("Connected!");
//     Serial.println("IP address: ");
//     Serial.println(WiFi.localIP());
// }  
