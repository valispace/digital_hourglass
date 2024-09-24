#include <Wire.h>
#include <LiquidCrystal.h>

// Define the MPU-6050 I2C address
const int MPU = 0x68;

// Define accelerometer variables
int16_t AcX, AcY, AcZ;
float roll;

// Initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// Button pin
const int buttonPin = 7;
const int DELAY = 1000;

// Create a custom character (full block)
byte fullBlock[8] = {
  B11111,  // Top row (all pixels on)
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,  // Bottom row (all pixels on)
};

void setup() {
  // Set up the LCD's number of columns and rows
  lcd.begin(16, 2);  
  
  // Initialize the I2C communication
  Wire.begin();

  // Start the serial communication at 9600 baud rate
  Serial.begin(9600);

  // Initialize the MPU-6050
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);  // Wake up the MPU-6050 by writing 0 to the PWR_MGMT_1 register
  Wire.write(0);
  Wire.endTransmission(true);

  // Create the full block character in location 0
  lcd.createChar(0, fullBlock);

  // Fill the screen with full blocks
  fillScreenWithBlocks();
}

void loop() {
  // Clear the screen based on the current direction
  if(isUpsideDown()){
    Serial.println("UPSIDE DOWN");
    clearFromRight();
    while (!isUpright()){
      delay(100);
    }
  }
  else if (isUpright()){
    Serial.println("UPSIDE UP");
    clearFromLeft();
    while (!isUpsideDown()){
      delay(100);
    }
  }
  
  // Reset the display after clearing all pixels
  fillScreenWithBlocks();  // Reinitialize the screen with full blocks
}

// Function to fill both rows with full blocks
void fillScreenWithBlocks() {
  lcd.clear();
  lcd.setCursor(0, 0);  // First row
  for (int i = 0; i < 16; i++) {
    lcd.write(byte(0));  // Print full block character
  }

  lcd.setCursor(0, 1);  // Second row
  for (int i = 0; i < 16; i++) {
    lcd.write(byte(0));  // Print full block character
  }

  delay(DELAY);  // Wait for 1 second before starting to delete pixels
}

// Function to clear blocks from left to right
void clearFromLeft() {
  for (int col = 0; col < 16; col++) {
    // Clear the block on the first row at column `col`
    lcd.setCursor(col, 0);
    lcd.print(" ");  // Replace the block with a space (empty pixel)
    delay(DELAY);     // Wait for 1 second

    // Clear the block on the second row at column `col`
    lcd.setCursor(col, 1);
    lcd.print(" ");  // Replace the block with a space (empty pixel)
    delay(DELAY);     // Wait for 1 second
  }
}

// Function to clear blocks from right to left
void clearFromRight() {
  for (int col = 15; col >= 0; col--) {
    // Clear the block on the first row at column `col`
    lcd.setCursor(col, 0);
    lcd.print(" ");  // Replace the block with a space (empty pixel)
    delay(DELAY);     // Wait for 1 second

    // Clear the block on the second row at column `col`
    lcd.setCursor(col, 1);
    lcd.print(" ");  // Replace the block with a space (empty pixel)
    delay(DELAY);     // Wait for 1 second
  }
}

bool isFlat(){
  getAcceleration(AcX, AcY, AcZ);
  return AcZ > 15000 && AcZ < 16300;

}

bool isUpright(){
  getRoll();
  return roll > -180 && roll < 20;
}


bool isUpsideDown(){
  getRoll();
  return roll > 80 && roll < 120;
}

void getRoll(){
  getAcceleration(AcX, AcY, AcZ);
  roll = atan2(-AcX, AcZ) * 180.0 / PI;
  Serial.println(roll);
}

// Function to read acceleration data from MPU-6050
void getAcceleration(int16_t &x, int16_t &y, int16_t &z) {
  // Request accelerometer data from the MPU-6050
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);  // Start reading from register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true);  // Request 6 bytes of accelerometer data

  // Read accelerometer values
  x = Wire.read() << 8 | Wire.read();  // Accelerometer X-axis
  y = Wire.read() << 8 | Wire.read();  // Accelerometer Y-axis
  z = Wire.read() << 8 | Wire.read();  // Accelerometer Z-axis
}

