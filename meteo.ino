#include <Wire.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>
#include <LiquidCrystal_I2C.h>

#define MQ2pin A3
#define BUTTON_PIN 12

// LCD settings
#define LCD_ADDR 0x27
#define LCD_COLS 16
#define LCD_ROWS 2

LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);
Adafruit_BMP280 bmp;

short menu = 0;

float sensorValue;
float lastPressure = 0;
unsigned long lastReadTime = 0;
String prediction = "Unknown";

void setup() {
  Serial.begin(9600);
  Wire.begin();

  if (!bmp.begin(0x76)) {
    Serial.println("BMP280 not found!");
    while (1);
  }

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("Weather");
  lcd.setCursor(0, 1);
  lcd.print("Forecast");

  pinMode(BUTTON_PIN, INPUT);

  delay(1500);
  lcd.clear();
}

void loop() {
  static bool lastButton = HIGH;

  int temperature = bmp.readTemperature();
  int pressure = bmp.readPressure() / 100.0 * 0.75; // mmHg
  sensorValue = analogRead(MQ2pin);

  bool button = digitalRead(BUTTON_PIN);


  if (millis() - lastReadTime > 30000) {
    updateWeatherForecast(pressure);
    lastReadTime = millis();
  }


  if (lastButton == HIGH && button == LOW) {
    menu++;
    if (menu > 4) menu = 0;
    delay(200); // debounce
  }
  lastButton = button;

  lcd.clear();

  switch (menu) {
    case 0:
      lcd.setCursor(0, 0);
      lcd.print("Temp:");
      lcd.setCursor(0, 1);
      lcd.print(temperature);
      lcd.print(" C");
      break;

    case 1:
      lcd.setCursor(0, 0);
      lcd.print("Pressure:");
      lcd.setCursor(0, 1);
      lcd.print(pressure);
      lcd.print(" mmHg");
      break;

    case 2:
      lcd.setCursor(0, 0);
      lcd.print("CO2 Level:");
      lcd.setCursor(0, 1);
      lcd.print(sensorValue);
      break;

    case 3:
      lcd.setCursor(0, 0);
      lcd.print("Forecast:");
      lcd.setCursor(0, 1);
      lcd.print(prediction);
      break;

    case 4:
      lcd.setCursor(0, 0);
      lcd.print("ROBBO CLUB");
      lcd.setCursor(0, 1);
      lcd.print("Arduino Project");
      break;
  }

  delay(200);
}

void updateWeatherForecast(float pressure) {
  if (lastPressure == 0) {
    lastPressure = pressure;
    return;
  }

  float diff = pressure - lastPressure;

  if (diff < -1.5) {
    prediction = "Rainy";
  } 
  else if (diff > 1.5) {
    prediction = "Sunny";
  } 
  else {
    prediction = "Stable";
  }

  lastPressure = pressure;
}
