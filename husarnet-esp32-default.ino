#include <WiFi.h>
#include <WebSocketsServer.h>
#include <Husarnet.h>
#include <ArduinoJson.h>

/* =============== config section start =============== */
#define HTTP_PORT 8000
#define WEBSOCKET_PORT 8001

const int BUTTON_PIN = 22;
const int LED_PIN = 16;

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
const char* husarnetJoinCode = "fc94:b01d:1803:8dd8:b293:5c7d:7639:932a/Tk72hzrLatV7rCCx5Qvgca";

#endif
/* =============== config section end =============== */


WebSocketsServer webSocket = WebSocketsServer(WEBSOCKET_PORT);
HusarnetServer server(HTTP_PORT);

StaticJsonDocument<400> jsonBufferTx;
StaticJsonDocument<400> jsonBufferRx;

const char* html =
#include "html.h"
  ;

bool wsconnected = false;

void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      {
        wsconnected = false;
        Serial.printf("[%u] Disconnected\r\n", num);
      }
      break;
    case WStype_CONNECTED:
      {
        wsconnected = true;
        Serial.printf("\r\n[%u] Connection from Husarnet \r\n", num);
      }
      break;

    case WStype_TEXT:
      {
        Serial.printf("[%u] Text:\r\n", num);
        for (int i = 0; i < length; i++) {
          Serial.printf("%c", (char)(*(payload + i)));
        }
        Serial.println();

        jsonBufferRx.clear();
        auto error = deserializeJson(jsonBufferRx, payload);

        uint8_t ledState = jsonBufferRx["led"];

        Serial.printf("LED state = %d\r\n", ledState);
        if (ledState == 1) {
          digitalWrite(LED_PIN, HIGH);
        }
        if (ledState == 0) {
          digitalWrite(LED_PIN, LOW);
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
void taskHTTP( void * parameter );
void taskWebSocket( void * parameter );
void taskStatus( void * parameter );

void setup()
{
  Serial.begin(115200);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  xTaskCreate(
    taskWifi,          /* Task function. */
    "taskWifi",        /* String with name of task. */
    10000,            /* Stack size in bytes. */
    NULL,             /* Parameter passed as input of the task */
    1,                /* Priority of the task. */
    NULL);            /* Task handle. */

  xTaskCreate(
    taskHTTP,          /* Task function. */
    "taskHTTP",        /* String with name of task. */
    10000,            /* Stack size in bytes. */
    NULL,             /* Parameter passed as input of the task */
    2,                /* Priority of the task. */
    NULL);            /* Task handle. */

  xTaskCreate(
    taskWebSocket,          /* Task function. */
    "taskWebSocket",        /* String with name of task. */
    10000,            /* Stack size in bytes. */
    NULL,             /* Parameter passed as input of the task */
    1,                /* Priority of the task. */
    NULL);            /* Task handle. */

  xTaskCreate(
    taskStatus,          /* Task function. */
    "taskStatus",        /* String with name of task. */
    10000,            /* Stack size in bytes. */
    NULL,             /* Parameter passed as input of the task */
    1,                /* Priority of the task. */
    NULL);            /* Task handle. */
}

void taskWifi( void * parameter ) {
  while (1) {
    for (int i = 0; i < NUM_NETWORKS; i++) {
      Serial.println();
      Serial.print("Connecting to ");
      Serial.print(ssidTab[i]);
      WiFi.begin(ssidTab[i], passwordTab[i]);
      for (int j = 0; j < 10; j++) {
        if (WiFi.status() != WL_CONNECTED) {
          delay(500);
          Serial.print(".");
        } else {
          Serial.println("done");
          Serial.print("IP address: ");
          Serial.println(WiFi.localIP());

          Husarnet.join(husarnetJoinCode, hostName);
          Husarnet.start();

          server.begin();

          while (WiFi.status() == WL_CONNECTED) {
            delay(500);
          }
        }
      }
    }
  }
}

void taskHTTP( void * parameter )
{
  String header;

  while (1) {
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
    }

    HusarnetClient client = server.available();

    if (client) {
      Serial.println("New Client.");
      String currentLine = "";
      Serial.printf("connected: %d\r\n", client.connected());
      while (client.connected()) {

        if (client.available()) {
          char c = client.read();
          Serial.write(c);
          header += c;
          if (c == '\n') {
            if (currentLine.length() == 0) {
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/html");
              client.println("Connection: close");
              client.println();

              client.println(html);
              break;
            } else {
              currentLine = "";
            }
          } else if (c != '\r') {
            currentLine += c;
          }
        }
      }

      header = "";

      client.stop();
      Serial.println("Client disconnected.");
      Serial.println("");
    } else {
      delay(200);
    }
  }
}

void taskWebSocket( void * parameter )
{
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  webSocket.begin();
  webSocket.onEvent(onWebSocketEvent);

  while (1) {
    webSocket.loop();
    delay(1);
  }
}

void taskStatus( void * parameter )
{
  String output;
  int cnt = 0;
  uint8_t button_status = 0;
  while (1) {
    if (wsconnected == true) {
      if (digitalRead(BUTTON_PIN) == LOW) {
        button_status = 1;
      } else {
        button_status = 0;
      }
      output = "";

      jsonBufferTx["counter"] = cnt++;
      jsonBufferTx["button"] = button_status;
      serializeJson(jsonBufferTx, output);

      Serial.print(F("Sending: "));
      Serial.print(output);
      Serial.println();

      webSocket.sendTXT(0, output);
    }
    delay(100);
  }
}

void loop()
{
  delay(5000);
}
