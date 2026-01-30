#include <LiquidCrystal_I2C.h>

// -------------------- Pin Definitions --------------------
#define THROTTLE A0
#define REVERSE  3

#define BUZZER   2

#define PWM      9
#define PWM2     10

#define LED      4
#define LED2     5

#define LEFT     6
#define RIGHT    7

// -------------------- LCD --------------------
LiquidCrystal_I2C lcd(0x27, 20, 4);

// LCD update timing
unsigned long lastLcdUpdate = 0;
const unsigned long lcdUpdateInterval = 100;  // 100ms refresh rate


// -------------------- Reverse Beeper --------------------
unsigned long lastBeepTime = 0;
const unsigned long beepInterval = 1600; // time between beep cycles
bool beeping = false;
unsigned long beepStart = 0;
const unsigned long beepDuration = 800;  // how long buzzer stays ON

// -------------------- Smoothed Sensor Values --------------------
const float smoothFactor = 0.10;  // smoothing factor for readings


// -------------------- Utility --------------------
float smooth(float previous, float current) {
  return previous + smoothFactor * (current - previous);
}
// -------------------- Functions --------------------
void update_lcd(bool reverseActive, int driveThrottle) {
  // Throttle percent input
  int throttlePercent = map(smoothedThrottle, 0, 1023, 0, 100);

  // Top-right FWD / REV
  lcd.setCursor(17, 0);
  if (reverseActive)
    lcd.print("REV");
  else
    lcd.print("FWD");

  // ------------------
  // Row 1: Throttle
  // ------------------
  lcd.setCursor(0, 1);
  lcd.print("THR:");
  if (reverseActive) {
    lcd.print("SLOW AF");
  } else {
    lcd.print(throttlePercent);
  }
  
  lcd.print("%   ");
}

void handleReverseBeep(bool reverseActive) {
  unsigned long now = millis();

  // Start a new beep cycle
  if (reverseActive && !beeping && (now - lastBeepTime >= beepInterval)) {
    beeping = true;
    beepStart = now;
    lastBeepTime = now;
    digitalWrite(BUZZER, HIGH);
  }

  // Turn off buzzer after beepDuration
  if (beeping && (now - beepStart >= beepDuration)) {
    digitalWrite(BUZZER, LOW);
    beeping = false;
  }

  // If reverse disabled, ensure buzzer is off
  if (!reverseActive) {
    beeping = false;
    digitalWrite(BUZZER, LOW);
  }
}


// --------------- Startup Music -----------------

#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_E4  330
#define NOTE_FS4 370
#define NOTE_GS4 415

#define NOTE_A4  440
#define NOTE_B4  494
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_E5  659

void play_startup_song() {
  int melody[] = {
    NOTE_E5, NOTE_D5, NOTE_FS4, NOTE_GS4, 
    NOTE_CS5, NOTE_B4, NOTE_D4, NOTE_E4, 
    NOTE_B4, NOTE_A4, NOTE_CS4, NOTE_E4,
    NOTE_A4
  };

  int durations[] = {
    8, 8, 4, 4,
   8, 8, 4, 4,
    8, 8, 4, 4,
    2
  };

  int size = sizeof(durations) / sizeof(int);

  for (int note = 0; note < size; note++) {
    //to calculate the note duration, take one second divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int duration = 1000 / durations[note];
    tone(BUZZER, melody[note], duration);

    //to distinguish the notes, set a minimum time between them.
    //the note's duration + 30% seems to work well:
    int pauseBetweenNotes = duration * 1.30;
    delay(pauseBetweenNotes);
    
    //stop the tone playing:
    noTone(BUZZER);
  }

  delay(1000);
  digitalWrite(BUZZER, HIGH);
  delay(70);
  digitalWrite(BUZZER, LOW);
  delay(70);
  digitalWrite(BUZZER, HIGH);
  delay(70);
  digitalWrite(BUZZER, LOW);
  delay(70);
}

// -------------------- Setup --------------------
void setup() {
  lcd.init();
  lcd.backlight();
  lcd.print("TTPs HEART KILOBYTE");
  //play_startup_song();
  lcd.clear();

  Serial.begin(9600);

  pinMode(THROTTLE, INPUT);
  pinMode(REVERSE, INPUT);
  pinMode(LEFT, INPUT);
  pinMode(RIGHT, INPUT);
  pinMode(LED, OUTPUT);
  pinMode(LED2, OUTPUT);
  digitalWrite(LED2, HIGH);
  digitalWrite(LED, HIGH);
  pinMode(PWM, OUTPUT);
  pinMode(PWM2, OUTPUT);

  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);
  Wire.begin();
}

void sendPacket(byte throttle, byte leftBtn, byte rightBtn) {
  Wire.beginTransmission(8);   // slave address
  Wire.write(throttle);
  Wire.write(leftBtn);
  Wire.write(rightBtn);
  Wire.endTransmission();
}

// -------------------- Main Loop --------------------
void loop() {
  unsigned long now = millis();

  // Read raw values
  int throttleRaw = analogRead(THROTTLE);
  int leftPressed = digitalRead(LEFT);
  int rightPressed = digitalRead(RIGHT);
  int reverseActive = digitalRead(REVERSE);

  // Smooth sensor values
  smoothedThrottle = smooth(smoothedThrottle, throttleRaw);
  // Normal throttle mapping
  int driveThrottle = 255 - map(smoothedThrottle, 0, 1023, 0, 180);

  // Reverse mode
  if (reverseActive) {
    driveThrottle = (254);  // slow reverse speed
  }

  sendPacket(driveThrottle, leftPressed, rightPressed);
  // Reverse beep logic
  handleReverseBeep(reverseActive);

  // LCD update every lcdUpdateInterval
  if (now - lastLcdUpdate >= lcdUpdateInterval) {
    update_lcd(reverseActive, driveThrottle);
    lastLcdUpdate = now;

    Serial.println(driveThrottle);
    Serial.println(leftPressed);
    Serial.println(rightPressed);
  }
}

