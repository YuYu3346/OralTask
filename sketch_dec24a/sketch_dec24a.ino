#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <MQ2.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define DHTPIN 33
#define DHTTYPE DHT11
#define BUZZER_PIN 4
#define RESET_BUTTON_PIN 17
#define RELAY_PIN 16
#define MQ_PIN 39
#define GAS_THRESHOLD 200
#define RESET_THRESHOLD 500

const int STAGE1_DURATION = 1000;  // 1 秒
const int STAGE2_DURATION = 1000;
const int STAGE3_DURATION = 1000;
const int STAGE4_DURATION = 1000;
const int STAGE5_DURATION = 1000;
const int ALERT_INTERVAL = 1000;  //1sec

DHT dht(DHTPIN, DHTTYPE);
MQ2 mq2(MQ_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2);

bool alarmActive = false;
unsigned long lastAlertTime = 0;

const char *ssid = "Fall Guys";
const char *password = "yuxunkui";
const char *lineNotifyToken = "3NXvBIvHBqlcP0uTlGU2HFFt2BlDEzXFYUjGxc8fVpV";

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

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void sendLineNotify(String message, bool isResetMessage = false) {
  HTTPClient http;
  http.begin("https://notify-api.line.me/api/notify");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.addHeader("Authorization", "Bearer " + String(lineNotifyToken));

  String payload = "message=" + message;

  int httpResponseCode = http.POST(payload);
  if (httpResponseCode > 0) {
    Serial.print("Line Notify sent successfully. Response code: ");
    Serial.println(httpResponseCode);
  } else {
    Serial.print("Failed to send Line Notify. Error code: ");
    Serial.println(httpResponseCode);
  }

  http.end();

  // 如果是重置訊息，額外發送通知
  if (isResetMessage) {
    String resetMessage = "Reset Completed. Temp: " + String(dht.readTemperature(), 1) + "C, Gas: " + String(mq2.readCO()) + "ppm";
    sendLineNotify(resetMessage);
  }
}

void loop() {
  float temperatureValue = dht.readTemperature();
  int gasValue = mq2.readCO();
  unsigned long currentTime = millis();

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

  if (gasValue > GAS_THRESHOLD) {
    if (!alarmActive) {
      tone(BUZZER_PIN, 1000);
      delay(STAGE1_DURATION);
      noTone(BUZZER_PIN);
      delay(STAGE2_DURATION);
      tone(BUZZER_PIN, 1000);
      delay(STAGE3_DURATION);
      noTone(BUZZER_PIN);
      delay(STAGE4_DURATION);
      tone(BUZZER_PIN, 1000);
      delay(STAGE5_DURATION);
      noTone(BUZZER_PIN);

      alarmActive = true;

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Temp: ");
      lcd.print(temperatureValue, 1);
      lcd.print("C");

      lcd.setCursor(0, 1);
      lcd.print("Gas:  ");
      lcd.print(gasValue);
      lcd.print(" ppm");

      // Send Line Notify for Gas Alarm only every ALERT_INTERVAL milliseconds
      if (currentTime - lastAlertTime >= ALERT_INTERVAL) {
        String message = "Gas Alarm! Gas: " + String(gasValue) + "ppm";
        sendLineNotify(message);
        lastAlertTime = currentTime;
      }
    }
    digitalWrite(RELAY_PIN, HIGH);
  } else if (temperatureValue > 29) {
    if (!alarmActive) {
      tone(BUZZER_PIN, 1000);
      delay(STAGE1_DURATION);
      noTone(BUZZER_PIN);
      delay(STAGE2_DURATION);
      tone(BUZZER_PIN, 1000);
      delay(STAGE3_DURATION);
      noTone(BUZZER_PIN);
      delay(STAGE4_DURATION);
      tone(BUZZER_PIN, 1000);
      delay(STAGE5_DURATION);
      noTone(BUZZER_PIN);

      alarmActive = true;

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Temp: ");
      lcd.print(temperatureValue, 1);
      lcd.print("C");

      lcd.setCursor(0, 1);
      lcd.print("Gas:  ");
      lcd.print(gasValue);
      lcd.print(" ppm");

      // Send Line Notify for Temperature Alarm only every ALERT_INTERVAL milliseconds
      if (currentTime - lastAlertTime >= ALERT_INTERVAL) {
        String message = "Temperature Alarm! Temp: " + String(temperatureValue, 1) + "C";
        sendLineNotify(message);
        lastAlertTime = currentTime;
      }
    }
    digitalWrite(RELAY_PIN, HIGH);
  } else {
    noTone(BUZZER_PIN);
    if (digitalRead(RESET_BUTTON_PIN) == LOW && gasValue < RESET_THRESHOLD && temperatureValue < RESET_THRESHOLD) {
      digitalWrite(RELAY_PIN, LOW);

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

      // Send Line Notify for Reset Completed only every ALERT_INTERVAL milliseconds
      if (currentTime - lastAlertTime >= ALERT_INTERVAL) {
        String resetCompletedMessage = "Reset Completed. Temp: " + String(temperatureValue, 1) + "C, Gas: " + String(gasValue) + "ppm";
        sendLineNotify(resetCompletedMessage, true);
        lastAlertTime = currentTime;
      }
    }

    alarmActive = false;
  }

  delay(1000);
}
