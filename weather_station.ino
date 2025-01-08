#include <AdafruitIO_WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include <ESP32Servo.h>

// Adafruit IO and WiFi credentials
#define IO_USERNAME "abc"
#define IO_KEY "xyz"
#define WIFI_SSID "ABC"
#define WIFI_PASS "XYZ"

// DHT11 setup
#define DHTPIN 4         // DHT11 data pin connected to GPIO 4
#define DHTTYPE DHT11    // DHT11 sensor type
DHT dht(DHTPIN, DHTTYPE);

// OLED display setup (128x64 pixels)
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Adafruit IO instance
AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);
AdafruitIO_Feed *temperatureFeed = io.feed("temperature");
AdafruitIO_Feed *humidityFeed = io.feed("humidity");

// LED pins
#define LED1 12  // LED for humidity change
#define LED2 14  // LED for humidity > 35%
#define LED3 27  // LED for temperature > 26°C

// Servo setup
Servo myServo;
#define SERVO_PIN 13 // Servo control pin

// Previous humidity value for change detection
float lastHumidity = -1.0;

void setup() {
  // Serial Monitor
  Serial.begin(115200);
  
  // Initialize DHT sensor
  dht.begin();
  
  // Initialize OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  
  // Clear display buffer
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.print("Connecting to IO...");
  display.display();
  
  // Initialize LED pins
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  
  // Turn off LEDs initially
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);

  // Initialize servo motor
  myServo.attach(SERVO_PIN);
  myServo.write(0); // Set servo to initial position (0 degrees)

  // Connect to Adafruit IO
  io.connect();

  // Wait for a connection
  while (io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("\nConnected to Adafruit IO!");
  display.clearDisplay();
  display.setCursor(0,0);
  display.print("Connected to IO");
  display.display();
}

void loop() {
  // Reading DHT11 sensor values
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Check if readings failed
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    display.clearDisplay();
    display.setCursor(0,0);
    display.print("DHT read failed!");
    display.display();
    return;
  }

  // Display on OLED
  display.clearDisplay();
  display.setCursor(0,0);
  display.print("Temperature: ");
  display.print(temperature);
  display.print(" C");
  display.setCursor(0, 20);
  display.print("Humidity: ");
  display.print(humidity);
  display.print(" %");
  display.display();

  // Send data to Adafruit IO
  Serial.print("Sending temperature: ");
  Serial.println(temperature);
  temperatureFeed->save(temperature);

  Serial.print("Sending humidity: ");
  Serial.println(humidity);
  humidityFeed->save(humidity);

  // Control LEDs based on conditions
  if (temperature > 25) {
    digitalWrite(LED1, HIGH);
  } else {
    digitalWrite(LED1, LOW);
  }

  // Turn on LED2 if humidity > 35%
  if (humidity >= 35.0 && humidity <= 50) {
    digitalWrite(LED2, LOW);
  } else {
    digitalWrite(LED2, HIGH);
  }

  // Turn on LED3 and move servo if temperature > 26°C
  if (temperature > 30.0) {
    digitalWrite(LED3, HIGH);
    myServo.write(90); // Move servo to 90 degrees
  } else {
    digitalWrite(LED3, LOW);
    myServo.write(0);  // Move servo back to 0 degrees
  }

  // Delay before the next reading
  delay(15000);  // 15 seconds
}