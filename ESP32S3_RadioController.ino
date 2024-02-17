#include <WiFi.h>
#include <ESPAsyncWebServer.h>

const char* ssid = "XIAO_WebServer";
const char* password = "password";

AsyncWebServer server(80);

// Define the HTML content as a string
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 Web Server</title>
</head>
<body>
    <h1>Hello from ESP32!</h1>
    <p>This is a simple web server running on ESP32.</p>
</body>
</html>
)rawliteral";

void setup() {
    Serial.begin(115200);

    // Set up XIAO_ESP32S3 as an access point
    WiFi.softAP(ssid, password);
    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);

    // Start web server
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/html", index_html);
    });

    server.begin();
}

void loop() {
  // Nothing to do here in the loop
}
