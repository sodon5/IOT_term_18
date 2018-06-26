#include "WiFiEsp.h"
#include <dht11.h>

/* LED 제어 핀 */
int redPin= 7;
int greenPin = 6;
int bluePin = 5;
int temp36=0;

/* 온도 제어 핀 */
dht11 DHT11;
int pin_DHT11 = 49;


// Emulate Serial1 on pins 6/7 if not present
#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(6, 7); // RX, TX
#endif

char ssid[] = "AndroidHotspot7597";            // your network SSID (name)
char pass[] = "candy56bjkp";        // your network password
int status = WL_IDLE_STATUS;

int music_onoff = LOW;
int response_type = 1;

WiFiEspServer server(80);

// use a ring buffer to increase speed and reduce memory allocation
RingBuffer buf(25);

void setup()
{
  
  Serial.begin(115200);   // initialize serial for debugging
  Serial1.begin(9600);    // initialize serial for ESP module
  WiFi.init(&Serial1);    // initialize ESP module

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  /* 온도 변화 */
  
    
  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }

  // attempt to connect to WiFi network
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
  }

  Serial.println("You're connected to the network");
  printWifiStatus();
  
  // start the web server on port 80
  server.begin();
}


void loop()
{
  WiFiEspClient client = server.available();  // listen for incoming clients
  int chk = DHT11.read(pin_DHT11);

  switch (chk)
  {
    case DHTLIB_OK:
      Serial.print("Temperature : ");
      Serial.print(DHT11.temperature);
      Serial.print("[C] Humidity : ");
      Serial.print(DHT11.humidity);
      Serial.println("[%]");
      temp36 = DHT11.humidity;
      changeColor(temp36);
      break;
    case DHTLIB_ERROR_CHECKSUM: 
        break;
    case DHTLIB_ERROR_TIMEOUT: 
        break;
    default: 
        break;
    }

  if (client) {                               // if you get a client,
    Serial.println("New client");             // print a message out the serial port
    buf.init();                               // initialize the circular buffer
    while (client.connected()) {              // loop while the client's connected
      if (client.available()) {               // if there's bytes to read from the client,
        char c = client.read();               // read a byte, then
        buf.push(c);                          // push it to the ring buffer

        // printing the stream to the serial monitor will slow down
        // the receiving of data from the ESP filling the serial buffer
        //Serial.write(c);
      }
    }
    response_type = 1;
    switch(response_type){
        case 1: show_main_page(client); break;
    }
    
    // close the connection
    client.stop();
    Serial.println("Client disconnected");
  }
}


void show_main_page(WiFiEspClient client)
{
  
  // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
  // and a content-type so the client knows what's coming, then a blank line:
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println();
  
  // the content of the HTTP response follows the header:
  client.println("<html lang='ko'>");
  client.println("<head><title>IoT18</title><meta charset='UTF-8'></head>");
  client.println("<body>");
  client.println("<div style='width: 300px; text-align:center; border:1px solid #555; margin:auto; padding:30px 30px'>");

  client.println("<h2>Temperature is ");
  client.println(DHT11.temperature+"</h2>");
  client.println("<h2>Humidity is ");
  client.println(DHT11.humidity+"</h2>");

  if(DHT11.humidity>10 && DHT11.humidity<=20) {
    client.println("<a href='https://www.youtube.com/playlist?list=PL5barlq8kvpZucAQfAhAyHdxnbhmOZlaK'>따뜻해지는 노래</a></div>");
  }
  else if(DHT11.humidity>20 && DHT11.humidity<=30) {
    client.println("<a href='https://www.youtube.com/playlist?list=PL5barlq8kvpZXy2Q62fRklikfsYwZ3tk5'>편안해지는 노래</a></div>");
  }
  else {
    client.println("<a href='https://www.youtube.com/playlist?list=PL5barlq8kvpZxo5mFdJ9a92CwZdFiMH6V'>시원해지는 노래</a></div>");
  }
  client.println("<div style='text-align:center; margin-top:60px'>");
//  client.print("Currently... music player is ");
//  client.println(music_onoff ? "On.":"Off.");
//  
//  client.println("<br />");
//  client.println("<FORM method=\"get\" action=\"/music.html\">");
//  client.println("<P><INPUT type=\"radio\" name=\"onoff\" value=\"1\">Turn On");
//  client.println("<P><INPUT type=\"radio\" name=\"onoff\" value=\"0\">Turn off");
//  client.println("<P><INPUT type=\"submit\" value=\"Submit\"></FORM></div>");
  
  client.println("</body>");
  client.println("</html>");
  // The HTTP response ends with another blank line:
  client.println();
}

void printWifiStatus()
{
  // print the SSID of the network you're attached to
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print where to go in the browser
  Serial.println();
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
  Serial.println();
}

void changeColor(int temp36) {
  int redValue=0, greenValue=0, blueValue=0;
 
  blueValue = 255/100 * (temp36-15);
  redValue = 255/10 * (50-temp36);
  greenValue = 255/100 * (50-temp36);
  if(redValue > 255){
    redValue = 255;
  }
  if(greenValue > 255) {
    greenValue = 255;
  }
  if(blueValue > 255) {
    blueValue = 255;
  }
  
  if(redValue < 0) {
    redValue = 0;
  }
  if(greenValue < 0) {
    greenValue = 0;
  }
  if(blueValue < 0) {
    blueValue = 0;
  }
  
  setColor(redValue, greenValue, blueValue);
}

void setColor(int redValue, int greenValue, int blueValue) {
  analogWrite(redPin, redValue);
  analogWrite(greenPin, greenValue);
  analogWrite(bluePin, blueValue);
}