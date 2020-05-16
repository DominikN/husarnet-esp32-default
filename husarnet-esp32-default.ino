#include <WiFi.h>
#include <WiFiMulti.h>
#include <Husarnet.h>
#include <WebSocketsServer.h>
#include <WebServer.h>
#include <ArduinoJson.h>

#include "time.h"
#include "lut.h"

/* =============== config section start =============== */
#define HTTP_PORT 8000
#define WEBSOCKET_PORT 8001

#if __has_include("credentials.h")
#include "credentials.h"
#else

// WiFi credentials
#define NUM_NETWORKS 2
const char* ssidTab[NUM_NETWORKS] = {
  "wifi-ssid-1",
  "wifi-ssid-2",
};
const char* passwordTab[NUM_NETWORKS] = {
  "wifi-pass-for-ssid-1",
  "wifi-pass-for-ssid-2",
};

// Husarnet credentials
const char* hostName = "esp32websocket";
const char* husarnetJoinCode = "fc94:b01d:1803:8dd8:b293:5c7d:7639:932a/xxxxxxxxxxxxxxxxxxxxxx";

#endif
/* =============== config section end =============== */

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

WebServer server(HTTP_PORT);
WebSocketsServer webSocket = WebSocketsServer(WEBSOCKET_PORT);
WiFiMulti wifiMulti;

//https://arduinojson.org/v5/assistant/
StaticJsonDocument<100> jsonDocRx;  // {"set_output":"sine"}
StaticJsonDocument<200> jsonDocTx;  // {"output_type":"sine", "timestamp":1000000000, "value":0.12345}

String modeName = "sine"; //"square", "triangle", "none"

SemaphoreHandle_t sem = NULL;

const char* html =
#include "html.h"
  ;

const char* default_js =
#include "default_js.h"
  ;

bool getTime(time_t& rawtime) {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return 0;
  } else {
    rawtime = mktime(&timeinfo);
    return 1;
  }
}

void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      {
        Serial.printf("[%u] Disconnected\r\n", num);
      }
      break;
    case WStype_CONNECTED:
      {
        Serial.printf("\r\n[%u] Connection from Husarnet \r\n", num);
      }
      break;

    case WStype_TEXT:
      {
        Serial.printf("[%u] Text: %s\r\n", num, (char*)payload);

        jsonDocRx.clear();
        auto error = deserializeJson(jsonDocRx, payload);

        if (!error) {
          if (jsonDocRx["set_output"]) {
            String output = jsonDocRx["set_output"];
            if ( (output == "sine") || (output == "triangle") || (output == "square") || (output == "none")) {
              modeName = output;
            }
          }
        }
      }
      break;

    case WStype_BIN:
    case WStype_ERROR:
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
    default:
      break;
  }
}

void taskWifi( void * parameter );
void taskStatus( void * parameter );

void onHttpReqFunc() {
  delay(100);
  server.sendHeader("Connection", "close");
  server.send(200, "text/html", html);
}

void setup()
{
  Serial.begin(115200);

  sem = xSemaphoreCreateMutex();
  xSemaphoreTake(sem, ( TickType_t)portMAX_DELAY);

  xTaskCreate(
    taskWifi,          /* Task function. */
    "taskWifi",        /* String with name of task. */
    20000,            /* Stack size in bytes. */
    NULL,             /* Parameter passed as input of the task */
    1,                /* Priority of the task. */
    NULL);            /* Task handle. */

  xTaskCreate(
    taskStatus,          /* Task function. */
    "taskStatus",        /* String with name of task. */
    20000,            /* Stack size in bytes. */
    NULL,             /* Parameter passed as input of the task */
    1,                /* Priority of the task. */
    NULL);            /* Task handle. */
}

void taskWifi( void * parameter ) {
  uint8_t stat = WL_DISCONNECTED;

  /* Add Wi-Fi network credentials */
  for (int i = 0; i < NUM_NETWORKS; i++) {
    wifiMulti.addAP(ssidTab[i], passwordTab[i]);
    Serial.printf("WiFi %d: SSID: \"%s\" ; PASS: \"%s\"\r\n", i, ssidTab[i], passwordTab[i]);
  }

  while (stat != WL_CONNECTED) {
    stat = wifiMulti.run();
    Serial.printf("WiFi status: %d\r\n", (int)stat);
    delay(100);
  }
  Serial.printf("WiFi connected\r\n", (int)stat);
  Serial.printf("IP address: ");
  Serial.println(WiFi.localIP());

  Husarnet.join(husarnetJoinCode, hostName);
  Husarnet.start();

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  webSocket.begin();
  webSocket.onEvent(onWebSocketEvent);

  server.on("/", HTTP_GET, onHttpReqFunc);
  server.on("/index.html", HTTP_GET, onHttpReqFunc);
  server.on("/default.js", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "application/javascript", default_js);
    xSemaphoreGive( sem );
  });

  server.begin();

  xSemaphoreGive( sem );

  while (1) {
    while (WiFi.status() == WL_CONNECTED) {
      if (xSemaphoreTake(sem, ( TickType_t)0) == pdTRUE ) {
        webSocket.loop();
        xSemaphoreGive( sem );
      }

      server.handleClient();
      delay(10);
    }
    Serial.printf("WiFi disconnected, reconnecting\r\n");
    delay(500);
    stat = wifiMulti.run();
    Serial.printf("WiFi status: %d\r\n", (int)stat);
  }
}

void taskStatus( void * parameter )
{
  time_t currentTime;

  while (1) {
    if (xSemaphoreTake(sem, ( TickType_t)portMAX_DELAY) == pdTRUE ) {
      jsonDocTx.clear();
      if (getTime(currentTime)) {
        String output;
        jsonDocTx["output_type"] = modeName;
        jsonDocTx["timestamp"] = currentTime;
        jsonDocTx["value"] = getLutVal(modeName);
        serializeJson(jsonDocTx, output);

        if (webSocket.sendTXT(0, output)) {
          Serial.print(F("Sending: "));
          Serial.print(output);
          Serial.println();
        }
      }
      xSemaphoreGive( sem );
    }
    delay(200);
  }
}

void loop()
{
  Serial.printf("[RAM: %d]\r\n", esp_get_free_heap_size());
  delay(5000);
}
