#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <MQ2.h>
#include <LiquidCrystal_I2C.h>

#define DHTPIN 33
#define DHTTYPE DHT11
#define BUZZER_PIN 22
#define RESET_BUTTON_PIN 17
#define RELAY_PIN 16
#define MQ_PIN 39
#define GAS_THRESHOLD 200
#define RESET_THRESHOLD 500  // 設定一個數值，表示重置按鈕的閾值

DHT dht(DHTPIN, DHTTYPE);
MQ2 mq2(MQ_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2);

bool alarmActive = false;  // 警報是否啟動的標誌

void setup() {
  Serial.begin(115200);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RESET_BUTTON_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);

  digitalWrite(RELAY_PIN, HIGH);
  delay(1000);
  digitalWrite(RELAY_PIN, LOW);
  delay(1000);

  delay(500);

  mq2.begin();
  dht.begin();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Hello, World!");
  lcd.setCursor(0, 1);
  lcd.print("made by YUXUN");
  delay(2000);
}
void loop() {
  float temperatureValue = dht.readTemperature();
  int gasValue = mq2.readCO();

  Serial.print("Temperature: ");
  Serial.print(temperatureValue);
  Serial.println(" °C\t");

  Serial.print("Gas Concentration: ");
  Serial.print(gasValue);
  Serial.println(" ppm");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperatureValue, 1);
  lcd.print("C");

  lcd.setCursor(0, 1);
  lcd.print("Gas:  ");
  lcd.print(gasValue);
  lcd.print(" ppm");

  // 檢查瓦斯濃度和溫度
  if (gasValue > GAS_THRESHOLD || temperatureValue > 29) {
    // 啟動蜂鳴器
    if (!alarmActive) {
      tone(BUZZER_PIN, 500);
      alarmActive = true;

      // 在更新LCD之前清除顯示
      lcd.clear();

      lcd.setCursor(0, 0);
      lcd.print("Temp: ");
      lcd.print(temperatureValue, 1);
      lcd.print("C");

      lcd.setCursor(0, 1);
      lcd.print("Gas:  ");
      lcd.print(gasValue);
      lcd.print(" ppm");
    }

    // 啟動繼電器
    digitalWrite(RELAY_PIN, HIGH);
  } else {
    // 關閉蜂鳴器
    noTone(BUZZER_PIN);
    alarmActive = false;

    // 如果重置按鈕被按下，且數值回到正常值，則重置繼電器
    if (digitalRead(RESET_BUTTON_PIN) == LOW && gasValue < RESET_THRESHOLD && temperatureValue < RESET_THRESHOLD) {
      digitalWrite(RELAY_PIN, LOW);

      // 在更新LCD之前清除顯示
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Reset Success");

      delay(1000);

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Temp: ");
      lcd.print(temperatureValue, 1);
      lcd.print("C");

      lcd.setCursor(0, 1);
      lcd.print("Gas:  ");
      lcd.print(gasValue);
      lcd.print(" ppm");
    }
  }
  Serial.println(analogRead(39));
  delay(1000);
}
