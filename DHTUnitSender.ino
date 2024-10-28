#include <esp_now.h> 
#include <WiFi.h> 
#include <Wire.h> 
#include <LiquidCrystal_I2C.h> 
#include "DHT.h" 

#define DHTPIN 4 
#define DHTTYPE DHT11 

DHT dht(DHTPIN, DHTTYPE); 
LiquidCrystal_I2C lcd(0x27, 16, 2); // Alamat I2C dapat berbeda, sesuaikan dengan alamat modul Anda 
uint8_t broadcastAddress[] = {0xE4, 0x65, 0xB8, 0x0C, 0x23, 0x04}; // Alamat MAC tujuan 
struct SensorData { 
  float temperature; 
  float humidity; 
  }; 

SensorData sensorData; 
esp_now_peer_info_t peerInfo; 
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) { 
Serial.print("\r\nLast Packet Send Status:\t"); 
Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" 
: "Delivery Fail"); 
} 

void setup() { 
  Serial.begin(115200); 
  lcd.init(); 
  lcd.backlight(); 
  Serial.println(F("DHT11 test!")); 
  dht.begin(); 
  WiFi.mode(WIFI_STA); 
  if (esp_now_init() != ESP_OK) { 
    Serial.println("Error initializing ESP-NOW"); 
    return; 
    } 
  
  esp_now_register_send_cb(OnDataSent); 
  memcpy(peerInfo.peer_addr, broadcastAddress, 6); 
  peerInfo.channel = 0;   
  peerInfo.encrypt = false; 
  if (esp_now_add_peer(&peerInfo) != ESP_OK) { 
    Serial.println("Failed to add peer"); 
    return; 
  } 
} 

void loop() { 
  delay(2000); 
  sensorData.humidity = dht.readHumidity(); 
  sensorData.temperature = dht.readTemperature(); 
  if (isnan(sensorData.humidity) || isnan(sensorData.temperature)) { 
    Serial.println(F("Failed to read from DHT sensor!")); 
    lcd.clear(); 
    lcd.setCursor(0, 0); 
    lcd.print("Read failed"); 
    return; 
    } 
    
  lcd.clear(); 
  lcd.setCursor(0, 0); 
  lcd.print("Humidity: "); 
  lcd.print(sensorData.humidity, 2); 
  lcd.print("%"); 
  lcd.setCursor(0, 1); 
  lcd.print("Temp: "); 
  lcd.print(sensorData.temperature, 2); 
  lcd.print((char)223); // Degree symbol 
  lcd.print("C"); 
  Serial.print(F("Humidity: ")); 
  Serial.print(sensorData.humidity); 
  Serial.print(F("%  Temperature: "));
  Serial.print(sensorData.temperature); 
  Serial.println(F("°C")); 
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &sensorData, 
  sizeof(sensorData)); 
  
  if (result == ESP_OK) { 
    Serial.println("Sent with success"); 
  } else { 
    Serial.println("Error sending the data"); 
  } 
}