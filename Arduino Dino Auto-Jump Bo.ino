/*
  ======================================================================
   🦖 Arduino Dino Auto-Jump Bot (LDR + Servo)
  ======================================================================

   📖 Project Description:
   This project uses an Arduino Uno, an LDR sensor module, and a servo 
   motor to play the Chrome Dino game automatically. The LDR detects 
   obstacles (cactus/bird) on the screen, and the servo presses the 
   spacebar key to make the Dino jump.

   🎥 YouTube Demo & Tutorial:
   https://youtu.be/MWaC0_pVWJk?si=Y6doYtkcxfjNwg0X

   📌 Author:
   Wired Wanderer (@TheWiredWander)

   📜 License:
   © 2025 Wired Wanderer. All rights reserved.
   Educational and personal hobby use only.
   Commercial use or redistribution without permission is prohibited.

   ⚙️ Hardware Required:
   - Arduino Uno (or compatible)
   - SG90 Servo motor (or similar micro servo)
   - LDR module (with digital output D0)
   - Breadboard + jumper wires
   - USB cable

   🔌 Pin Connections:
   | Component     | Arduino Pin |
   |---------------|-------------|
   | LDR D0        | D2          |
   | Servo Signal  | D9          |
   | Servo VCC     | 5V          |
   | Servo GND     | GND         |
   | LDR VCC       | 5V          |
   | LDR GND       | GND         |

   🛠️ Setup:
   1. Place LDR facing your monitor where obstacles appear.
   2. Mount the servo over the spacebar.
   3. Upload this code to Arduino Uno.
   4. Adjust PRESS_ANGLE & REST_ANGLE to fit your keyboard.
   5. Fine-tune LDR sensitivity with onboard potentiometer.

   ======================================================================
*/


#include <Servo.h>

Servo jumpServo;

// === Pins ===
const int LDR_D0     = 2;   // D0 from LDR module
const int SERVO_PIN  = 9;   // Servo signal

// === Sensor logic ===
// Set this after the test sketch:
//   true  -> D0 goes LOW when obstacle/dark detected (most boards)
//   false -> D0 goes HIGH when obstacle/dark detected
const bool SENSOR_ACTIVE_LOW = false;

// === Servo angles (tune for your keyboard) ===
const int PRESS_ANGLE = 20;   // angle that taps spacebar
const int REST_ANGLE  = 0;    // resting angle

// === Timing ===
unsigned long lastJumpTime = 0;
const unsigned long jumpCooldown   = 250;  // ms between allowed jumps
const unsigned long doubleJumpGap  = 100;  // ms to check for a 2nd obstacle

void setup() {
  pinMode(LDR_D0, INPUT);
  jumpServo.attach(SERVO_PIN);
  jumpServo.write(REST_ANGLE);
  Serial.begin(9600);
  delay(300);
}

void loop() {
  bool detected = readObstacle();

  // Debug: see 0/1 and interpreted state
  Serial.print("D0=");
  Serial.print(digitalRead(LDR_D0));
  Serial.print("  detected=");
  Serial.println(detected);

  if (detected && millis() - lastJumpTime > jumpCooldown) {
    doJump();                    // jump (with optional double jump inside)
    lastJumpTime = millis();
  }
}

// Read sensor with correct polarity
bool readObstacle() {
  int raw = digitalRead(LDR_D0);
  return SENSOR_ACTIVE_LOW ? (raw == LOW) : (raw == HIGH);
}

// One tap
void pressSpacebar() {
  jumpServo.write(PRESS_ANGLE);
  delay(120);                   // hold briefly
  jumpServo.write(REST_ANGLE);
}

// Jump + quick check for a second obstacle
void doJump() {
  pressSpacebar();              // first jump

  delay(doubleJumpGap);         // brief gap; Dino is still airborne
  if (readObstacle()) {         // still dark/obstacle -> second jump
    pressSpacebar();
  }
}
