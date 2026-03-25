#include <Arduino.h>
#include <Servo.h>

const int PIN_MIC = 2;       
const int PIN_ISD = 3;       
const int SERVO_PIN = 9;     
const int LED_PIN = 13;      

Servo myServo;
volatile int tapCount = 0;
volatile unsigned long lastTapTime = 0;

// Ngắt cho Micro
void soundISR() {
    unsigned long now = millis();
    if (now - lastTapTime > 300) { // Chống nhiễu 300ms
        tapCount++;
        lastTapTime = now;
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(PIN_MIC, INPUT);
    pinMode(PIN_ISD, OUTPUT);
    pinMode(LED_PIN, OUTPUT);
    
    myServo.attach(SERVO_PIN);
    myServo.write(90); 
    
    attachInterrupt(digitalPinToInterrupt(PIN_MIC), soundISR, RISING);
    Serial.println("HE THONG SAN SANG!");
    Serial.println("Nhap 1, 2, 3 tu ban phim hoac go vao Mic:");
}

void loop() {
    // --- LỆNH TỪ SERIAL MONITOR ---
    if (Serial.available() > 0) {
        char incomingByte = Serial.read();
        if (incomingByte == '1') tapCount = 1;
        else if (incomingByte == '2') tapCount = 2;
        else if (incomingByte == '3') tapCount = 3;
    }

    // --- LOGIC ĐIỀU KHIỂN ---
    // Bước 1: Chỉ bật LED
    if (tapCount == 1) {
        digitalWrite(LED_PIN, HIGH);
        myServo.write(90);
    } 
    
    // Bước 2: Nhạc + Servo quay
    else if (tapCount == 2) {
        static bool isMusicTriggered = false;
        if (!isMusicTriggered) {
            Serial.println("Dang chay Stage 2...");
            digitalWrite(PIN_ISD, HIGH);
            delay(200);
            digitalWrite(PIN_ISD, LOW);
            isMusicTriggered = true;
        }
        myServo.write(180); // Servo 360 quay toàn tốc
        digitalWrite(LED_PIN, HIGH);
    } 
    
    // Bước 3 hoặc Reset: Tắt hết
    else if (tapCount >= 3) {
        Serial.println("Reset he thong...");
        digitalWrite(LED_PIN, LOW);
        myServo.write(90); // Dừng servo
        digitalWrite(PIN_ISD, LOW);
        
        delay(500); // Đợi một chút để hệ thống ổn định
        tapCount = 0;
        asm volatile ("  jmp 0"); // Reset mềm để xóa mọi trạng thái static
    }
}