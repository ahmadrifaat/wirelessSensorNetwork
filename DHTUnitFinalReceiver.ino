// Rangkaian Terakhir 
#include <esp_now.h> 
#include <WiFi.h> 
#include <LiquidCrystal_I2C.h> 
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Struct untuk menyimpan data yang diterima 
struct SensorData { 
float temperature; 
float humidity; 
}; 

// Alamat MAC dari kedua transmitter 
uint8_t transmitter1[] = {0xD4, 0x8A, 0xFC, 0x60, 0x9B, 0xD0}; 
uint8_t transmitter2[] = {0xE8, 0x6B, 0xEA, 0xDF, 0x99, 0xCC}; 
SensorData sensorData1; 
SensorData sensorData2; 
bool newData1 = false; 
bool newData2 = false; 
unsigned long lastDisplayTime = 0; 
const unsigned long displayInterval = 3000; // Interval untuk menampilkan data (ms) 
bool displayingData1 = true; 

// Fungsi callback yang akan dijalankan saat data diterima 
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) { 
  if (memcmp(mac, transmitter1, 6) == 0) { 
    memcpy(&sensorData1, incomingData, sizeof(sensorData1)); 
    newData1 = true; 
    Serial.print("Received data from transmitter 1\n"); 
    Serial.print("Temperature: "); 
    Serial.println(sensorData1.temperature); 
    Serial.print("Humidity: "); 
    Serial.println(sensorData1.humidity); 
  } else if (memcmp(mac, transmitter2, 6) == 0) { 
    memcpy(&sensorData2, incomingData, sizeof(sensorData2)); 
    newData2 = true; 
    Serial.print("Received data from transmitter 2\n"); 
    Serial.print("Temperature: "); 
    Serial.println(sensorData2.temperature); 
    Serial.print("Humidity: "); 
    Serial.println(sensorData2.humidity); 
  } 
} 

void setup() { 
  lcd.init(); 
  lcd.backlight(); 
  Serial.begin(115200); 

// Set device as a Wi-Fi Station 
  WiFi.mode(WIFI_STA); 
// Init ESP-NOW 
  if (esp_now_init() != ESP_OK) { 
    Serial.println("Error initializing ESP-NOW"); 
    lcd.clear(); 
    lcd.setCursor(0, 0); 
    lcd.print("Error initializing"); 
    return; 
  }

// Register callback function to receive data 
  esp_now_register_recv_cb(OnDataRecv); 
} 

void loop() { 
  unsigned long currentTime = millis(); 
  static int scrollIndex1 = 0; 
  static int scrollIndex2 = 0; 
  if (currentTime - lastDisplayTime >= displayInterval) { 
    lastDisplayTime = currentTime; 
  if (displayingData1 && newData1) { 
  String line = "Temp: " + String(sensorData1.temperature, 2) + (char)223 + 
  "C Hum: " + String(sensorData1.humidity, 2) + "%"; 
  if (line.length() > 16) { 
    scrollIndex1 = (scrollIndex1 + 1) % line.length(); 
    lcd.clear(); 
    lcd.setCursor(0, 0); 
    lcd.print("Transmitter 1"); 
    lcd.setCursor(0, 1); 
    lcd.print(line.substring(scrollIndex1)); 
  
    if (line.length() - scrollIndex1 < 16) { 
      lcd.print(line.substring(0, 16 - (line.length() - scrollIndex1))); 
    } 
    } else { 
      lcd.clear(); 
      lcd.setCursor(0, 0); 
      lcd.print("Transmitter 1"); 
      lcd.setCursor(0, 1); 
      lcd.print(line); 
    } 
    displayingData1 = false; 
    } else if (!displayingData1 && newData2) { 
      String line = "Temp: " + String(sensorData2.temperature, 2) + (char)223 + 
      "C Hum: " + String(sensorData2.humidity, 2) + "%"; 
      if (line.length() > 16) { 
        scrollIndex2 = (scrollIndex2 + 1) % line.length(); 
        lcd.clear(); 
        lcd.setCursor(0, 0); 
        lcd.print("Transmitter 2"); 
        lcd.setCursor(0, 1); 
        lcd.print(line.substring(scrollIndex2)); 
    if (line.length() - scrollIndex2 < 16) { 
      lcd.print(line.substring(0, 16 - (line.length() - scrollIndex2))); 
    } 
    } else { 
      lcd.clear(); 
      lcd.setCursor(0, 0); 
      lcd.print("Transmitter 2"); 
      lcd.setCursor(0, 1); 
      lcd.print(line); 
    } 
    displayingData1 = true; 
  } 
  } 
}