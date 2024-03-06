#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h> // for connect to web service
#include <ArduinoJson.h> // for parse JSON

// put your WiFi credentials here
const char* ssid = "virus";
const char* password = "1234asdf";
String token;
String web_service_protocal_ip_port = "http://192.168.74.251:3000";

void reconnectWiFi();
void sendSensorValue(String sens_id, int sens_val);

long lastMsg = 0;

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

  if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
    HTTPClient http;  //Declare an object of class HTTPClient
    
    Serial.println("connect to web service");
    http.begin(web_service_protocal_ip_port + "/login");  //Specify request destination
    http.addHeader("Content-Type", "application/json");  //Specify content-type header

    // Prepare your JSON payload
    String payload = "{\"usr\":\"usr\", \"pass\":\"pass\"}";

    int httpCode = http.POST(payload);   //Send the request
    String response = http.getString();                  //Get the response payload

    Serial.println("web service return");
    Serial.println(httpCode);   //Print HTTP return code
    Serial.println(response);    //Print request response payload

    DynamicJsonDocument doc(1024);
    deserializeJson(doc, response);
    token = doc["token"].as<String>();

    Serial.println(token); 

    // request with token
    http.begin(web_service_protocal_ip_port + "/protected");  //Specify request destination
    http.addHeader("Authorization", "Bearer " + token);  //Add Authorization header

    httpCode = http.GET();   //Send the request
    response = http.getString();                  //Get the response payload

    Serial.println(httpCode);   //Print HTTP return code
    Serial.println(response);    //Print request response payload


    http.end();  //Close connection
  } else {
    Serial.println("Error in WiFi connection");
  }

}



void loop() {
  // put your main code here, to run repeatedly:
   if (WiFi.status() != WL_CONNECTED) {  
      reconnectWiFi();  
  }

long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;
  // read the input on analog pin 0:
  int sensorValue = analogRead(A0); // read analog input pin 0
  sendSensorValue("LDR01", sensorValue);
  // Serial.print(sensorValue, DEC); // prints the value read
  // Serial.print(" \n"); // prints a space between the numbers
  // delay(1000); // wait 100ms for next reading
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

void sendSensorValue(String sens_id, int sens_val) {
  if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
    HTTPClient http;  //Declare an object of class HTTPClient

    http.begin(web_service_protocal_ip_port + "/sensVal");  //Specify request destination
    http.addHeader("Content-Type", "application/json");  //Specify content-type header

    // Prepare your JSON payload
    DynamicJsonDocument doc(1024);
    doc["token"] = token;
    doc["sens_id"] = sens_id;
    doc["sens_val"] = sens_val;
    String payload;
    serializeJson(doc, payload);

    int httpCode = http.POST(payload);   //Send the request
    String response = http.getString();  //Get the response payload

    Serial.println(httpCode);   //Print HTTP return code
    Serial.println(response);   //Print request response payload

    http.end();  //Close connection
  } else {
    Serial.println("Error in WiFi connection");
  }
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
