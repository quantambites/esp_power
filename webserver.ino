#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

const char *ssid = "#####";
const char *password = "#####";
const char *serverAddress = "192.168.0.110";//www.google.com  http:\\192.168.0.110

AsyncWebServer server(80);
int accessCount = 0;

const char index_html[] PROGMEM = R"(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Black Market</title>
  <link rel="stylesheet" href="https://fonts.googleapis.com/css2?family=Open+Sans:wght@400;700&display=swap">
  <style>
    * {
      margin: 0;
      padding: 0;
      box-sizing: border-box;
      font-family: 'Open Sans', sans-serif;
    }

    body {
      background-image: url('https://images.unsplash.com/photo-1485470733090-0aae1788d5af?ixlib=rb-4.0.3&ixid=MnwxMjA3fDB8MHxwaG90by1wYWdlfHx8fGVufDB8fHx8&auto=format&fit=crop&w=1517&q=80');
      background-size: cover;
    }

    .navbar {
      overflow: hidden;
      position: fixed;
      top: 36px;
      right: 45px;
      z-index: 1000;
    }

    .navbar a {
      float: right;
      display: block;
      color: white;
      text-align: center;
      padding: 10px 35px;
      text-decoration: none;
      border-radius: 15px;
      background-color: rgba(255, 255, 255, 0);
      margin-left: 45px;
      box-shadow: 0 4px 8px rgba(255, 255, 255, 0);
      transition: background-color 0.3s;
    }

    .navbar a:hover {
      background-color: rgba(0, 0, 0, 0.7);
    }

    .logo {
      position: absolute;
      top: 50%;
      transform: translateY(-50%);
      width: 40px;
      height: 40px;
      display: inline-block;
      background-color: rgba(255, 255, 255, 0);
      border-radius: 50%;
      text-align: center;
      line-height: 40px;
      font-size: 40px;
      color: #ffffff;
      font-weight: bold;
    }

    .logo.left {
      left: 20px;
    }

    .logo.right {
      right: 20px;
    }

    .content {
      position: absolute;
      top: 50%;
      left: 50%;
      transform: translate(-50%, -50%);
      text-align: center;
      color: rgba(236, 229, 229, 0.774);
    }

    .name {
      position:absolute;
      top: 25px;
      left: 50px;
      color: rgba(236, 229, 229, 0.774);
    }

    .name h1 {
      font-family: 'Arial', 'Impact', 'Haettenschweiler', 'Arial Narrow Bold', sans-serif;
      font-weight: bolder;
      font-size: 3.5rem;
      margin-bottom: 20px;
      letter-spacing: 2px;
    }

    .tagline {
      font-size: 2rem;
      font-weight: bold;
      color: white;
      background-color: rgba(252, 252, 252, 0);
      padding: 10px 20px;
      border-radius: 10px;
      display: inline-block;
      margin-right: 10px;
    }

    .visit-now-btn {
      display: inline-block;
      padding: 15px 30px;
      background-color: #ffffffe4;
      color: #333;
      text-decoration: none;
      border-radius: 20px;
      margin-top: 20px;
      font-weight: bold;
      transition: background-color 0.3s;
    }

    .visit-now-btn:hover {
      background-color: #ddd;
    }
  </style>
</head>
<body>

<div class="navbar">  
  <a href="#">Services</a>
  <a href="#">Cancel</a>
  <a href="#">Checkout</a>
  <a href="#">Book</a>
  <a href="#">Home</a>
</div>

<div class="logo left"><<</div>
<div class="logo right">>></div>
<div class="name">
  <h1>Visit I.O.</h1>
</div>
<div class="content">
  
  <div class="tagline">Himalayas</div>
  <a href="#" class="visit-now-btn">Visit Now</a>
</div>

</body>
</html>
)";
void updateOLED() {
  // Display access count on OLED
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,43);
  display.print("Access count: ");
  display.println(accessCount);
  display.display();
}


void setup() {
  Serial.begin(115200);

  // Initialize the OLED display with I2C connection
   if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C,-1)){//if (!display.begin(0x3C, 21, 22))
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  display.display();
  delay(2000);
  display.clearDisplay();
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.print("Connecting to WiFi...");
    display.display();
  }

  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());

  // Display IP address on OLED
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
 if (WiFi.isConnected()) {
  display.print("Connected to WiFi !!!");
} else {
  display.print("NO WiFi available");
}
  display.clearDisplay();
  display.print("Website hoasted at--");
  display.println("\n");
  display.print("http:\\\\");
  display.println(WiFi.localIP());
  display.display();

 // Capture variables explicitly for the lambda function
  AsyncWebServer* serverPtr = &server;
  int* accessCountPtr = &accessCount;

  server.on("/", HTTP_GET, [serverPtr, accessCountPtr](AsyncWebServerRequest *request) {
    (*accessCountPtr)++;
    request->send_P(200, "text/html", index_html, [accessCountPtr](const String &contentType) -> String {
      return String("Access-Control-Allow-Origin: *\r\n");
    });
    updateOLED(); // Update OLED display with access count
  });
   
  server.begin();
}

void loop() {
    delay(5000); // Wait for 5 seconds before "pinging" again

  unsigned long startTime = millis();
  WiFiClient client;
  if (client.connect(serverAddress, 80)) {
    unsigned long endTime = millis();
    unsigned long pingTime = endTime - startTime;
    Serial.print("Ping success! Round-trip time: ");
    Serial.print(pingTime);
    Serial.println(" ms");

    // Display ping result on OLED
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0,53);
    display.print("Ping: ");
    display.print(pingTime);
    display.print(" ms");
    display.display();


    client.stop();
  } else {
    Serial.println("Ping failed.");
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0,53);
    display.print("Ping failed.");
    display.display();

  }
}


