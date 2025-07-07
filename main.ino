#include "thingProperties.h"
#include <ESP8266WiFi.h>
#include <ArduinoHttpClient.h>

// Ganti dengan nama perangkat Anda
const char* deviceName = "HansLaptop";

// Koneksi WiFi
const char* ssid = "";
const char* password = "";

// Koneksi client
WiFiClient wifiClient;

void setup() {
  Serial.begin(9600);
  delay(1500); 
  initProperties();

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi!");

  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();
}

void loop() {
  ArduinoCloud.update();
}

void sendRequest(const char* host, int port, String path, bool useHttps = false) {
  HttpClient client = HttpClient(wifiClient, host, port);

  Serial.print("Requesting: ");
  Serial.println(path);

  client.get(path);

  int statusCode = client.responseStatusCode();
  String response = client.responseBody();

  Serial.print("Status Code: ");
  Serial.println(statusCode);
  Serial.print("Response: ");
  Serial.println(response);

  client.stop();
}

void onWakeOnLanChange() {
  if (wakeOnLan) {
    Serial.println("wakeOnLan is true, sending request to REST API...");
    sendRequest("wol.local.apps.net", 80, "/wake/" + String(deviceName));
  } else {
    Serial.println("wakeOnLan is false, sending request to REST API...");
    sendRequest("utility.local.apps.net", 80, "/shutdown/" + String(deviceName));
  }
}

void onSpotifyPlayPauseChange() {
  String action = spotifyPlayPause ? "play" : "pause";
  sendRequest("workflow.hansprojects.com", 80, "/webhook/spotify/" + action, true);
}

void onSpotifyNextChange() {
  if (spotify_Next) {
    sendRequest("workflow.hansprojects.com", 80, "/webhook/spotify/next", true);
  }
}

void onSpotifyPrevChange() {
  if (spotify_Prev) {
    sendRequest("workflow.hansprojects.com", 80, "/webhook/spotify/prev", true);
  }
}

// Fungsi umum untuk request

