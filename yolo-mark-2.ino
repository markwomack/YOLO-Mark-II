/**
 * Code written by Mark Womack
 * Distributed under the Apache License 2.0, a copy of which should accompany
 * this file.
 * 
 * Code for the Yolo Mark II robot.
 */
 
#define ENCODER_OPTIMIZE_INTERRUPTS
#include <Encoder.h>
#include <i2c_t3.h>
#include <ButtonExecutor.h>

// Teensy pin definitions
const int BLINK_PIN(13);
const int PUSH_BUTTON_PIN(33);
const int ENCODER_L_PIN_1(31);
const int ENCODER_L_PIN_2(32);
const int ENCODER_R_PIN_1(35);
const int ENCODER_R_PIN_2(34);

// Motor encoders
Encoder encoder_L(ENCODER_L_PIN_1, ENCODER_L_PIN_2);
Encoder encoder_R(ENCODER_R_PIN_1, ENCODER_R_PIN_2);

ButtonExecutor buttonExecutor(&Serial);

void readEncoders(void);

void setup() {
  Serial.begin(9600);

  buttonExecutor.setup(PUSH_BUTTON_PIN, LOW, sketchSetup, sketchStart, sketchStop);
}

void loop() {
  buttonExecutor.loop();
}

// Called when the buttonExecutor is set up
void sketchSetup(void) {
  Serial.println("sketchSetup called!");
}

// Called when the buttonExecutor is started with button push
void sketchStart(void) {
  Serial.println("sketchStart called!");

  encoder_L.write(0);
  encoder_R.write(0);

  buttonExecutor.callbackEveryByMillis(500, readEncoders);
}

// Called when buttonExecutor stopped with button push
void sketchStop(void) {
  Serial.println("sketchStop called!");
}

void readEncoders(void) {
  long new_encoder_L_pos = encoder_L.read();
  long new_encoder_R_pos = encoder_R.read();
  Serial.print("L: ");
  Serial.print(new_encoder_L_pos);
  Serial.print("  R: ");
  Serial.println(new_encoder_R_pos);
}
