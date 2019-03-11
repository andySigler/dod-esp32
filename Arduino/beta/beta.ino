////////////////////////////////
////////////////////////////////
////////////////////////////////

#include <esp_wifi.h>
#include <WiFi.h>

const char* ssid     = "dirtyfeet";
const char* password = "clinton2Ahill";

IPAddress local_IP(192, 168, 1, 21);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

IPAddress hue_IP(192, 168, 1, 11);

////////////////////////////////
////////////////////////////////
////////////////////////////////

#define WAKEUP_FROM_EXTERNAL_RTC_I0     1
#define WAKEUP_FROM_EXTERNAL_RTC_CNTL   2
#define WAKEUP_FROM_TIMER               3
#define WAKEUP_FROM_TOUCHPAD            4
#define WAKEUP_FROM_ULP                 5

//#define SERIAL_DEBUG

////////////////////////////////
////////////////////////////////
////////////////////////////////

#define TOUCHPAD_GPIO         4
#define TOUCHPAD_THRESHOLD    40 // triggers when drops below this value

// map touchpad wakeup status code to the GPIO number
#define TOUCHPAD_TOTAL_PADS 10
const uint8_t touchpad_status_lookup[TOUCHPAD_TOTAL_PADS] = {
  4, 0, 2, 15, 13, 12, 14, 27, 33, 32
};

esp_sleep_wakeup_cause_t wakeup_code;

////////////////////////////////
////////////////////////////////
////////////////////////////////

#define SPEAKER_PIN                 23
#define SPEAKER_CHANNEL             0
#define SPEAKER_RESOLUTION          8
#define SPEAKER_FREQ_DEFAULT        800
#define SPEAKER_FREQ_MIN            400
#define SPEAKER_FREQ_MAX            6000
#define SPEAKER_PWM_MIN_VOL         0
#define SPEAKER_PWM_MAX_VOL         60

#define SPEAKER_EVENT_WAKEUP         1
#define SPEAKER_EVENT_FALSE_WAKEUP   2
#define SPEAKER_EVENT_FACE           3
#define SPEAKER_EVENT_SUCCESS        4
#define SPEAKER_EVENT_FAIL           5

bool did_speaker_setup = false;

////////////////////////////////
////////////////////////////////
////////////////////////////////

uint8_t CURRENT_FACE = 0;

#define TOTAL_HALL_SENSORS          4
#define HALL_ENABLE_PIN             12
#define HALL_PIN_BOTTOM             A1
#define HALL_PIN_TOP                A7
#define HALL_PIN_WIDE               A13
#define HALL_PIN_SMALL              A9
const uint8_t hall_pins[TOTAL_HALL_SENSORS] = {
  HALL_PIN_BOTTOM,
  HALL_PIN_TOP,
  HALL_PIN_SMALL,
  HALL_PIN_WIDE
};
int hall_value[TOTAL_HALL_SENSORS] = {0, };

#define TOTAL_ACCEL_AXES            3
const uint8_t accel_pins[TOTAL_HALL_SENSORS] = {A4, A3, A2};
int accel_value[TOTAL_HALL_SENSORS] = {0, };

#define ACCEL_ENABLE_PIN            5

////////////////////////////////
////////////////////////////////
////////////////////////////////

void begin_serial() {
#ifdef SERIAL_DEBUG
  Serial.begin(115200);
#endif
}

void println() {
#ifdef SERIAL_DEBUG
  Serial.println();
#endif
}

void println(String s) {
#ifdef SERIAL_DEBUG
  Serial.println(s);
#endif
}

void println(char c) {
#ifdef SERIAL_DEBUG
  Serial.println(c);
#endif
}

void println(float f) {
#ifdef SERIAL_DEBUG
  Serial.println(f);
#endif
}

void println(int i) {
#ifdef SERIAL_DEBUG
  Serial.println(i);
#endif
}

void println(unsigned long i) {
#ifdef SERIAL_DEBUG
  Serial.println(i);
#endif
}

void println(IPAddress i) {
#ifdef SERIAL_DEBUG
  Serial.println(i);
#endif
}

void print(String s) {
#ifdef SERIAL_DEBUG
  Serial.print(s);
#endif
}

void print(char c) {
#ifdef SERIAL_DEBUG
  Serial.print(c);
#endif
}

void print(float f) {
#ifdef SERIAL_DEBUG
  Serial.print(f);
#endif
}

void print(int i) {
#ifdef SERIAL_DEBUG
  Serial.print(i);
#endif
}

void print(unsigned long i) {
#ifdef SERIAL_DEBUG
  Serial.print(i);
#endif
}

void print(IPAddress i) {
#ifdef SERIAL_DEBUG
  Serial.print(i);
#endif
}

////////////////////////////////
////////////////////////////////
////////////////////////////////

