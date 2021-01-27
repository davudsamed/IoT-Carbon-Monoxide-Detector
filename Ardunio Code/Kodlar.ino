#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SimpleTimer.h>
#include <MQ7.h>         //Sensörden ölçülen veriyi ppm değerine çevirmek için kullanıdğım kütüphane
#include <ThingSpeak.h>
#define BLYNK_PRINT Serial
WiFiClient client;

String apiKey = "Z8HWLXZ3L7OR9FCQ";
char auth[] = "_d5tnr7iNoas1QgzEC8mu3i9f3Baygmy";
char ssid[] = "";
char pass[] = ""; 
SimpleTimer timer;
unsigned long channelID =1257048;
unsigned  int field_no=1;
const char* writeAPIKey = "Z8HWLXZ3L7OR9FCQ";
int buzzerPin = D2;
int ledPinBlue= 13;
int ledPinRed=D0;
const char* host = "maker.ifttt.com";

void setup() 
{
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(1000L, getSendData); // new data will be updated every 1 sec
  ThingSpeak.begin(client);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  pinMode(buzzerPin,OUTPUT);
  pinMode(ledPinBlue,OUTPUT);
  pinMode(ledPinRed,OUTPUT);
}
 
void loop() 
{
  MQ7 mq7(A0,3.3);
  float ppm= mq7.getPPM();
  timer.run();
  Blynk.run();
  ThingSpeak.writeField (channelID, field_no, ppm, writeAPIKey);


 const int httpPort = 80;  
  if (!client.connect(host, httpPort))
  {  
        Serial.println("connection failed");  
  return;
  } 
  
  if(ppm>20)
  {
    String url = "/trigger/high_ppm_warning/with/key/RUquS3EirL_ZPZAhhGCcF";   
    Serial.print("Requesting URL: ");
    Serial.println(url);
    client.print(String("GET ") + url + " HTTP/1.1\r\n" + 
                  "Host: " + host + "\r\n" +   
                         "Connection: close\r\n\r\n");// IFTT yeni 
    digitalWrite(ledPinRed,HIGH);
    digitalWrite(ledPinBlue,LOW);
    digitalWrite(buzzerPin, HIGH);
  }
  else{
    digitalWrite(ledPinBlue,HIGH);
    digitalWrite(ledPinRed, LOW);
    digitalWrite(buzzerPin, LOW);
  }
}

void getSendData()
{
  
MQ7 mq7(A0,3.3);

float ppm= mq7.getPPM();

  Blynk.virtualWrite(V2, ppm); //
 
  if (ppm > 20)
  {
    Blynk.notify("Yüksek Karbonmonoksit Değeri Ölçüldü !!!");
    Blynk.email("bregath@hotmail.com","Karbonmonoksit Uyarısı !!!",String("Yüksek Karbonmonoksit Oranı Algılandı")+ppm + String("ppm"));
  }
}
