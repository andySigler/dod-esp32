#include <esp_wifi.h>

#define SERIAL_DEBUG

#define TOUCHPAD_GPIO 4
#define THRESHOLD 40  // Greater the value, more the sensitivity

// map touchpad wakeup status code to the GPIO number
#define TOUCHPAD_TOTAL_PADS 10
const uint8_t touchpad_status_lookup[TOUCHPAD_TOTAL_PADS] = {
  4, 0, 2, 15, 13, 12, 14, 27, 33, 32
};

#define WAKEUP_FROM_EXTERNAL_RTC_I0 1
#define WAKEUP_FROM_EXTERNAL_RTC_CNTL 2
#define WAKEUP_FROM_TIMER 3
#define WAKEUP_FROM_TOUCHPAD 4
#define WAKEUP_FROM_ULP 5

bool did_wake_up_from_touchpad(){
  esp_sleep_wakeup_cause_t wakeup_code = esp_sleep_get_wakeup_cause();
  if (wakeup_code == WAKEUP_FROM_TOUCHPAD) {
    touch_pad_t touchpad_code = esp_sleep_get_touchpad_wakeup_status();
    if (touchpad_status_lookup[touchpad_code] == TOUCHPAD_GPIO) {
      return true;
    }
    else {
#ifdef SERIAL_DEBUG
      Serial.print("Touchpad wakeup code #"); Serial.println(touchpad_code);
#endif
    }
  }
  else {
#ifdef SERIAL_DEBUG
    Serial.print("Sleep wakeup code #"); Serial.println(wakeup_code);
#endif
  }
  return false;
}

bool is_still_touching() {
  return (touchRead(TOUCHPAD_GPIO) < THRESHOLD);
}

void empty_callback(){}

void setup(){
#ifdef SERIAL_DEBUG
  Serial.begin(115200);
#endif

  if (did_wake_up_from_touchpad()) {
#ifdef SERIAL_DEBUG
    Serial.println("Woke from touchpad");
#endif
    while (is_still_touching()) {
      // wait until the touchpad is no longer being touched
    }
  }

#ifdef SERIAL_DEBUG
  Serial.println("Going to sleep now");
#endif
  touchAttachInterrupt(TOUCHPAD_GPIO, empty_callback, THRESHOLD);
  esp_sleep_enable_touchpad_wakeup();
  esp_wifi_stop();
  esp_deep_sleep_start();
}

void loop(){}
