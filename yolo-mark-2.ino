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
#include <PololuQik.h>
#include <QTRSensors.h>
#include <ButtonExecutor.h>

// Teensy pin definitions
const int BLINK_PIN(13);
const int RX1(0);
const int TX1(1);
const int PUSH_BUTTON_PIN(33);
const int ENCODER_L_PIN_1(31);
const int ENCODER_L_PIN_2(32);
const int ENCODER_R_PIN_1(35);
const int ENCODER_R_PIN_2(34);
const int EDGE_LF(2);
const int EDGE_RF(17);
const int EDGE_LR(30);
const int EDGE_RR(36);


// Motor encoders
Encoder encoder_L(ENCODER_L_PIN_1, ENCODER_L_PIN_2);
Encoder encoder_R(ENCODER_R_PIN_1, ENCODER_R_PIN_2);

// Motorcontroller
PololuQik2s9v1 qik(TX1, RX1, 3);

// Edge sensors
const int NUM_EDGE_SENSORS(4);
const int EDGE_TIMEOUT(500);   // waits for 500 microseconds for sensor outputs to go low

QTRSensorsRC edgeSensors((unsigned char[]) {EDGE_LF, EDGE_RF, EDGE_LR, EDGE_RR},
  NUM_EDGE_SENSORS, EDGE_TIMEOUT, QTR_NO_EMITTER_PIN); 
unsigned int edgeSensorValues[NUM_EDGE_SENSORS];

ButtonExecutor buttonExecutor(&Serial);

void readEncoders(void);
void readEdgeSensors(void);

const char* param_names[] = {
  "Device ID",
  "PWM Parameter",
  "Shut Down Motors on Error",
  "Serial Timeout"
};

int motorState = 0;

void setup() {
  Serial.begin(9600);

  qik.init();

/*
  for (int i = QIK_CONFIG_DEVICE_ID; i <= QIK_CONFIG_SERIAL_TIMEOUT; i++)
  {
    Serial.print(param_names[i]);
    Serial.print(": ");
    Serial.println(qik.getConfigurationParameter(i));
  }
*/
  pinMode(2, INPUT);
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
  motorState = 0;

  buttonExecutor.callbackEveryByMillis(10, readEncoders);
  buttonExecutor.callbackEveryByMillis(10, readEdgeSensors);
  buttonExecutor.callbackEveryByMillis(10, controlMotors);
}

// Called when buttonExecutor stopped with button push
void sketchStop(void) {
  Serial.println("sketchStop called!");
  qik.setSpeeds(0, 0);
}

void readEncoders(void) {
  long new_encoder_L_pos = encoder_L.read();
  long new_encoder_R_pos = encoder_R.read();

  /*
  Serial.print("L: ");
  Serial.print(new_encoder_L_pos);
  Serial.print("  R: ");
  Serial.println(new_encoder_R_pos);
  */
}

void readEdgeSensors(void) {
  // read raw sensor values
  edgeSensors.read(edgeSensorValues);

  /*
  // print the sensor values as numbers from 0 to 500, where 0 means maximum reflectance and
  // 500 means minimum reflectance
  for (unsigned char i = 0; i < NUM_EDGE_SENSORS; i++)
  {
    Serial.print(edgeSensorValues[i]);
    Serial.print('\t'); // tab to format the raw data into columns in the Serial monitor
  }
  Serial.println();
  */
}

void controlMotors(void) {
  long new_encoder_L_pos = encoder_L.read();
  long new_encoder_R_pos = encoder_R.read();
  Serial.print("motorState: ");
  Serial.println(motorState);
  switch(motorState) {
    case 0:
      encoder_L.write(0);
      encoder_R.write(0);
      qik.setSpeeds(100, 100);
      motorState = 1;
      break;
    case 1:
      if (new_encoder_L_pos > 2000 || new_encoder_R_pos > 2000) {
        qik.setSpeeds(-100, -100);
        motorState = 2;
      }
      break;
    case 2:
      if (new_encoder_L_pos < 0 || new_encoder_R_pos < 0) {
        qik.setSpeeds(100, 100);
        motorState = 1;
      }
      break;
  }
}

