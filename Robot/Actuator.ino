#include "Actuator.h"

void Actuator::Actuator::extend() {
  extend(255);
}

void Actuator::Actuator::extend(uint8_t pwm) {
  digitalWrite(in1Pin, HIGH);
  digitalWrite(in2Pin, LOW);
  analogWrite(enaPin, pwm);
}

void Actuator::retract() {
  retract(255);
}

void Actuator::retract(uint8_t pwm) {
  digitalWrite(in1Pin, LOW);
  digitalWrite(in2Pin, HIGH);
  analogWrite(enaPin, pwm);
}

void Actuator::off() { // leave motor floating
  digitalWrite(in1Pin, HIGH);
  digitalWrite(in2Pin, HIGH);
  digitalWrite(enaPin, LOW);
}

void Actuator::brake() {
  digitalWrite(in1Pin, LOW);
  digitalWrite(in2Pin, LOW);
  digitalWrite(enaPin, LOW);
}

void Actuator::readPosition() {
  total -= readings[index];                // subtract the last value

  //current position
  rawPosition = analogRead(feedbackPin);   // read the sensor
  readings[index] = rawPosition;           // record the reading
  total += readings[index];                // add the value

  filtPosition = total / SMOOTH;           // average reading and update the filtered position

  index++;
  if (index >= SMOOTH) {  // reset index if at end of array
    index = 0;
  }
}

Actuator::Actuator(byte attachToIn1Pin, byte attachToIn2Pin, byte attachToEnaPin, byte attachToFeedbackPin) :
  in1Pin(attachToIn1Pin),
  in2Pin(attachToIn2Pin),
  enaPin(attachToEnaPin),
  feedbackPin(attachToFeedbackPin) {
     
  isCalibrated = false;
  calibrationStage = 0; 
  isReady = false;
  isSet = false; 
  
  memset(readings, 0, sizeof(readings));
  total = 0;
  index = 0;
  
  currPos = 0;
  isExtending = true;
  pwm = 255;
  readPosition();
}

void Actuator::setup() {
  pinMode(in1Pin, OUTPUT);
  pinMode(in2Pin, OUTPUT);
  pinMode(enaPin, OUTPUT);
  pinMode(feedbackPin, INPUT);
  off();
}

void Actuator::setInitialHeight() { 
  targetPosition = minPosition + (maxPosition - minPosition) * RELATIVE_START_HEIGHT;
  readPosition(); 
  // Serial.print("minPosition:"); Serial.println(minPosition);
  // Serial.print("maxPosition:"); Serial.println(maxPosition);
  // Serial.print("targetPosition:"); Serial.println(targetPosition);
  // Serial.print("filtPosition:"); Serial.println(filtPosition);

  if (!isSet) {
    if (filtPosition <= targetPosition + TOLERANCE && filtPosition >= targetPosition - TOLERANCE) { 
      brake();
      isSet = true;
    } else if (filtPosition > targetPosition + TOLERANCE) { 
      extend();
    } else if (filtPosition < targetPosition - TOLERANCE) { 
      retract();
    }
  } else { 
      brake();
  }
}

void Actuator::loop() {
  t_now = millis();
  
  if (t_now >= t_end) {
    Serial.println("actuator hit end of loop");
    isReady = true; 
    isExtending = true;
  }
  else if (t_now > (t_start + movement[currPos]) && currPos < NUM_MAX_SWITCHES) {
    Serial.println("switch");
    isExtending = !isExtending;
    currPos++;
  }

  setPWM();

  isExtending ? extend(pwm) : retract(pwm);
}

/*
 * 0. get current time
 * 1. figure out where we are in position in relation to time
 * 2 . caclualte constatnts
 * 3. calculate speed
 * 4. set speed
*/
void Actuator::setPWM() {

   //1. figure out where we are in position in relation to time
  unsigned long start_time;
  unsigned long end_time;
  
  if (currPos > NUM_MAX_SWITCHES) {
    // should never hit here 
    pwm = 200;
    Serial.print("ERROR: Reached the null case for set speed");
    return;
  } else if (currPos == NUM_MAX_SWITCHES) {
    start_time = t_start + movement[currPos - 1];
    end_time = t_end;
  } else if (currPos == 0) {
    start_time = t_start;
    end_time = t_start + movement[currPos];
  } else {
    start_time = t_start + movement[currPos - 1];
    end_time = t_start + movement[currPos];
  }

  //2. calculate constants
  //what we have: vertex and start/end points, current time, range of y (15-245)
  float h = (float(start_time) + float(end_time)) / 2;
  float k = 245;

  // a = (y - k) / (x - h)^2
  // y = y, x = x, h = x vertex, k = y vertex
  // point = (start_time, 15)
  float a = (15 - k) / (sq(start_time - h));

  //3. calculate speed
  //y = a(x - h)^2 + k
  pwm = a*(sq(t_now - h)) + k;
}

void Actuator::calibrate() {
  switch (calibrationStage) {
    case 0: 
    case 7: extend();
            break;

    case 3: extend(255 * CALIB_SPEED_RATIO);
            break;

    case 2:
    case 5: retract();
            break;
    
    case 8: retract(255 * CALIB_SPEED_RATIO);
            break;
            
    case 1:
    case 4:
    case 6:
    case 9: for(int i = 0; i < SMOOTH; i++)
              readPosition();
            brake();
            break;

    default:break;
  }

  switch (calibrationStage) {
    case 1: minPosition = filtPosition;
            break;

    case 4: minPosition = max(minPosition, filtPosition);
            break;

    case 6: maxPosition = filtPosition;
            break;

    case 9: maxPosition = min(maxPosition, filtPosition);
            break;

    default: break;
  }
  calibrationStage++;
  if(calibrationStage > NUM_CALIB_STAGES) {
    calibrationStage = 0;
    isCalibrated = true;
    isReady = true;
    
  }
  
}

void Actuator::calibrate(uint16_t (&settings)[2]) {
  maxPosition = max(settings[0], settings[1]);
  minPosition = min(settings[0], settings[1]);
  isCalibrated = true;
  isReady = true;
}

void Actuator::setMovements(int movements[]){
  for (int i = 0; i < NUM_MAX_SWITCHES; i++){
    movement[i] = ((unsigned long)movements[i]) * 1000;
    Serial.print(movement[i]); Serial.print(", ");
  }
  Serial.println();
}

void Actuator::restartLoop(unsigned long t_startPlat, unsigned long t_endPlat){
  if (isCalibrated && isReady){
    isReady = false;
    currPos = 0; 
    t_start = t_startPlat; 
    t_end = t_endPlat; 
  }
}

bool Actuator::isActuatorReady() {
  return isReady;
}

bool Actuator::isActuatorSet() { 
  return isSet; 
}

int Actuator::getRawPosition() {
  return rawPosition;
}

int Actuator::getPosition() {
  return filtPosition;
}

int Actuator::getMaxPosition() {
  return maxPosition;
}

int Actuator::getMinPosition() {
  return minPosition;
}
