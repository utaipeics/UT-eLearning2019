#include <Wire.h>  // Wire library - used for I2C communication

int ADXL345 = 0x53; // The ADXL345 sensor I2C address

static float prevX;
static float prevY;
static float prevZ;
static float x;
static float y;
static float z;

static bool goingUp;
static int currentTone;

void setup() {
  Serial.begin(9600); // Initiate serial communication for printing the results on the Serial monitor
  Wire.begin(); // Initiate the Wire library
  // Set ADXL345 in measuring mode
  Wire.beginTransmission(ADXL345); // Start communicating with the device 
  Wire.write(0x2D); // Access/ talk to POWER_CTL Register - 0x2D
  // Enable measurement
  Wire.write(8); // (8dec -> 0000 1000 binary) Bit D3 High for measuring enable 
  Wire.endTransmission();
  delay(10);

  // Initialize buzzer.
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);

  prevX = 0;
  prevY = 0;
  prevZ = 0;
  x = 0;
  y = 0;
  z = 0;
  currentTone = 440;
}

void playAlarmSound(int speakerPin) {
  Serial.println("playing sound");
  if (currentTone > 1000) {
    goingUp = false;
  } else if (currentTone < 440) {
    goingUp = true;
  }

  if (goingUp) {
    currentTone += 50;
  } else {
    currentTone -= 50;
  }
  
  tone(speakerPin, currentTone, 50);
}

void stopAlarmSound(int speakerPin) {
  Serial.println("stopping sound");
  noTone(speakerPin);
}


void loop() {
  // === Read acceleromter data === //
  Wire.beginTransmission(ADXL345);
  Wire.write(0x32); // Start with register 0x32 (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(ADXL345, 6, true); // Read 6 registers total, each axis value is stored in 2 registers

  prevX = x;
  prevY = y;
  prevZ = z;
  
  x = (Wire.read() | Wire.read() << 8);
  x /= 256;
  
  y = (Wire.read() | Wire.read() << 8);
  y /= 256;
  
  z = (Wire.read() | Wire.read() << 8);
  z /= 256;

  if (abs(x - prevX) >= .2f || abs(y - prevY) >= .2f || abs(z - prevZ) >= .2f) {
    digitalWrite(13, HIGH);
    playAlarmSound(12);
  } else {
    digitalWrite(13, LOW);
    stopAlarmSound(12);
  }
  
  //Serial.printf("x= %f  y=%f  z=%f\n", x, y, z);
}
