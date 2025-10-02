#include <Servo.h>  // 引入 Arduino Servo 库，用来控制舵机

// -------------------- 引脚定义 --------------------
#define PIN_TRIG 12   // 超声波传感器 TRIG 发射引脚
#define PIN_ECHO 13   // 超声波传感器 ECHO 接收引脚
#define PIN_SERVO 10  // 舵机控制引脚
#define PIN_LED   9   // LED 引脚

Servo myServo;       // 创建舵机对象

// -------------------- 超声波测距函数 --------------------
long getDistance() {
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  long duration = pulseIn(PIN_ECHO, HIGH);  
  long distance = duration * 0.034 / 2;     // 单位：厘米
  return distance;
}

// -------------------- 平滑角度控制函数 --------------------
// startAngle: 起始角度
// endAngle: 目标角度
// stepDelay: 每步延迟(ms)，控制运动速度
void moveServoByAngle(int startAngle, int endAngle, int stepDelay = 20) {
  if (startAngle < endAngle) {
    // 抬杆：从小角度到大角度逐步增加
    for (int angle = startAngle; angle <= endAngle; angle++) {
      myServo.write(angle);
      delay(stepDelay);
    }
  } else {
    // 放杆：从大角度到小角度逐步减少
    for (int angle = startAngle; angle >= endAngle; angle--) {
      myServo.write(angle);
      delay(stepDelay);
    }
  }
}

// -------------------- 初始化 --------------------
void setup() {
  pinMode(PIN_TRIG, OUTPUT);  
  pinMode(PIN_ECHO, INPUT);   
  pinMode(PIN_LED, OUTPUT);   

  myServo.attach(PIN_SERVO);  
  myServo.write(0);           // 初始挡车杆放下
  digitalWrite(PIN_LED, LOW); // 初始 LED 灯熄灭

  Serial.begin(9600);         
}

// -------------------- 主循环 --------------------
void loop() {
  long d = getDistance();          
  Serial.print("Distance: ");      
  Serial.println(d);

  // 当车辆靠近（距离小于 20cm）
  if (d < 20) { 
    digitalWrite(PIN_LED, HIGH);         // 抬杆时 LED 点亮
    moveServoByAngle(0, 90, 20);         // 平滑抬杆到 90°
    delay(2000);                          // 等待车辆通过
    moveServoByAngle(90, 0, 20);         // 平滑放下挡车杆
    digitalWrite(PIN_LED, LOW);          // 放下后 LED 熄灭
  }

  delay(200);  // 循环间隔
}
