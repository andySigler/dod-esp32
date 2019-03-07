////////////////////////////////
////////////////////////////////
////////////////////////////////

#include <esp_wifi.h>

#define WAKEUP_FROM_EXTERNAL_RTC_I0     1
#define WAKEUP_FROM_EXTERNAL_RTC_CNTL   2
#define WAKEUP_FROM_TIMER               3
#define WAKEUP_FROM_TOUCHPAD            4
#define WAKEUP_FROM_ULP                 5

#define SERIAL_DEBUG

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

////////////////////////////////
////////////////////////////////
////////////////////////////////

#define TOTAL_HALL_SENSORS          3
const uint8_t hall_pins[TOTAL_HALL_SENSORS] = {A7, A9, A1};
int hall_value[TOTAL_HALL_SENSORS] = {0, };

#define HALL_ENABLE_PIN             12

#define TOTAL_ACCEL_AXES            3
const uint8_t accel_pins[TOTAL_HALL_SENSORS] = {A4, A3, A2};
int accel_value[TOTAL_HALL_SENSORS] = {0, };

#define ACCEL_ENABLE_PIN            5

////////////////////////////////
////////////////////////////////
////////////////////////////////

bool is_correct_touch_pin() {
  touch_pad_t touchpad_code = esp_sleep_get_touchpad_wakeup_status();
  if (touchpad_status_lookup[touchpad_code] == TOUCHPAD_GPIO) {
#ifdef SERIAL_DEBUG
    Serial.print("[dod-touch] Touched by correct pin: "); Serial.println(TOUCHPAD_GPIO);
#endif
    return true;
  }
#ifdef SERIAL_DEBUG
  Serial.print("[dod-touch] Touchpad wakeup code #"); Serial.println(touchpad_code);
#endif
  return false;
}

bool did_wake_up_from_touchpad(){
  wakeup_code = esp_sleep_get_wakeup_cause();
  if (wakeup_code != WAKEUP_FROM_TOUCHPAD) {
#ifdef SERIAL_DEBUG
    Serial.print("[dod-wakup] Sleep wakeup code #"); Serial.println(wakeup_code);
#endif
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
  Serial.print("Accel: ");
  for (uint8_t i=0;i<TOTAL_ACCEL_AXES;i++) {
    accel_value[i] = analogRead(accel_pins[i]);
    Serial.print(accel_value[i]);Serial.print('\t');
  }
  delay(10);
  Serial.println();
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
  //
}

////////////////////////////////
////////////////////////////////
////////////////////////////////

void update_hall_sensors() {
  Serial.print("Hall: ");
  for (uint8_t i=0;i<TOTAL_HALL_SENSORS;i++) {
    hall_value[i] = analogRead(hall_pins[i]);
    Serial.print(hall_value[i]);Serial.print('\t');
  }
  delay(10);
  Serial.println();
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
  //
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
}

void speaker_play_event(int event_code) {
  /*
#define SPEAKER_EVENT_WAKEUP         1
#define SPEAKER_EVENT_FALSE_WAKEUP   2
#define SPEAKER_EVENT_FACE           3
#define SPEAKER_EVENT_SUCCESS        4
#define SPEAKER_EVENT_FAIL           5
  */
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
#ifdef SERIAL_DEBUG
  Serial.println("[dod-sleep] Going to sleep in 1 second...");
  Serial.println();
  Serial.println("////////////////////////////////");
  Serial.println("////////////////////////////////");
  Serial.println("////////////////////////////////");
  Serial.println();
  delay(1000);
#endif
  touchAttachInterrupt(TOUCHPAD_GPIO, empty_callback, TOUCHPAD_THRESHOLD);
  esp_sleep_enable_touchpad_wakeup();
//  esp_wifi_stop();
  esp_deep_sleep_start();
}

////////////////////////////////
////////////////////////////////
////////////////////////////////

void setup(){
#ifdef SERIAL_DEBUG
  Serial.begin(115200);
  Serial.println();Serial.println();
  Serial.println("[dod-setup] entering Dod");
#endif
  if (did_wake_up_from_touchpad()) {
    enable_accelerometer();
    enable_hall_sensors();
    speaker_setup();
    speaker_play_event(SPEAKER_EVENT_WAKEUP);
    bool just_started = true;
#ifdef SERIAL_DEBUG
    Serial.println("[dod-accelerometer] entering Movement loop");
#endif
    while (just_started || accelerometer_is_moving() || is_touching()) {
      just_started = false;
      update_accelerometer();
      update_hall_sensors();
    }
    int dod_face;
    if (is_hall_sensor_active()) {
      dod_face = get_hall_sensors_face();
#ifdef SERIAL_DEBUG
      Serial.print("[dod-hall] using Hall Sensor face: "); Serial.println(dod_face);
#endif
    }
    else {
      dod_face = get_accelerometer_face();
#ifdef SERIAL_DEBUG
      Serial.print("[dod-accelerometer] using Accelerometer face: "); Serial.println(dod_face);
#endif
    }
    disable_hall_sensors();
    disable_accelerometer();
    speaker_play_event(SPEAKER_EVENT_FACE);
    if (wifi_set_hue_scene(dod_face)) {
      speaker_play_event(SPEAKER_EVENT_SUCCESS);
    }
    else {
      speaker_play_event(SPEAKER_EVENT_FAIL);
    }
  }
  else {
    speaker_play_event(SPEAKER_EVENT_FALSE_WAKEUP);
  }
  go_to_sleep();
}

////////////////////////////////
////////////////////////////////
////////////////////////////////

void loop(){
  // this never runs
}

////////////////////////////////
////////////////////////////////
////////////////////////////////
