// Load Wi-Fi library
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <SPI.h>

// Replace with your network credentials
const char* ssid     = "SSID";
const char* password = "PASSWORD";

// Set web server port number to 80
WiFiServer server(80);

// Decode HTTP GET value
String redString = "0";
String greenString = "0";
String blueString = "0";
int pos1 = 255;
int pos2 = 255;
int pos3 = 255;
int pos4 = 0;

// Variable to store the HTTP req  uest
String header;

// Red, green, and blue pins for PWM control
const int redPin = 12;     // 13 corresponds to GPIO13
const int greenPin = 14;   // 12 corresponds to GPIO12
const int bluePin = 13;    // 14 corresponds to GPIO14

// Setting PWM bit resolution
const int resolution = 256;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void setup() {
  Serial.begin(115200);

  // configure LED PWM resolution/range and set pins to LOW
  analogWriteRange(resolution);
  pinMode(LED_BUILTIN, OUTPUT);
  analogWrite(redPin, 0);
  analogWrite(greenPin, 0);
  analogWrite(bluePin, 0);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
    server.begin();
  }

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {            // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // Display the HTML web page
            client.println("<!DOCTYPE html>");
            client.println("<html>");
            client.println("<head>");
            client.println("<script src=\"https://cdn.jsdelivr.net/npm/@jaames/iro@5\"></script>");
            client.println("<meta charset=\"utf-8\"/>");
            client.println("</head>");
            client.println("<style>");
            client.println(".button {");
            client.println("font-size: 30px;");

client.println("margin: 0;");
client.println("position: absolute;");
client.println("right: 50%;");
            client.println("}</style>");
            client.println("<body>");
            client.println("<div class=\"colorPicker\"></div>");
            client.println("<button class=\"button\" onclick=\"change()\">Cambiar color</button>");
            client.println("<p id=\"demo\"></p>");
            client.println("<script>");
            client.println("var colorPicker = new iro.ColorPicker(\".colorPicker\", {");
            client.println("width: 900,");
            client.println("handleRadius: 15,");
            // client.println("color: \"rgb("+String(unsigned(pos1))+","+String(unsigned(pos2))+","+String(unsigned(pos3))+"\",");
            client.println("color: \"rgb(255,255,255\",");
            client.println("borderWidth: 1,");
            client.println("borderColor: \"#fff\",");
            client.println("});");
            client.println("function change() {");
            client.println("document.location.href = \"?r\" + Math.round(colorPicker.color.rgb[\"r\"]) + \"g\" +  Math.round(colorPicker.color.rgb[\"g\"]) + \"b\" + Math.round(colorPicker.color.rgb[\"b\"]) + \"&\";");
            client.println("}");
            client.println("colorPicker.on([\"color:init\", \"color:change\"], function(color){");
            client.println("console.log(colorPicker.color.hexString);");
            // client.println("document.location.href = \"192.168.0.108/\" + \"?r\" + Math.round(colorPicker.color.rgb[0]) + \"g\" +  Math.round(colorPicker.color.rgb[1]) + \"b\" + Math.round(colorPicker.color.rgb[2]) + \"&\";");
            client.println("});");
            client.println("</script>");
            client.println("</body>");
            client.println("</html>");

            // The HTTP response ends with another blank line
            client.println();

            // Request sample: /?r201g32b255&
            // Red = 201 | Green = 32 | Blue = 255
            if(header.indexOf("GET /?r") >= 0) {
              pos1 = header.indexOf('r');
              pos2 = header.indexOf('g');
              pos3 = header.indexOf('b');
              pos4 = header.indexOf('&');
              redString = header.substring(pos1+1, pos2);
              greenString = header.substring(pos2+1, pos3);
              blueString = header.substring(pos3+1, pos4);
              Serial.println(redString.toInt());
              Serial.println(greenString.toInt());
              Serial.println(blueString.toInt());
              analogWrite(redPin, redString.toInt());
              analogWrite(greenPin, greenString.toInt());
              analogWrite(bluePin, blueString.toInt());
              digitalWrite(LED_BUILTIN, HIGH);
              delay(100);
              digitalWrite(LED_BUILTIN, LOW);
            }
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}

