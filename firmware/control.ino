#include <Wire.h>

// -------------------- Pin Definitions --------------------
#define PWM   9
#define PWM2  10

// -------------------- I2C Variables --------------------
byte receivedThrottle = 180; 
byte receivedLeft = 1;   // default "not pressed"
byte receivedRight = 1;  // default "not pressed"

unsigned long lastPacketTime = 0;
const unsigned long PACKET_TIMEOUT = 200;  // ms until FAILSAFE activates

// -------------------- Throttle --------------------
float leftThrottle  = 180;      // left motor
float rightThrottle = 180;      // right motor
const float accelStep = 4.0;     // how much each button press adds
const float decayStep = 1.5;     // how fast it returns to neutral
const float minThrottle = 180;

// -------------------- I2C Receive ----------------------
void receiveEvent(int numBytes) {
  if (numBytes >= 3) {
    receivedThrottle = Wire.read();
    receivedLeft = Wire.read();
    receivedRight = Wire.read();

    lastPacketTime = millis();   // mark last good packet
  }
}


// -------------------- Accelerate With Button Press --------------------
void updateThrottle() {
  // Left motor
  if (receivedLeft == 0) {             // button pressed
    leftThrottle += accelStep;
    if (leftThrottle > receivedThrottle) leftThrottle = receivedThrottle;
  } else {                             // button not pressed
    if (leftThrottle > minThrottle) {
      leftThrottle -= decayStep;
      if (leftThrottle < minThrottle) leftThrottle = minThrottle;
    }
  }

  // Right motor
  if (receivedRight == 0) {
    rightThrottle += accelStep;
    if (rightThrottle > receivedThrottle) rightThrottle = receivedThrottle;
  } else {
    if (rightThrottle > minThrottle) {
      rightThrottle -= decayStep;
      if (rightThrottle < minThrottle) rightThrottle = minThrottle;
    }
  }
}

// -------------------- Setup --------------------
void setup() {
  Serial.begin(9600);
  Wire.begin(8);  
  Wire.onReceive(receiveEvent);

  pinMode(PWM, OUTPUT);
  pinMode(PWM2, OUTPUT);
}

// -------------------- Main Loop --------------------
void loop() {
  unsigned long now = millis();

  // FAILSAFE: no packet recently?
  if (now - lastPacketTime > PACKET_TIMEOUT) {
    receivedLeft = 1;
    receivedRight = 1;
    leftThrottle = 180;
    rightThrottle = 180;
    Serial.println("Communication error");
  }

  // Update throttle based on buttons
  updateThrottle();

  // Apply throttle to both motors unless one button disables it
  analogWrite(PWM,  (receivedRight == 0 ? receivedThrottle : 180));
  analogWrite(PWM2, (receivedLeft  == 0 ? receivedThrottle : 180));


  Serial.print("Left: ");
  Serial.print(receivedThrottle);
  Serial.print(receivedLeft);
  Serial.print("  Right: ");
  Serial.println(receivedThrottle);
  Serial.print(receivedRight);

  delay(10);
}

