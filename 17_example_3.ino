#include <Servo.h>

// Arduino pin assignment
#define PIN_IR    A0        // IR sensor at Pin A0
#define PIN_LED   9
#define PIN_SERVO 10

// Servo microsecond duty (根据舵机规格，常用值如下)
#define _DUTY_MIN 500      // servo full clock-wise position (0 degree)
#define _DUTY_NEU 1500      // servo neutral position (90 degree)
#define _DUTY_MAX 2500      // servo full counter-clockwise position (180 degree)

// Distance limits
#define _DIST_MIN  100.0    // minimum distance 100mm
#define _DIST_MAX  250.0    // maximum distance 250mm

// EMA Filter (alpha 越大越敏感，越小越平滑；PPT 要求适当 alpha)
#define EMA_ALPHA  0.2      // 你可以调成 0.1 ~ 0.3

// Loop period
#define LOOP_INTERVAL 20     // ms, PPT 要求 ≥ 20ms

Servo myservo;
unsigned long last_loop_time = 0;

float dist_ema = _DIST_MIN;

void setup()
{
  pinMode(PIN_LED, OUTPUT);

  myservo.attach(PIN_SERVO);
  myservo.writeMicroseconds(_DUTY_NEU);

  Serial.begin(1000000);   // PPT 要求 1,000,000 bps
}

void loop()
{
  unsigned long time_curr = millis();
  if (time_curr < (last_loop_time + LOOP_INTERVAL))
    return;
  last_loop_time = time_curr;

  int duty;
  float a_value, dist_raw;

  a_value = analogRead(PIN_IR);

  // raw distance from IR sensor (PPT 提供的公式)
  dist_raw = ((6762.0 / (a_value - 9.0)) - 4.0) * 10.0;

  // -------- 范围过滤（Range Filter）---------
  if (dist_raw < _DIST_MIN || dist_raw > _DIST_MAX)
  {
    // 超出范围，不更新 EMA，无 LED
    digitalWrite(PIN_LED, LOW);
  }
  else
  {
    // 范围内才做 EMA 过滤
    dist_ema = EMA_ALPHA * dist_raw + (1.0 - EMA_ALPHA) * dist_ema;
    digitalWrite(PIN_LED, HIGH);
  }

  // -------- 不使用 map()，手写线性映射 ----------
  duty = _DUTY_MIN + (dist_ema - _DIST_MIN) * (_DUTY_MAX - _DUTY_MIN) / (_DIST_MAX - _DIST_MIN);

  myservo.writeMicroseconds(duty);

  // -------- Serial 输出（PPT 格式）-----------
  Serial.print("_DUTY_MIN:");  Serial.print(_DUTY_MIN);
  Serial.print(",_DIST_MIN:"); Serial.print(_DIST_MIN);
  Serial.print(",IR:");        Serial.print(a_value);
  Serial.print(",dist_raw:");  Serial.print(dist_raw);
  Serial.print(",ema:");       Serial.print(dist_ema);
  Serial.print(",servo:");     Serial.print(duty);
  Serial.print(",_DIST_MAX:"); Serial.print(_DIST_MAX);
  Serial.print(",_DUTY_MAX:"); Serial.print(_DUTY_MAX);
  Serial.println("");
}
