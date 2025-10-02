#include <Servo.h>  // 引入 Arduino Servo 库，用来控制舵机

// -------------------- 引脚定义 --------------------
#define PIN_TRIG 12   // 超声波传感器 TRIG 发射引脚
#define PIN_ECHO 13   // 超声波传感器 ECHO 接收引脚
#define PIN_SERVO 10  // 舵机控制引脚
#define PIN_LED   9   // LED 引脚

Servo myServo;       // 创建舵机对象

// -------------------- 超声波测距函数 --------------------
// 返回测量到的距离（单位：厘米）
long getDistance() {
  // 发射超声波前先拉低 TRIG，确保信号干净
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);

  // 发送 10 微秒高电平脉冲，让模块发出超声波
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  // pulseIn() 读取 ECHO 高电平持续时间（单位：微秒）
  long duration = pulseIn(PIN_ECHO, HIGH);

  // 距离公式：distance = (声速 × 时间) / 2
  long distance = duration * 0.034 / 2; // 声速 0.034 cm/μs
  return distance;
}

// -------------------- Sigmoid 平滑函数 --------------------
// 用于平滑舵机移动，避免动作生硬
float sigmoid(float x) {
  return 1.0 / (1.0 + exp(-x));
}

// -------------------- 平滑控制舵机 --------------------
// startAngle：起始角度
// endAngle：目标角度
void moveServoSigmoid(int startAngle, int endAngle) {
  for (int i = 0; i <= 100; i++) {
    float t = (float)i / 100.0;                // t 从 0~1
    float s = sigmoid((t - 0.5) * 10);         // Sigmoid 拉伸，使运动曲线平滑
    int angle = startAngle + (endAngle - startAngle) * s; // 插值计算角度
    myServo.write(angle);                      // 写入舵机角度
    delay(20);                                 // 每步延迟 20ms，控制速度
  }
}

// -------------------- 初始化 --------------------
void setup() {
  pinMode(PIN_TRIG, OUTPUT);  // 超声波发射引脚设为输出
  pinMode(PIN_ECHO, INPUT);   // 超声波接收引脚设为输入
  pinMode(PIN_LED, OUTPUT);   // LED 引脚设为输出

  myServo.attach(PIN_SERVO);  // 绑定舵机到指定引脚
  myServo.write(0);           // 初始挡车杆放下
  digitalWrite(PIN_LED, LOW); // 初始 LED 灯熄灭

  Serial.begin(9600);         // 打开串口调试
}

// -------------------- 主循环 --------------------
void loop() {
  long d = getDistance();          // 获取当前距离
  Serial.print("Distance: ");      // 串口输出距离值
  Serial.println(d);

  // 如果车辆靠近（距离小于 20cm）
  if (d < 20) { 
    digitalWrite(PIN_LED, HIGH);   // 抬杆时点亮 LED
    moveServoSigmoid(0, 90);       // 平滑抬杆到 90°
    delay(2000);                   // 等待车辆通过
    moveServoSigmoid(90, 0);       // 平滑放下挡车杆
    digitalWrite(PIN_LED, LOW);    // 放下杆后立即熄灭 LED
  }

  delay(200);  // 循环间隔 200ms，避免检测过于频繁
}
