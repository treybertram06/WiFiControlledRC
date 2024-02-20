#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ESP32Servo.h>

const char* ssid = "XIAO_WebServer";
const char* password = "password";

AsyncWebServer server(80);

Servo ESC; 
Servo Steer;
int steeringValue = 0;
int throttleValue = 0;
int steeringValueFinal;
int throttleValueFinal;
int steeringPin = D3;
int throttlePin = D2;
int minPulseWidth = 1000;
int maxPulseWidth = 2000;

// Define the HTML content as a string
const char index_html[] PROGMEM = R"rawliteral(
  <!DOCTYPE html>
  <html lang="en">
  <head>
      <meta charset="UTF-8">
      <meta name="viewport" content="width=device-width, initial-scale=1.0">
      <title>ESP32 Web Server</title>
      <style>
          body {
              display: flex;
              flex-direction: column;
              align-items: center;
              justify-content: center;
              height: 100vh;
              text-align: center;
          }
          .container {
              display: flex;
              flex-direction: column;
              gap: 20px;
          }
          @media screen and (orientation:landscape) {
              .container {
                  flex-direction: row;
              }
          }
      </style>
  </head>
  <body>
      <h1>ESP32 WiFi RC Controller!</h1>
      <div class="container">
          <div class="slider-container">
              <input type="range" min="-100" max="100" value="0" class="slider" id="throttleRange" oninput="throttleFunction(this.value)" orient="vertical">
          </div>
          <input type="range" min="-100" max="100" value="0" class="slider" id="steeringRange" oninput="steeringFunction(this.value)">
      </div>
      <script>
          function throttleFunction(value) {
              var xhr = new XMLHttpRequest();
              xhr.open("GET", "/throttle?value=" + value, true);
              xhr.send();
              console.log("Throttle value: " + value);
          }
          function steeringFunction(value) {
              var xhr = new XMLHttpRequest();
              xhr.open("GET", "/steering?value=" + value, true);
              xhr.send();
              console.log("Steering value: " + value);
          }
          document.getElementById('steeringRange').addEventListener('change', function(){
              this.value = 0;
          });
          document.getElementById('throttleRange').addEventListener('change', function(){
              this.value = 0;
          });
      </script>
  </body>
  </html>
)rawliteral";

void setup() {

    //Webserver Setup
    Serial.begin(115200);
    Serial.println("Setup started");

    WiFi.softAP(ssid, password);
    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/html", index_html);
    });
    server.on("/throttle", HTTP_GET, [](AsyncWebServerRequest *request){
        String value = request->getParam("value")->value();
        throttleValue = value.toInt(); // update the global variable directly
        request->send(200, "text/plain", "Throttle set to " + value);
    });

    server.on("/steering", HTTP_GET, [](AsyncWebServerRequest *request){
        String value = request->getParam("value")->value();
        steeringValue = value.toInt(); // update the global variable directly
        request->send(200, "text/plain", "Steering value set to " + value);
    });

    server.begin();


    //RC control
    ESC.attach(throttlePin, minPulseWidth, maxPulseWidth);
    Steer.attach(steeringPin);

    ESC.writeMicroseconds(1000); // Send a low signal to keep the motor off initially
    delay(1000); // Wait for a second
}

void loop() {
  throttleValueFinal = map(throttleValue, -100, 100, 0, 180);
  steeringValueFinal = map(steeringValue, -100, 100, 0, 180);
  Serial.println("Throttle: " + String(throttleValue) + ", Steering: " + String(steeringValue));
  ESC.write(throttleValue);
  Steer.write(steeringValueFinal);
}
