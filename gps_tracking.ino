  // Rx D1  TX D2 
  #include <TinyGPS++.h>
  #include <SoftwareSerial.h>
  #include <ESP8266WiFi.h>
  #include <ESP8266HTTPClient.h>
  #include <ArduinoJson.h>

  TinyGPSPlus gps;
  SoftwareSerial SerialGPS(4, 5); 

  const char* ssid = "Cuon Oi";
  const char* password = "lethituyetmai";
  const char* backendUrl = "https://e95d-2001-ee0-4b62-5ec0-4084-ed0b-337a-efb1.ngrok-free.app/bin/update";


  // Default location (16°03'58.0"N 108°08'38.6"E)
  float Latitude = 16.066111, Longitude = 108.144056;
  String LatitudeString = "16.066111", LongitudeString = "108.144056";

  int year , month , date, hour , minute , second;
  String DateString , TimeString ;

  WiFiServer server(80);

  void setup() {
    Serial.begin(9600);
    SerialGPS.begin(9600);
    Serial.println();
    Serial.print("Connecting to WiFi");
    WiFi.begin(ssid, password);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 60) { // 30s timeout
      delay(500);
      Serial.print(".");
      attempts++;
    }
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("");
      Serial.println("WiFi connected");
      server.begin();
      Serial.println("Server started");
      Serial.println(WiFi.localIP());
    } else {
      Serial.println("");
      Serial.println("WiFi connection failed");
    }
  }

  void loop() {
    while (SerialGPS.available() > 0)
      if (gps.encode(SerialGPS.read())) {
        if (gps.location.isValid()) {
          Latitude = gps.location.lat();
          LatitudeString = String(Latitude, 6);
          Longitude = gps.location.lng();
          LongitudeString = String(Longitude, 6);
        }

        if (gps.date.isValid()) {
          DateString = "";
          date = gps.date.day();
          month = gps.date.month();
          year = gps.date.year();

          if (date < 10) DateString += '0';
          DateString += String(date);
          DateString += " / ";

          if (month < 10) DateString += '0';
          DateString += String(month);
          DateString += " / ";
          DateString += String(year);
        }

        if (gps.time.isValid()) {
          TimeString = "";
          hour = gps.time.hour() + 7; // adjust UTC
          minute = gps.time.minute();
          second = gps.time.second();

          if (hour < 10) TimeString += '0';
          TimeString += String(hour);
          TimeString += " : ";

          if (minute < 10) TimeString += '0';
          TimeString += String(minute);
          TimeString += " : ";

          if (second < 10) TimeString += '0';
          TimeString += String(second);
        }
      }

    /* WiFiClient client = server.available();
    if (!client) {
      return;
    }

    // Response
    String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
    s += "<!DOCTYPE html><html><head><title>NEO-6M GPS Readings</title><style>";
    s += "table, th, td {border: 1px solid blue;}</style></head><body>";
    s += "<h1 style=\"font-size:300%; text-align:center;\">NEO-6M GPS Readings</h1>";
    s += "<p style=\"font-size:150%; text-align:center;\"><b>Location Details</b></p>";
    s += "<table align=\"center\" style=\"width:50%\">";
    s += "<tr><th>Latitude</th><td align=\"center\">" + LatitudeString + "</td></tr>";
    s += "<tr><th>Longitude</th><td align=\"center\">" + LongitudeString + "</td></tr>";
    s += "<tr><th>Date</th><td align=\"center\">" + DateString + "</td></tr>";
    s += "<tr><th>Time</th><td align=\"center\">" + TimeString + "</td></tr>";
    s += "</table>";

    s += "<p align=\"center\"><a style=\"color:RED; font-size:125%;\" href=\"http://maps.google.com/maps?&z=15&mrt=yp&t=k&q=";
    s += LatitudeString + "+" + LongitudeString + "\" target=\"_top\">Click here</a> to open the location in Google Maps.</p>";

    s += "</body></html>\n"; 

    client.print(s);
    delay(100); */

    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      
      // Prepare JSON payload
      DynamicJsonDocument jsonDoc(200);
      jsonDoc["id"] = "1";
      jsonDoc["lat"] = 16.066111; // Your latitude value
      jsonDoc["lng"] = 108.144056; // Your longitude value
      jsonDoc["status"] = "0"; // Replace with your desired status

      // 0 empty - 1 full
      
      String payload;
      serializeJson(jsonDoc, payload);
  WiFiClientSecure client;
client.setInsecure(); //the magic line, use with caution
    // Your IP address with path or Domain name with URL path 
    http.begin(client, backendUrl);
      // Send POST request
      http.addHeader("Content-Type", "application/json");
      int httpResponseCode = http.POST(payload);
      
      // Check response
      if (httpResponseCode > 0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        
        String response = http.getString();
        Serial.println("Response: " + response);
      } else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      
      http.end();
    } else {
      Serial.println("WiFi not connected");
    }
    
    delay(10000); // Adjust delay according to your needs
  }