void WiFiEvent(WiFiEvent_t event) {
    switch (event){
      case SYSTEM_EVENT_WIFI_READY:
          print("(ms="); print(millis()); println(")");
          println("WiFi ready");
          break;
      case SYSTEM_EVENT_STA_CONNECTED:
          print("(ms="); print(millis()); println(")");
          println("WiFi connected to AP");
          break;
      case SYSTEM_EVENT_STA_GOT_IP:
          print("(ms="); print(millis()); println(")");
          print("WiFi got IP from AP: ");
          println(WiFi.localIP());
          print("\tGateway IP: "); println(WiFi.gatewayIP());
          print("\tSubnet Mask: "); println(WiFi.subnetMask());
          print("\tChannel: "); println(WiFi.channel());
          WiFi.disconnect();
          break;
      case SYSTEM_EVENT_STA_DISCONNECTED:
          print("(ms="); print(millis()); println(")");
          println("WiFi lost connection");
          speaker_play_event(SPEAKER_EVENT_FALSE_WAKEUP);
          go_to_sleep();  // function never returns
          break;
      default:
          print("(ms="); print(millis()); println(")");
          print("[WiFi-event] event: "); print(event);
          break;
    }
}

////////////////////////////////
////////////////////////////////
////////////////////////////////

bool is_correct_touch_pin() {
  touch_pad_t touchpad_code = esp_sleep_get_touchpad_wakeup_status();
  if (touchpad_status_lookup[touchpad_code] == TOUCHPAD_GPIO) {
    print("[dod-touch] Touched by correct pin: "); println(TOUCHPAD_GPIO);
    return true;
  }
  print("[dod-touch] Touchpad wakeup code #"); println(touchpad_code);
  return false;
}

bool did_wake_up_from_touchpad(){
  wakeup_code = esp_sleep_get_wakeup_cause();
  if (wakeup_code != WAKEUP_FROM_TOUCHPAD) {
    print("[dod-wakup] Sleep wakeup code #"); println(wakeup_code);
    return false;
  }
  return is_correct_touch_pin();
}

////////////////////////////////
////////////////////////////////
////////////////////////////////

void empty_callback(){
  // used as touchpad interrupt argument
}

bool is_touching() {
  return (touchRead(TOUCHPAD_GPIO) < TOUCHPAD_THRESHOLD);
}

void block_until_touch_released() {
  while (is_touching()) {
    // wait until the touchpad is no longer being touched
  }
}

////////////////////////////////
////////////////////////////////
////////////////////////////////

void update_accelerometer() {
  print("Accel: ");
  for (uint8_t i=0;i<TOTAL_ACCEL_AXES;i++) {
    accel_value[i] = analogRead(accel_pins[i]);
    print(accel_value[i]);print('\t');
  }
  delay(10);
  println();
}

bool accelerometer_is_moving() {
  //
}

void enable_accelerometer() {
  pinMode(ACCEL_ENABLE_PIN, OUTPUT);
  digitalWrite(ACCEL_ENABLE_PIN, LOW);
  for (uint8_t i=0;i<TOTAL_ACCEL_AXES;i++) {
    pinMode(accel_pins[i], INPUT);
  }
  delay(1);
}

void disable_accelerometer() {
  digitalWrite(ACCEL_ENABLE_PIN, HIGH);
}

int get_accelerometer_face() {
    int dod_face = random(0, TOTAL_HALL_SENSORS);
      print("[dod-accelerometer] using Accelerometer face: "); println(dod_face);
}

////////////////////////////////
////////////////////////////////
////////////////////////////////

void update_hall_sensors() {
  print("Hall: ");
  for (uint8_t i=0;i<TOTAL_HALL_SENSORS;i++) {
    hall_value[i] = analogRead(hall_pins[i]);
    print(hall_value[i]);print('\t');
  }
  delay(10);
  println();
}

bool is_hall_sensor_active() {
  //
}

void enable_hall_sensors() {
  pinMode(HALL_ENABLE_PIN, OUTPUT);
  digitalWrite(HALL_ENABLE_PIN, HIGH);
  for (uint8_t i=0;i<TOTAL_HALL_SENSORS;i++) {
    pinMode(hall_pins[i], INPUT);
  }
  delay(1);
}

void disable_hall_sensors() {
  digitalWrite(HALL_ENABLE_PIN, LOW);
}

int get_hall_sensors_face() {
  int dod_face = random(0, TOTAL_HALL_SENSORS);
  print("[dod-hall] using Hall Sensor face: "); println(dod_face);
  return dod_face;
}

////////////////////////////////
////////////////////////////////
////////////////////////////////

bool wifi_set_hue_scene(int face) {
  return true;
}

////////////////////////////////
////////////////////////////////
////////////////////////////////

void speaker_set_volume(int volume) {
  if (volume > SPEAKER_PWM_MAX_VOL) volume = SPEAKER_PWM_MAX_VOL;
  if (volume < SPEAKER_PWM_MIN_VOL) volume = SPEAKER_PWM_MIN_VOL;
  ledcWrite(SPEAKER_CHANNEL, volume);
}

