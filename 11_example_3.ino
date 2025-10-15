//作业11P07
#include <Servo.h>  // 引入舵机控制库

// -------------------- 引脚定义 --------------------
#define PIN_LED   9    // LED（主动低）
#define PIN_TRIG  12   // 超声波传感器 TRIG 引脚
#define PIN_ECHO  13   // 超声波传感器 ECHO 引脚
#define PIN_SERVO 10   // 舵机引脚

// -------------------- 超声波测距参数 --------------------
#define SND_VEL 346.0      // 声速（m/s），24℃时约为346m/s
#define INTERVAL 25        // 采样间隔（毫秒）
#define PULSE_DURATION 10  // 超声波脉冲发射时间（微秒）

#define _DIST_MIN 180.0    // 最小检测距离（mm）→ 18cm
#define _DIST_MAX 360.0    // 最大检测距离（mm）→ 36cm
#define TIMEOUT ((INTERVAL / 2) * 1000.0)  // 超声波回波最大等待时间
#define SCALE (0.001 * 0.5 * SND_VEL)      // 回波时间转换为距离的系数
#define _EMA_ALPHA 0.5     // EMA滤波系数（新值权重）

// -------------------- 舵机参数 --------------------
#define _DUTY_MIN 1000   // 舵机最小脉宽（0°）
#define _DUTY_NEU 1500   // 舵机中间脉宽（90°）
#define _DUTY_MAX 2000   // 舵机最大脉宽（180°）

// -------------------- 全局变量 --------------------
Servo myservo;                   // 创建舵机对象
float dist_raw = _DIST_MAX;      // 当前测量的原始距离（mm）
float dist_ema = _DIST_MAX;      // EMA滤波后的距离
float dist_prev = _DIST_MAX;     // 上一次测量的有效距离
unsigned long last_sampling_time = 0;  // 上次采样时间（ms）

// -------------------- 函数声明 --------------------
float USS_measure(int TRIG, int ECHO);   // 超声波测距函数
float mapDistanceToAngle(float dist);    // 距离映射为角度函数

// -------------------- 初始化函数 --------------------
void setup() {
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  digitalWrite(PIN_TRIG, LOW);  // 确保初始时不发射超声波

  myservo.attach(PIN_SERVO);    // 初始化舵机
  myservo.writeMicroseconds(_DUTY_NEU);  // 初始角度90°

  Serial.begin(57600);  // 串口波特率（配合 Serial Plotter）
}

// -------------------- 主循环函数 --------------------
void loop() {
  // 控制采样周期
  if (millis() < (last_sampling_time + INTERVAL)) return;
  last_sampling_time = millis();

  // 超声波测距
  dist_raw = USS_measure(PIN_TRIG, PIN_ECHO);

  // -------------------- 1. 范围过滤器（Range Filter） --------------------
  // 仅当距离在18~36cm之间时有效，否则保持上一次测量值
  if (dist_raw < _DIST_MIN || dist_raw > _DIST_MAX) {
    dist_raw = dist_prev;        // 超出范围 → 不更新
    digitalWrite(PIN_LED, HIGH); // LED灭（无效）
  } else {
    dist_prev = dist_raw;        // 有效范围 → 更新
    digitalWrite(PIN_LED, LOW);  // LED亮（有效）
  }

  // -------------------- 2. EMA滤波器（Exponential Moving Average） --------------------
  // 平滑距离数据，减少抖动
  dist_ema = _EMA_ALPHA * dist_raw + (1 - _EMA_ALPHA) * dist_ema;

  // -------------------- 3. 距离 → 舵机角度 映射 --------------------
  float servo_angle = mapDistanceToAngle(dist_ema);
  int duty = map(servo_angle, 0, 180, _DUTY_MIN, _DUTY_MAX);
  myservo.writeMicroseconds(duty);  // 根据角度输出PWM信号

  // -------------------- 4. 串口输出（用于 Serial Plotter） --------------------
  Serial.print("Min:");    Serial.print(_DIST_MIN);
  Serial.print(",dist:");  Serial.print(min(dist_raw, _DIST_MAX + 100));  // 原始距离
  Serial.print(",ema:");   Serial.print(min(dist_ema, _DIST_MAX + 100));  // EMA滤波后距离
  Serial.print(",Servo:"); Serial.print(servo_angle);                     // 当前舵机角度
  Serial.print(",Max:");   Serial.print(_DIST_MAX);
  Serial.println("");  // 换行
}

// -------------------- 超声波测距函数 --------------------
float USS_measure(int TRIG, int ECHO) {
  digitalWrite(TRIG, HIGH);                 // 发射超声波
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);

  // pulseIn() 返回高电平持续时间 → 根据时间计算距离
  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE; // 单位：mm
}

// -------------------- 距离 → 角度映射函数 --------------------
float mapDistanceToAngle(float dist) {
  if (dist <= _DIST_MIN) return 0;        // 小于18cm → 舵机0°
  if (dist >= _DIST_MAX) return 180;      // 大于36cm → 舵机180°
  // 在18~36cm之间，线性映射到0~180°
  return (dist - _DIST_MIN) * 180.0 / (_DIST_MAX - _DIST_MIN);
}
