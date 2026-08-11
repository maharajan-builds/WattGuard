#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define VOLTAGE_PIN 34
#define CURRENT_PIN 35

#define LED_PIN 2
#define BUZZER_PIN 4

// Example tariff for simulation
#define TARIFF_PER_KWH 8.0

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Energy tracking
float energyWh = 0.0;
float peakPower = 0.0;

unsigned long previousTime = 0;

void setup() {

  Serial.begin(115200);

  // I2C
  Wire.begin(21, 22);

  // Output pins
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(LED_PIN, LOW);
  noTone(BUZZER_PIN);

  // OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED not found!");
    while (true);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  previousTime = millis();
}

void loop() {

  // -------------------------
  // READ INPUTS
  // -------------------------

  int voltageRaw = analogRead(VOLTAGE_PIN);
  int currentRaw = analogRead(CURRENT_PIN);

  // Convert potentiometer values
  float voltage =
      180.0 + (voltageRaw / 4095.0) * 70.0;

  float current =
      (currentRaw / 4095.0) * 10.0;

  // -------------------------
  // POWER
  // -------------------------

  float power = voltage * current;

  // -------------------------
  // PEAK POWER
  // -------------------------

  if (power > peakPower) {
    peakPower = power;
  }

  // -------------------------
  // ENERGY
  // -------------------------

  unsigned long currentTime = millis();

  // 1 real second = 1 simulated minute
  float simulatedSeconds = 60.0;

  float elapsedHours =
      ((currentTime - previousTime) / 1000.0)
      * simulatedSeconds / 3600.0;

  energyWh += power * elapsedHours;

  previousTime = currentTime;

  float energyKWh = energyWh / 1000.0;

  // -------------------------
  // COST
  // -------------------------

  float cost = energyKWh * TARIFF_PER_KWH;

  // -------------------------
  // STATUS
  // -------------------------

  String status;

  if (power < 500) {

    status = "NORMAL";

    digitalWrite(LED_PIN, LOW);
    noTone(BUZZER_PIN);
  }

  else if (power <= 1000) {

    status = "HIGH";

    digitalWrite(LED_PIN, HIGH);
    noTone(BUZZER_PIN);
  }

  else {

    status = "CRITICAL";

    digitalWrite(LED_PIN, HIGH);
    tone(BUZZER_PIN, 2000);
  }

  // -------------------------
  // OLED DISPLAY
  // -------------------------

  display.clearDisplay();

  display.setTextSize(1);

  display.setCursor(0, 0);
  display.println("WATTGUARD");

  display.setCursor(0, 12);
  display.print("V: ");
  display.print(voltage, 1);
  display.print("V");

  display.setCursor(65, 12);
  display.print("I: ");
  display.print(current, 2);
  display.print("A");

  display.setCursor(0, 24);
  display.print("Power : ");
  display.print(power, 0);
  display.println(" W");

  display.setCursor(0, 35);
  display.print("Peak  : ");
  display.print(peakPower, 0);
  display.println(" W");

  display.setCursor(0, 46);
  display.print("Energy: ");
  display.print(energyKWh, 3);
  display.println(" kWh");

  display.setCursor(0, 57);
  display.print(status);

  display.display();

  // -------------------------
  // SERIAL MONITOR
  // -------------------------

  Serial.print("Voltage: ");
  Serial.print(voltage, 1);

  Serial.print(" V | Current: ");
  Serial.print(current, 2);

  Serial.print(" A | Power: ");
  Serial.print(power, 1);

  Serial.print(" W | Peak: ");
  Serial.print(peakPower, 1);

  Serial.print(" W | Energy: ");
  Serial.print(energyKWh, 3);

  Serial.print(" kWh | Cost: Rs ");
  Serial.print(cost, 2);

  Serial.print(" | Status: ");
  Serial.println(status);

  delay(500);
}