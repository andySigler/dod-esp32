/* 
* WiFi Events

SYSTEM_EVENT_WIFI_READY               < ESP32 WiFi ready
SYSTEM_EVENT_SCAN_DONE                < ESP32 finish scanning AP
SYSTEM_EVENT_STA_START                < ESP32 station start
SYSTEM_EVENT_STA_STOP                 < ESP32 station stop
SYSTEM_EVENT_STA_CONNECTED            < ESP32 station connected to AP
SYSTEM_EVENT_STA_DISCONNECTED         < ESP32 station disconnected from AP
SYSTEM_EVENT_STA_AUTHMODE_CHANGE      < the auth mode of AP connected by ESP32 station changed
SYSTEM_EVENT_STA_GOT_IP               < ESP32 station got IP from connected AP
SYSTEM_EVENT_STA_LOST_IP              < ESP32 station lost IP and the IP is reset to 0
SYSTEM_EVENT_STA_WPS_ER_SUCCESS       < ESP32 station wps succeeds in enrollee mode
SYSTEM_EVENT_STA_WPS_ER_FAILED        < ESP32 station wps fails in enrollee mode
SYSTEM_EVENT_STA_WPS_ER_TIMEOUT       < ESP32 station wps timeout in enrollee mode
SYSTEM_EVENT_STA_WPS_ER_PIN           < ESP32 station wps pin code in enrollee mode
SYSTEM_EVENT_AP_START                 < ESP32 soft-AP start
SYSTEM_EVENT_AP_STOP                  < ESP32 soft-AP stop
SYSTEM_EVENT_AP_STACONNECTED          < a station connected to ESP32 soft-AP
SYSTEM_EVENT_AP_STADISCONNECTED       < a station disconnected from ESP32 soft-AP
SYSTEM_EVENT_AP_PROBEREQRECVED        < Receive probe request packet in soft-AP interface
SYSTEM_EVENT_GOT_IP6                  < ESP32 station or ap or ethernet interface v6IP addr is preferred
SYSTEM_EVENT_ETH_START                < ESP32 ethernet start
SYSTEM_EVENT_ETH_STOP                 < ESP32 ethernet stop
SYSTEM_EVENT_ETH_CONNECTED            < ESP32 ethernet phy link up
SYSTEM_EVENT_ETH_DISCONNECTED         < ESP32 ethernet phy link down
SYSTEM_EVENT_ETH_GOT_IP               < ESP32 ethernet got IP from connected AP
SYSTEM_EVENT_MAX
*/

#include <WiFi.h>

const char* ssid     = "dirtyfeet";
const char* password = "clinton2Ahill";

IPAddress local_IP(192, 168, 1, 21);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

IPAddress hue_IP(192, 168, 1, 11);

void WiFiEvent(WiFiEvent_t event) {
    switch (event){
      case SYSTEM_EVENT_WIFI_READY:
          Serial.print("(ms="); Serial.print(millis()); Serial.println(")");
          Serial.println("WiFi ready");
          break;
      case SYSTEM_EVENT_STA_CONNECTED:
          Serial.print("(ms="); Serial.print(millis()); Serial.println(")");
          Serial.println("WiFi connected to AP");
          break;
      case SYSTEM_EVENT_STA_GOT_IP:
          Serial.print("(ms="); Serial.print(millis()); Serial.println(")");
          Serial.print("WiFi got IP from AP: ");
          Serial.println(WiFi.localIP());
          Serial.print("\tGateway IP: "); Serial.println(WiFi.gatewayIP());
          Serial.print("\tSubnet Mask: "); Serial.println(WiFi.subnetMask());
          Serial.print("\tChannel: "); Serial.println(WiFi.channel());
          break;
      case SYSTEM_EVENT_STA_DISCONNECTED:
          Serial.print("(ms="); Serial.print(millis()); Serial.println(")");
          Serial.println("WiFi lost connection");
          break;
      default:
          Serial.print("(ms="); Serial.print(millis()); Serial.println(")");
          Serial.print("[WiFi-event] event: "); Serial.print(event);
          break;
    }
}

void setup() {
    Serial.begin(115200);
    // delete old config
//    WiFi.disconnect(true);
    WiFi.onEvent(WiFiEvent);

    if (!WiFi.config(local_IP, gateway, subnet)) {
      Serial.println("STA Failed to configure");
    }

    WiFi.begin(ssid, password);

    Serial.println();
    Serial.println();
    Serial.println("Wait for WiFi... ");
}

void loop() {
    delay(1000);
}