void speaker_set_frequency(int frequency) {
  if (frequency > SPEAKER_FREQ_MAX) frequency = SPEAKER_FREQ_MAX;
  if (frequency < SPEAKER_FREQ_MIN) frequency = SPEAKER_FREQ_MIN;
  ledcWriteTone(SPEAKER_CHANNEL, frequency);
}

void speaker_setup() {
  ledcSetup(SPEAKER_CHANNEL, SPEAKER_FREQ_DEFAULT, SPEAKER_RESOLUTION);
  ledcAttachPin(SPEAKER_PIN, SPEAKER_CHANNEL);
  speaker_set_volume(0);
  did_speaker_setup = true;
}

void speaker_play_event(int event_code) {
  if (did_speaker_setup == false) {
    speaker_setup();
  }
  switch (event_code) {
    case SPEAKER_EVENT_WAKEUP:
      speaker_set_frequency(1200);
      speaker_set_volume(SPEAKER_PWM_MAX_VOL);
      delay(200);
      speaker_set_volume(SPEAKER_PWM_MIN_VOL);
      break;
    case SPEAKER_EVENT_FALSE_WAKEUP:
      speaker_set_frequency(400);
      speaker_set_volume(SPEAKER_PWM_MAX_VOL);
      delay(1000);
      speaker_set_volume(SPEAKER_PWM_MIN_VOL);
      break;
    case SPEAKER_EVENT_FACE:
      speaker_set_frequency(2400);
      speaker_set_volume(SPEAKER_PWM_MAX_VOL);
      delay(200);
      speaker_set_volume(SPEAKER_PWM_MIN_VOL);
      break;
    case SPEAKER_EVENT_SUCCESS:
      speaker_set_frequency(1200);
      speaker_set_volume(SPEAKER_PWM_MAX_VOL);
      delay(100);
      speaker_set_volume(SPEAKER_PWM_MIN_VOL);
      delay(100);
      speaker_set_volume(SPEAKER_PWM_MAX_VOL);
      delay(100);
      speaker_set_volume(SPEAKER_PWM_MIN_VOL);
      delay(100);
      speaker_set_volume(SPEAKER_PWM_MAX_VOL);
      delay(100);
      speaker_set_volume(SPEAKER_PWM_MIN_VOL);
      delay(100);
      break;
    case SPEAKER_EVENT_FAIL:
      speaker_set_frequency(400);
      speaker_set_volume(SPEAKER_PWM_MAX_VOL);
      delay(1000);
      speaker_set_volume(SPEAKER_PWM_MIN_VOL);
      break;
    default:
      break;
  }
}

////////////////////////////////
////////////////////////////////
////////////////////////////////

void go_to_sleep() {
  println("[dod-sleep] Going to sleep in 1 second...");
  println();
  println("////////////////////////////////");
  println("////////////////////////////////");
  println("////////////////////////////////");
  println();
#ifdef SERIAL_DEBUG
  // wait for buffer to empty towards host
  delay(1000);
#endif
  touchAttachInterrupt(TOUCHPAD_GPIO, empty_callback, TOUCHPAD_THRESHOLD);
  esp_sleep_enable_touchpad_wakeup();
  esp_wifi_stop();
  esp_deep_sleep_start();
}

////////////////////////////////
////////////////////////////////
////////////////////////////////

int get_face_from_movements() {
  println("[dod-sensors] entering get_face_from_movements loop");
  enable_accelerometer();
  enable_hall_sensors();
  while (accelerometer_is_moving() || is_touching()) {
    update_accelerometer();
    update_hall_sensors();
  }
  disable_hall_sensors();
  disable_accelerometer();
  if (is_hall_sensor_active()) {
    CURRENT_FACE = get_hall_sensors_face();
  }
  else {
    CURRENT_FACE = get_accelerometer_face();
  }
}

////////////////////////////////
////////////////////////////////
////////////////////////////////

void setup(){

  begin_serial();
  println();println();
  println("[dod-setup] entering Dod");

  if (!did_wake_up_from_touchpad()) {
    WiFi.disconnect(true);
    WiFi.onEvent(WiFiEvent);
//    if (!WiFi.config(local_IP, gateway, subnet)) {
//      Serial.println("STA Failed to configure");
//    }
    WiFi.begin(ssid, password);
    println("Wait for WiFi... ");
    while (1) {
      delay(1000);
    }
  }
}

////////////////////////////////
////////////////////////////////
////////////////////////////////

void loop(){
  speaker_play_event(SPEAKER_EVENT_WAKEUP);
  get_face_from_movements();
  speaker_play_event(SPEAKER_EVENT_FACE);
  if (wifi_set_hue_scene(CURRENT_FACE)) {
    speaker_play_event(SPEAKER_EVENT_SUCCESS);
  } else {
    speaker_play_event(SPEAKER_EVENT_FAIL);
  }
  go_to_sleep();  // "loop" never actually loops, because we sleep
}

////////////////////////////////
////////////////////////////////
////////////////////////////////
