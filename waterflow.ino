#include <ArtronShop_LineMessaging.h>
#include <WiFi.h> // นำเข้าไลบรารี่ WiFi

const char* ssid     = "72"; // ชื่อ WiFi
const char* password = "lovenothings"; // รหัสผ่าน WiFi

volatile int flow_frequency;  // ตัวแปรนับจำนวนพัลส์จากเซ็นเซอร์
float flow_rate = 0.0;        // คำนวณลิตรต่อวินาที
float total_litres = 0.0;     // ปริมาณน้ำรวม
const byte flowsensor = 32;   // ขาที่ต่อกับเซ็นเซอร์

unsigned long currentTime;
unsigned long cloopTime;

void flow() {
  flow_frequency++;  // ฟังก์ชันอินเทอร์รัพท์ เพิ่มพัลส์ทุกครั้งที่เกิดสัญญาณ
}

#define LINE_TOKEN "nmG4oYYBbE8f3rwBuKstkFkgwExWc4kbWoGyVorkbk2dagOrlEy+O/G0WEpe2+mvtjTviE0XbEjAtghlc+eyRn1Oe04jCL4RmRRw9BGe1ogQ7LS8mNnVAG8F2fSqDil3mQabTzKl8k60d4SkzyjUVwdB04t89/1O/w1cDnyilFU=" // Channel access token
void setup() {
    pinMode(flowsensor, INPUT_PULLUP);  // ใช้ pull-up ภายใน

  Serial.begin(115200);
  attachInterrupt(digitalPinToInterrupt(flowsensor), flow, RISING);  // ใช้ขอบขาขึ้น
  currentTime = millis();
  cloopTime = currentTime;
    // We start by connecting to a WiFi network

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
  currentTime = millis();

  if (currentTime >= (cloopTime + 1000)) {  // คำนวณทุก ๆ 1 วินาที
    cloopTime = currentTime;

    if (flow_frequency >= 10) {
      // อัตราการไหล (L/min) = พัลส์ต่อวินาที / 7.5
      float litre_per_minute = flow_frequency / 7.5;
      flow_rate = litre_per_minute / 60.0;   // แปลงเป็นลิตรต่อวินาที
      total_litres += flow_rate;             // สะสมปริมาณน้ำ

      Serial.print("Flow rate: ");
      Serial.print(flow_rate, 3);
      Serial.print(" L/sec | Total: ");
      Serial.print(total_litres, 3);
      Serial.println(" L");
      if(flow_rate >=0.050){
      if (LINE.send("Uf55ad6a39092e2de041b43500ee51bfe", "น้ำล้นที่ปั๊ม....")) {  // ถ้าส่งข้อความ "รถโดนขโมย" ไปที่ LINE สำเร็จ
        Serial.println("Send notify successful"); // ส่งข้อความ "Send notify successful" ไปที่ Serial Monitor
      } else { // ถ้าส่งไม่สำเร็จ
        Serial.printf("Send notify fail. check your token (code: %d)\n", LINE.status_code); // ส่งข้อความ "Send notify fail" ไปที่ Serial Monitor
      }
      }else{
      }
   flow_frequency = 0;  // รีเซ็ตตัวนับพัลส์
    } else {
      Serial.println("Flow rate = 0 L/sec");
      }
    }
    delay(1000);
  }