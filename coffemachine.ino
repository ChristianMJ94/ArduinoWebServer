#include <SPI.h>
#include <Ethernet.h>
#include <Wire.h>

/// Skal være intgreret lys på boardet
int led = 13; 
/// Temperaturmåler på boardet
float tempC = 0.00f;
/// Ønsket temperatur fra potentiometer
float desiredTempC = 0.00f;
/// Status på kaffebrygningen 0 1 2
int brewingState = 0;
/// Status på animationen 0 1
int animationState = 0;
/// Status på kaffen kan brygges true false
bool warmingState = false;
/// De forskellige status situationer i array
String action[] = {"Waiting","Warming Up", "Brewing"};

/// Mac adresse
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED}; 
///  ip i lan (Som indtastes i browser)
byte ip[] = {192, 168, 1, 178};   
/// Internet adgang via router                 
byte gateway[] = {192, 168, 1, 1}; 
/// Subnet maske               
byte subnet[] = {255, 255, 255, 0};
/// Serverport
EthernetServer server(80);
String readString;
void setup() {
  Serial.begin(9600); 
   while (!Serial)
  {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  pinMode(led, OUTPUT);  
  /// Start Ethernet forbindelse til server
  Ethernet.begin(mac, ip, gateway, subnet);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());  

  /// Metode hent setup fra Temperature
  tempSetup();    
}
//void loop()
//{
//  int sensorValue =  map(analogRead(A1), 0, 1023, 19,25);
//  Serial.println(sensorValue);
//  delay(100);
//}
void loop()
{
  
  /// Metode hent temperatur beregning fra Temperature 
  tempC = tempLoop();
   /// Input fra Pontentiometer
  float sensorValue = analogRead(A1); 
  /// Hent ønsket temperatur fra potentiometer
  desiredTempC = map(sensorValue, 0, 1023, 19,25);
  
  /// Lav client forbindelse
  EthernetClient client = server.available();
  if (client)
  {
    while (client.connected())
    {
      if (client.available())
      {
        /// Læs char for char ind fra HTTP request
        char c = client.read();

        /// Sæt en begrænsning
        if (readString.length() < 100)
        {
          /// Gem alle karakterne i en string
          readString += c;
        }

        /// Når HTTP request er slut
        if (c == '\n')
        {
          
          Serial.println(readString);
          client.println(F("HTTP/1.1 200 OK"));
          client.println(F("Content-Type: text/html"));
          client.println("Refresh: 5");
          client.println(F(""));
          client.println(F("<HTML>"));
          client.println(F("<HEAD>"));
          client.println(F("<meta name='apple-mobile-web-app-capable' content='yes' />"));
          client.println(F("<meta name='apple-mobile-web-app-status-bar-style' content='black-translucent' />"));
          client.println(F("<link rel='stylesheet' href='https://cdn.jsdelivr.net/npm/purecss@3.0.0/build/pure-min.css' integrity='sha384-X38yfunGUhNzHpBaEBsWLO+A0HDYOQi8ufWDkZ0k9e0eXz/tH3II7uKZ9msv++Ls' crossorigin='anonymous'>"));
          client.println(F("<script src='https://unpkg.com/@lottiefiles/lottie-player@latest/dist/lottie-player.js'></script>"));
          client.println(F("<TITLE>The Coffee Machine Project</TITLE>"));
          client.println(F("</HEAD>"));
          client.println(F("<BODY>"));
          client.println(F("<div style='text-align:center'>"));
          client.println(F("<H1>The Coffee Machine Project</H1>"));
          client.println(F("<hr />"));
          client.println(F("<br />"));
          client.println(F("<H2>Arduino Mega 2560 with Ethernet Shield to web</H2>"));
          client.print(F("<p>The perfect water temperature is: "));
          client.print(desiredTempC);
          client.println(F("</p>"));
          client.println(F("<br />"));
          client.print(F("Status: "));
          client.print(action[brewingState]);
          client.println(String(tempC) = brewingState > 0 ? " " + (String)tempC : "");
          client.println(F("<br />"));
          client.print(F("<lottie-player src='https://assets1.lottiefiles.com/packages/lf20_hjfxddwh.json'  background='transparent'  speed='"));
          client.print(animationState);
          client.println(F("'style='height: 300px;'  loop  autoplay></lottie-player>"));
          client.println(F("<a class='pure-button' href=\"/?button1on\"\">Brew Some Coffee</a>"));
          client.println(F("<a class='pure-button' href=\"/?button1off\"\">Please, Turn it off</a><br />"));
          client.println(F("<br />"));
          client.println(F("</div>"));
          client.println(F("</BODY>"));
          client.println(F("</HTML>"));

          delay(1);
          /// Stop clienten
          client.stop();
          
          /// Arduino kontrol fra læsning af http request
          if (readString.indexOf("?button1on") > 0)
          {
            /// Tænder lyset på boardet
            digitalWrite(LED_BUILTIN, HIGH);
            warmingState = true;
            tempC = tempLoop();
            animationState = tempC > desiredTempC ? 1 : 0; 
            brewingState = 1;
            if(animationState == 1){
              brewingState = 2;
            }
          }
          if (readString.indexOf("?button1off") > 0)
          {
            /// Slukker lyset på boardet
            digitalWrite(LED_BUILTIN, LOW);
            warmingState = false;
            animationState = 0;
            brewingState = 0;
          }
          // clearing string for next read
          readString = "";
        }
      }
    }
  }
}
