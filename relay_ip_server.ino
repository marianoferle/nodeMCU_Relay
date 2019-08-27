//#include <SoftwareSerial.h>
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>



const char *ssid =  "nn";     // replace with your wifi ssid and wpa2 key
const char *pass =  "nn";

WiFiClient client;

int relayInput = 2;
WiFiServer server(80);

IPAddress gateway;
float hora = 0;

const long utcOffsetInSeconds = 3600;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

WiFiUDP ntpUDP;
//NTPClient timeClient(ntpUDP);
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

String formattedDate;
String dayStamp;
String timeStamp;
String formattedTime;

void setup(){
  
       Serial.begin(9600);
       delay(10);
               
       Serial.println("Connecting to ");
       Serial.println(ssid); 
 
       WiFi.begin(ssid, pass); 
       while(WiFi.status() != WL_CONNECTED){
            delay(500);
            Serial.print(".");
       }
        
        Serial.println("");
        Serial.println("WiFi connected"); 
         /*-------- server started---------*/ 
        server.begin();
        Serial.println("Server started");                   
        /*------printing ip address--------*/
        Serial.print("IP Address of network: ");
        Serial.println(WiFi.localIP());
        Serial.println("/");  

         gateway = WiFi.gatewayIP();
         Serial.print("GATEWAY: ");
         Serial.println(gateway);

        pinMode(relayInput, OUTPUT);

        timeClient.begin();
        timeClient.setTimeOffset(3600);


       
}

void times_(){
  Serial.print(daysOfTheWeek[timeClient.getDay()]);
  Serial.print(", ");
  Serial.print(timeClient.getHours());
  Serial.print(":");
  Serial.print(timeClient.getMinutes());
  Serial.print(":");
  Serial.println(timeClient.getSeconds());
  //Serial.println(timeClient.getFormattedTime())
}
 
void loop(){  
  
  WiFiClient client = server.available();  
  
  if(!client){ return; }
  
  Serial.println("Waiting for new client");
  
  while(!client.available()){  delay(1); }
  
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

  
  //----------------------------------------------------------------
  
  
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }

 
  times_();
  delay(1000);
  

 
 
  //---------------------------
  int value = LOW;
  if(request.indexOf("/RELAY=ON") != -1){
    digitalWrite(relayInput, HIGH);
    value = HIGH;
  }
  if(request.indexOf("/RELAY=OFF") != -1){
    digitalWrite(relayInput, LOW); 
    value = LOW;
  }
  //------------------------

  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); 
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
 
  client.print("RELAY is: "); 
  
  if(value == HIGH){
    client.print("ON");
  }else{ 
    client.print("OFF"); 
  }
  
  String info_= "<br><br>"
      "<a href=\"/RELAY=ON\"\"><button>ON</button></a>"
      "<a href=\"/RELAY=OFF\"\"><button>OFF</button></a>"
      "<br />"
      "<script>var t=new Date(); console.log(t);</script>"
      "</html>";
  client.println(info_);

 /* client.println("<br><br>");
  client.println("<a href=\"/RELAY=ON\"\"><button>ON</button></a>");
  client.println("<a href=\"/RELAY=OFF\"\"><button>OFF</button></a><br />"); 
  client.println(""); 
  client.println("</html>");*/
 
  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");
 
  
}
