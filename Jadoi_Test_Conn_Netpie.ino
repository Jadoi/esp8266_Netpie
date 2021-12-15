#include <ESP8266WiFi.h> //เป็นการเรียกใช้ Library โมดูล ESP8266 * บอร์ท esp8266 เลือกเวอร์ชั่นต่ำๆ 2.7.4
#include <WiFiManager.h>
#include <PubSubClient.h>

const char* mqtt_server = "broker.netpie.io";
const int mqtt_port = 1883;
const char* mqtt_Client = "29d9cd81-0ac5-4bd0-8b62-bb9ff277db80";
const char* mqtt_username = "rbvigf5yD5uL6uEN1M9VT4xJWG84tmnA";
const char* mqtt_password = "foAPYbA5kaYLB*Qvf!fN!g!ACCb6Gj-5";
WiFiClient espClient;
PubSubClient client(espClient);
char msg[50];
int Relay1 = 0,Relay2 = 0;
#define R1 16  // กำหนด D0 คือขา 16
#define R2 5  // กำหนด  D1 คือขา 5 
#define R01 R1 
#define R02 R2 

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection…");
    if (client.connect(mqtt_Client, mqtt_username, mqtt_password)) {
      Serial.println("connected");
      client.subscribe("@msg/#");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println("try again in 5 seconds");
      delay(5000);
    }
  }
}
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String message;
  String tpc;
  for (int i = 0; i < length; i++) {
    message = message + (char)payload[i];
  }
   Serial.println(message);
   tpc = getMsg(topic,message);
   tpc.toCharArray(msg, (tpc.length() + 1));
   client.publish("@shadow/data/update", msg);
}
void setup() {
    pinMode(R01,OUTPUT); digitalWrite(R01, HIGH);
    pinMode(R02,OUTPUT); digitalWrite(R02, HIGH);
    Serial.begin(115200);
    onAutoConnWifi(""); //เชื่อมต่อAuto และ ตั้งค่าการเชื่อมต่อเมื่อเจอเครือข่ายใหม่ โดย IP Access point 192.168.4.1 
    WiFi.mode(WIFI_STA);
    while (WiFi.waitForConnectResult() != WL_CONNECTED){
      Serial.println("Connection Failed! Rebooting...");
      delay(5000);
      ESP.restart();
    }
    Serial.println(""); 
    Serial.println("Wi-Fi connected"); //แสดงว่าเชื่อมต่อ Wi-Fi ได้แล้ว
    Serial.print("IP Address : ");
    Serial.println(WiFi.localIP()); //แสดง IP ของบอร์ดที่ได้รับแจกจาก AP
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(callback);

}

void loop() {
    //------Check connection------
    if (!client.connected()) {
      reconnect();
    }
    client.loop();
    delay(1000);
}
void onAutoConnWifi(String chk){
   WiFiManager wifiManager;
   if(chk=="reset"){
    wifiManager.resetSettings();
    Serial.println("reset wifi");
   }else{
    wifiManager.autoConnect("LED Test"); // ชื่อ Access point เริ่มต้น
    Serial.println(WiFi.localIP());
   }
}
 
 String getMsg(String topic_,String message_){       //datasources["???"]["shadow"]["Relay1"]     Indicator Light : datasources["???"]["shadow"]["Relay1"]=='1'
  String tpc;
  if(topic_ == "@msg/Relay1") {
        if (message_ == "R01_ON"){                   //netpie["???"].publish("@msg/Relay1","R01_OFF")
          digitalWrite(R01, HIGH);
          tpc = "{\"data\": {\"Relay1\":1}}";        //datasources["???"]["shadow"]["Relay1"]=='1'
          Serial.println("R01_ON");
        } else if (message_ == "R01_OFF") {          //netpie["???"].publish("@msg/Relay1","R01_OFF")
          digitalWrite(R01, LOW);
           tpc = "{\"data\": {\"Relay1\":0}}";       //datasources["???"]["shadow"]["Relay1"]=='0'
          Serial.println("R01_OFF");
        }
      }
     return tpc;
 }
