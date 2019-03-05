#define SPEAKER_PIN             12
#define SPEAKER_CHANNEL         0
#define SPEAKER_RESOLUTION      8
#define SPEAKER_FREQ_DEFAULT    800
#define SPEAKER_FREQ_MIN        400
#define SPEAKER_FREQ_MAX        6000
#define SPEAKER_PWM_MIN_VOL     0
#define SPEAKER_PWM_MAX_VOL     60

void speaker_set_volume(int volume) {
  if (volume > SPEAKER_PWM_MAX_VOL) volume = SPEAKER_PWM_MAX_VOL;
  if (volume < SPEAKER_PWM_MIN_VOL) volume = SPEAKER_PWM_MIN_VOL;
  ledcWrite(SPEAKER_CHANNEL, volume);
}

void speaker_set_frequency(int frequency) {
  Serial.print("Freq: "); Serial.println(frequency);
  ledcWriteTone(SPEAKER_CHANNEL, frequency);
}

void speaker_setup() {
  ledcSetup(SPEAKER_CHANNEL, SPEAKER_FREQ_DEFAULT, SPEAKER_RESOLUTION);
  ledcAttachPin(SPEAKER_PIN, SPEAKER_CHANNEL);
  speaker_set_volume(0);
}
 
void setup() {
  Serial.begin(115200);
  speaker_setup();
}

int freq = SPEAKER_FREQ_MIN;
 
void loop() {
  speaker_set_frequency(freq);
  freq *= 2;
  if (freq > SPEAKER_FREQ_MAX) freq = SPEAKER_FREQ_MIN;  
  for (int d = 0; d < SPEAKER_PWM_MAX_VOL; d++){
    Serial.println(d);
    speaker_set_volume(d);
    delay(20);
  }  
  for (int d = SPEAKER_PWM_MAX_VOL; d > 0; d--){
    Serial.println(d);
    speaker_set_volume(d);
    delay(20);
  }
}
