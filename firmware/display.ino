#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "pitches.h"

// -------------------- Pin Definitions --------------------
#define THROTTLE A0
#define REVERSE 3
#define BUZZER 2
#define LED 4
#define LED2 5
#define LEFT 6
#define RIGHT 7

// -------------------- LCD --------------------
LiquidCrystal_I2C lcd(0x27, 20, 4);

unsigned long lastLcdUpdate = 0;
const unsigned long lcdUpdateInterval = 100;

// -------------------- Reverse Beeper --------------------
unsigned long lastBeepTime = 0;
const unsigned long beepInterval = 1600;
bool beeping = false;
unsigned long beepStart = 0;
const unsigned long beepDuration = 800;

// -------------------- Throttle Smoothing --------------------
const float smoothFactor = 0.10;
float smoothedThrottle = 0;

float smooth(float previous, float current) {
  return previous + smoothFactor * (current - previous);
}

// -------------------- LCD --------------------
byte block1[8] = { 16, 16, 16, 16, 16, 16, 16, 16 };  // 1/5 vertical line
byte block2[8] = { 24, 24, 24, 24, 24, 24, 24, 24 };  // 2/5 vertical lines
byte block3[8] = { 28, 28, 28, 28, 28, 28, 28, 28 };  // 3/5 vertical lines
byte block4[8] = { 30, 30, 30, 30, 30, 30, 30, 30 };  // 4/5 vertical lines
byte block5[8] = { 31, 31, 31, 31, 31, 31, 31, 31 };  // full vertical line


void drawThrottleBar(int throttlePercent, int totalBlocks) {
  int totalSubblocks = totalBlocks * 5;  // 5 levels per block

  int filledSubblocks = map(throttlePercent, 0, 100, 0, totalSubblocks);

  lcd.print("[");
  for (int i = 0; i < totalBlocks; i++) {
    int subblockLevel = filledSubblocks - i * 5;
    if (subblockLevel >= 5) subblockLevel = 5;
    else if (subblockLevel < 0) subblockLevel = 0;

    if (subblockLevel == 0) {
      lcd.print(" ");
    } else {
      lcd.write(byte(subblockLevel));
    }
  }
  lcd.print("]");
}

void update_lcd(bool reverseActive) {
  int throttlePercent = map(smoothedThrottle, 1023, 0, 0, 100);

  lcd.setCursor(0, 0);
  if (reverseActive) {
    lcd.print("<<< REV >>>      ");
  } else {
    lcd.print("FWD ");
    drawThrottleBar(throttlePercent, 10);  // high-res bar
    lcd.print(" ");                        // padding
  }

  lcd.print(throttlePercent);
  lcd.print("%");

  // placeholder, will be speedometer
  lcd.setCursor(0, 3);
  drawThrottleBar(throttlePercent, 18);


  // -------------------- Reverse Beep --------------------
void handleReverseBeep(bool reverseActive) {
  unsigned long now = millis();

  if (reverseActive && !beeping && (now - lastBeepTime >= beepInterval)) {
    beeping = true;
    beepStart = now;
    lastBeepTime = now;
    digitalWrite(BUZZER, HIGH);
  }
  if (beeping && (now - beepStart >= beepDuration)) {
    digitalWrite(BUZZER, LOW);
    beeping = false;
  }
  if (!reverseActive) {
    beeping = false;
    digitalWrite(BUZZER, LOW);
  }
}

void play_startup_song() {
  // Imperial March melody
  int melody[] = {
    NOTE_A4, NOTE_A4, NOTE_A4, NOTE_F4, NOTE_C5, NOTE_A4,
    NOTE_F4, NOTE_C5, NOTE_A4,
    NOTE_E5, NOTE_E5, NOTE_E5, NOTE_F5, NOTE_C5, NOTE_GS4,
    NOTE_F4, NOTE_C5, NOTE_A4,
    NOTE_A5, NOTE_A4, NOTE_A4, NOTE_A5, NOTE_GS5, NOTE_G5,
    NOTE_FS5, NOTE_F5, NOTE_FS5, NOTE_REST, NOTE_AS4, NOTE_DS5,
    NOTE_D5, NOTE_CS5, NOTE_C5, NOTE_B4, NOTE_C5, NOTE_REST
  };
  int durations[] = {
    4, 4, 4, 6, 12, 4,
    6, 12, 2,
    4, 4, 4, 6, 12, 4,
    6, 12, 2,
    4, 6, 16, 4, 6, 16,
    16, 16, 8, 8, 8, 8,
    16, 16, 16, 8, 8, 4
  };
  int size = sizeof(durations) / sizeof(int);
  for (int i = 0; i < sizeof(melody) / sizeof(melody[0]); i++) {
    int noteDuration = (1000 / durations[i]);
    tone(BUZZER, melody[i], noteDuration * 2);
    delay(noteDuration * 2.3);
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
  Wire.begin();

  lcd.init();
  lcd.createChar(0, block0);
  lcd.createChar(1, block1);
  lcd.createChar(2, block2);
  lcd.createChar(3, block3);
  lcd.createChar(4, block4);
  lcd.createChar(5, block5);

  pinMode(THROTTLE, INPUT);
  pinMode(REVERSE, INPUT);
  pinMode(LEFT, INPUT);
  pinMode(RIGHT, INPUT);

  pinMode(LED, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(PWM, OUTPUT);
  pinMode(PWM2, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  digitalWrite(LED, HIGH);
  digitalWrite(LED2, HIGH);
  digitalWrite(BUZZER, LOW);

  lcd.backlight();
  lcd.print("TTPs HEART KILOBYTE");
  //play_startup_song();
  lcd.clear();

  Serial.begin(9600);
}

  // -------------------- I2C Packet --------------------
  bool sendPacket(byte throttle, byte leftBtn, byte rightBtn) {
    Wire.beginTransmission(8);
    Wire.write(throttle);
    Wire.write(leftBtn);
    Wire.write(rightBtn);

    byte status = Wire.endTransmission();
    return (status == 0);
  }

  // -------------------- Main Loop --------------------
void loop() {
  unsigned long now = millis();

  smoothedThrottle = smooth(smoothedThrottle, analogRead(THROTTLE));
  bool reverseActive = !digitalRead(REVERSE);
  int leftPressed = digitalRead(LEFT);
  int rightPressed = digitalRead(RIGHT);

  // Throttle mapping
  //  180 = zero throttle
  //  255 = full
  //  0 = full rever
  int driveThrottle = map(smoothedThrottle, 1023, 0, 180, 255);

  if (reverseActive) {
    driveThrottle = 255 - driveThrottle;
  }

  // Send packet to control unit
  while (!sendPacket(driveThrottle, leftPressed, rightPressed)) {
    left = 0;
    right = 0;  // disable input
    lcd.clear();
    lcd.print("COMMUNICATION LOST");
    delay(1000);
  }

  handleReverseBeep(reverseActive);

  if (now - lastLcdUpdate >= lcdUpdateInterval) {
    update_lcd(reverseActive);
    lastLcdUpdate = now;
  }
}