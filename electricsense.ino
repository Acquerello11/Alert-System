#include <ArtronShop_LineMessaging.h>
#include <WiFi.h> // นำเข้าไลบรารี่ WiFi
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 21, 22); // ที่อยู่ I2C (ส่วนใหญ่เป็น 0x27 หรือ 0x3F)

const int arraySize = 20;   // จำนวนค่าที่จะเก็บ
int rawValueArray[arraySize]; // ตัวแปรสำหรับเก็บค่าของ rawValue
int arrayIndex = 0;         // ตัวชี้ตำแหน่งในอาเรย์
long totalRawValue = 0;     // ผลรวมของค่าทั้งหมดในอาเรย์

const char* ssid     = "72"; // ชื่อ WiFi
const char* password = "lovenothings"; // รหัสผ่าน WiFi

const int currentPin = 32;  // ใช้ GPIO32 สำหรับ analog input

int minPeak, maxPeak;       // ค่าสูงสุด ต่ำสุดในช่วงเวลา
int rawValue;               // ค่า peak-to-peak
unsigned long startMillis;  // ตัวจับเวลา
#define LINE_TOKEN "nmG4oYYBbE8f3rwBuKstkFkgwExWc4kbWoGyVorkbk2dagOrlEy+O/G0WEpe2+mvtjTviE0XbEjAtghlc+eyRn1Oe04jCL4RmRRw9BGe1ogQ7LS8mNnVAG8F2fSqDil3mQabTzKl8k60d4SkzyjUVwdB04t89/1O/w1cDnyilFU=" // Channel access token
void setup() {
    Serial.begin(115200);
    analogReadResolution(12);  // ความละเอียด 12-bit (0–4095)
    // We start by connecting to a WiFi network

    lcd.init();           // เริ่มต้นใช้งาน LCD
    lcd.backlight();      // เปิดแสงพื้นหลัง

    Serial.println();
    Serial.println("******************************************************");
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password); // เริ่มต้นเชื่อมต่อ WiFi

    while (WiFi.status() != WL_CONNECTED) { // วนลูปหากยังเชื่อมต่อ WiFi ไม่สำเร็จ
        delay(500);
        Serial.print(".");
    }
    
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    
    LINE.begin(LINE_TOKEN);
}
void loop() {
  maxPeak = 0;     // เริ่มต้นค่า peak สูงสุด
  minPeak = 4095;  // เริ่มต้นค่า peak ต่ำสุด
  startMillis = millis();  // บันทึกเวลาเริ่มต้น

  // วัดค่า peak-to-peak ภายใน 16ms (ประมาณ 3/4 cycle ของ 50Hz)
  while (millis() - startMillis < 16) {
    int sample = analogRead(currentPin);  // อ่านค่า analog
    if (sample > maxPeak) maxPeak = sample;
    if (sample < minPeak) minPeak = sample;
  }

  rawValue = maxPeak - minPeak;  // คำนวณค่า peak-to-peak

  rawValueArray[arrayIndex] = rawValue;
  arrayIndex++;

  // แปลงเป็นแรงดัน (สมมุติว่าใช้แรงดันอ้างอิง 3.3V)

 if (arrayIndex >= arraySize) {
    totalRawValue = 0;
    for (int i = 0; i < arraySize; i++) {
      totalRawValue += rawValueArray[i];
      Serial.println(rawValueArray[i]);
    }

    // ตรวจสอบเงื่อนไขตามค่าผลรวม
    if (totalRawValue >= 2400) {
        Serial.println(" ADC total ");
        Serial.println(totalRawValue);
      if (LINE.send("Uf55ad6a39092e2de041b43500ee51bfe", "แจ้งเตือนๆ ปั้มเกิดสิ่งผิดปกติ")) {
        Serial.println("Send notify successful");
      } else {
        Serial.printf("Send notify fail. check your token (code: %d)\n", LINE.status_code);
      }
    } else {
        Serial.println(" ADC total ");
        Serial.println(totalRawValue);
      Serial.println("Result is below threshold. Restarting loop...");
    }

    arrayIndex = 0; // รีเซ็ตตัวชี้ตำแหน่ง
  }

  // แสดงผลบน LCD
  
  lcd.clear();
   if(totalRawValue>= 2400){
  lcd.setCursor(0, 0);
  lcd.print("Alert State");
   }else{
  lcd.setCursor(0, 0);
  lcd.print("Normal State");
  }

  delay(500);  // หน่วงเวลาเล็กน้อยก่อนวัดรอบใหม่
}
