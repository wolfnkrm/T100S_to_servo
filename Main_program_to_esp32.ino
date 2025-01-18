#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <Keypad_I2C.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>

// PCA9685 and servo configuration
#define SERVOS_PER_BOARD 16
#define SERVOMIN 150
#define SERVOMAX 600
#define I2C_ADDR_KEYPAD 0x20
#define I2C_ADDR_LCD 0x27

int PCA9685_COUNT = 0;  // Number of PCA9685 boards
int NUM_SERVOS = 0;     // Total number of connected servos

Adafruit_PWMServoDriver pwm[10];  // Supports up to 10 PCA9685 boards
LiquidCrystal_I2C lcd(I2C_ADDR_LCD, 16, 2);
Keypad_I2C keypad(makeKeymap(keys), I2C_ADDR_KEYPAD, 4, 4);

// Keypad configuration
char keys[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};
unsigned long lastKeyPressTime = 0;
const unsigned long debounceDelay = 200;  // Debounce delay in milliseconds

// SPI configuration
#define DATA_PIN 19
#define CLOCK_PIN 18
#define NUM_LEDS 32
uint8_t ledData[NUM_LEDS * 3];  // Buffer to store RGB data for LEDs

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();

  // Display startup message
  lcd.setCursor(0, 0);
  lcd.print("Servo Controller");
  delay(2000);
  lcd.clear();

  // Prompt user for PCA9685 and servo counts
  PCA9685_COUNT = getUserInput("Enter PCA Count:");
  NUM_SERVOS = getUserInput("Enter Servos:");

  // Initialize PCA9685 boards
  setupPCA9685();

  lcd.clear();
  lcd.print("Setup Complete!");
  delay(2000);

  // Initialize SPI
  SPI.begin(DATA_PIN, CLOCK_PIN, SCK);
  SPI.setFrequency(2000000);  // Adjust frequency for T1000S
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);
}

void loop() {
  // Read SPI data from T1000S
  readT1000SData();

  // Map LED data to servo angles
  for (int i = 0; i < NUM_SERVOS; i++) {
    uint8_t red = ledData[i * 3];
    uint8_t green = ledData[i * 3 + 1];
    uint8_t blue = ledData[i * 3 + 2];

    // Map RGB to servo angle
    float hue = RGBtoHue(red, green, blue);
    int servoAngle = map(hue, 0, 360, 0, 180);
    int pulseLength = map(servoAngle, 0, 180, SERVOMIN, SERVOMAX);

    // Determine PCA9685 board and servo channel
    int boardIndex = i / SERVOS_PER_BOARD;
    int servoIndex = i % SERVOS_PER_BOARD;

    if (boardIndex < PCA9685_COUNT) {
      pwm[boardIndex].setPWM(servoIndex, 0, pulseLength);
    }
  }

  delay(50);  // Adjust refresh rate
}

// Function to initialize PCA9685 boards
void setupPCA9685() {
  for (int i = 0; i < PCA9685_COUNT; i++) {
    uint8_t address = 0x40 + i;
    pwm[i] = Adafruit_PWMServoDriver(address);
    pwm[i].begin();
    pwm[i].setPWMFreq(60);  // Set PWM frequency to 60Hz
    lcd.clear();
    lcd.print("Init PCA #");
    lcd.print(i + 1);
    delay(1000);
  }
}

// Function to read user input from keypad with debounce
int getUserInput(String prompt) {
  lcd.clear();
  lcd.print(prompt);
  lcd.setCursor(0, 1);

  String input = "";
  while (true) {
    char key = keypad.getKey();
    unsigned long currentTime = millis();

    if (key && (currentTime - lastKeyPressTime > debounceDelay)) {
      lastKeyPressTime = currentTime;

      if (key >= '0' && key <= '9') {
        input += key;
        lcd.print(key);
      } else if (key == '*') {
        if (input.length() > 0) {
          input.remove(input.length() - 1);
          lcd.setCursor(input.length(), 1);
          lcd.print(' ');
          lcd.setCursor(input.length(), 1);
        }
      } else if (key == '#') {
        if (input.length() > 0) {
          lcd.clear();
          lcd.print("Input Confirmed!");
          delay(1000);
          return input.toInt();
        } else {
          lcd.clear();
          lcd.print("Invalid Input!");
          delay(1000);
          lcd.clear();
          lcd.print(prompt);
          lcd.setCursor(0, 1);
        }
      }
    }
  }
}

// Function to read SPI data from T1000S
void readT1000SData() {
  for (int i = 0; i < NUM_LEDS * 3; i++) {
    ledData[i] = SPI.transfer(0);  // Read one byte from SPI
  }
}

// Function to convert RGB to Hue
float RGBtoHue(uint8_t r, uint8_t g, uint8_t b) {
  float red = r / 255.0, green = g / 255.0, blue = b / 255.0;
  float maxVal = max(red, max(green, blue)), minVal = min(red, min(green, blue));
  float delta = maxVal - minVal, hue = 0.0;

  if (delta == 0) hue = 0;
  else if (maxVal == red) hue = 60 * fmod(((green - blue) / delta), 6);
  else if (maxVal == green) hue = 60 * (((blue - red) / delta) + 2);
  else if (maxVal == blue) hue = 60 * (((red - green) / delta) + 4);

  if (hue < 0) hue += 360;
  return hue;
}
