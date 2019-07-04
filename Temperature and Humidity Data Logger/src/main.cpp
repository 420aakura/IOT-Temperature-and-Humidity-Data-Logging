//Update 26th of Feb 2019
//DHT11 to Thingspeak Data logging
//Node Mcu v0.9 Lolin

//#include <Arduino.h>  //uncomment only in vscode
#include <DHTesp.h>
#include <ESP8266WiFi.h>

DHTesp dht;  //dhtesp object

// replace with your channel’s thingspeak API key,
String apiKey = "XXXXXXXXXXXXXXX";  //API write key of Thingspeak

const char* ssid = "SSID";  //enter ssid of wifi
const char* password = "Password";  //enter password of wifi

const char* server = "api.thingspeak.com";  // "184.106.153.149" or api.thingspeak.com
const int DHTPIN = 14; // SO CONNECT THE DHT11/22 SENSOR TO PIN D5 OF THE NODEMCU
//DHT dht(DHTPIN, DHT11,15); //CHANGE DHT11 TO DHT22 IF YOU ARE USING DHT22

//deepsleep mode
const int sleepTimeS = 1800; //second 30min (60*30 minute) connect reset with 16 number pin for wakeup call

const int NwLed = 15; //D8 PIN
int flag = 0;
WiFiClient client;

void setup() {
    pinMode(NwLed, OUTPUT);
    dht.setup(DHTPIN, DHTesp::DHT11);
    Serial.begin(115200);
    delay(10);

    WiFi.begin(ssid, password);

    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        flag = 1;
        //ESP.reset();  //if connection timeout is faced
    }
    delay(2000);
    Serial.println("");
    Serial.println("WiFi connected");
    flag = 0;

    if(flag == 0)
      digitalWrite(NwLed, HIGH);

}

void loop() {

    delay(dht.getMinimumSamplingPeriod());
    float h = dht.getHumidity();
    float t = dht.getTemperature();
    if (isnan(h) || isnan(t)) {
        Serial.println("Failed to read from DHT sensor!");
        return;
    }

    if (client.connect(server,80)) { 
        String postStr = apiKey;
        postStr +="&field1=";
        postStr += String(t);
        postStr +="&field2=";
        postStr += String(h);
        postStr += "\r\n\r\n";

        client.print("POST /update HTTP/1.1\n");
        client.print("Host: api.thingspeak.com\n");
        client.print("Connection: close\n");
        client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
        client.print("Content-Type: application/x-www-form-urlencoded\n");
        client.print("Content-Length: ");
        client.print(postStr.length());
        client.print("\n\n");
        client.print(postStr);

        Serial.print("Temperature: ");
        Serial.print(t);
        Serial.print(" degrees Celsius Humidity: ");
        Serial.print(h);
        Serial.println("% send to Thingspeak");
        
        delay(20000);
        Serial.println("जादै छु deepsleep मा ");
        ESP.deepSleep(sleepTimeS * 1000000); //deepsleep definition
    }

    client.stop(); //end the client's misery

    Serial.println("Client Stopped.");
    // thingspeak needs minimum 15 sec delay between updates
    delay(20000);
}
