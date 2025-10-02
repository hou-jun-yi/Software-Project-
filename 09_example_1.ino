// Arduino pin assignment
#define PIN_LED  9
#define PIN_TRIG 12
#define PIN_ECHO 13

// configurable parameters
#define SND_VEL 346.0     // sound velocity at 24 celsius degree (unit: m/sec)
#define INTERVAL 25       // sampling interval (unit: msec)
#define PULSE_DURATION 10 // ultra-sound Pulse Duration (unit: usec)
#define _DIST_MIN 100     // minimum distance to be measured (unit: mm)
#define _DIST_MAX 300     // maximum distance to be measured (unit: mm)

#define TIMEOUT ((INTERVAL / 2) * 1000.0) // maximum echo waiting time (unit: usec)
#define SCALE (0.001 * 0.5 * SND_VEL)     // coefficent to convert duration to distance

// --- median filter parameters ---
#define N_SAMPLES 30   // 可改成 3, 10, 30 进行比较

// global variables
unsigned long last_sampling_time;   // unit: msec
float sampleBuffer[N_SAMPLES];      // 保存最近 N 个样本
int sampleIndex = 0;
bool bufferFilled = false;

void setup() {
  pinMode(PIN_LED,OUTPUT);
  pinMode(PIN_TRIG,OUTPUT);
  pinMode(PIN_ECHO,INPUT);
  digitalWrite(PIN_TRIG, LOW);

  Serial.begin(57600);
}

// --- 工具函数：求中位数 ---
float getMedian(float arr[], int size) {
  float temp[size];
  for (int i=0; i<size; i++) temp[i] = arr[i];

  // 简单排序（冒泡法即可，因为 N 不大）
  for (int i=0; i<size-1; i++) {
    for (int j=i+1; j<size; j++) {
      if (temp[j] < temp[i]) {
        float t = temp[i];
        temp[i] = temp[j];
        temp[j] = t;
      }
    }
  }

  if (size % 2 == 0)
    return (temp[size/2 - 1] + temp[size/2]) / 2.0;
  else
    return temp[size/2];
}

void loop() {
  if (millis() < last_sampling_time + INTERVAL)
    return;

  float dist_raw = USS_measure(PIN_TRIG, PIN_ECHO);

  // --- 限制范围，超出范围记为 0（无效值）---
  if ((dist_raw == 0.0) || (dist_raw > _DIST_MAX) || (dist_raw < _DIST_MIN)) {
    dist_raw = 0.0;  
  }

  // --- 保存到缓冲区 ---
  sampleBuffer[sampleIndex] = dist_raw;
  sampleIndex++;
  if (sampleIndex >= N_SAMPLES) {
    sampleIndex = 0;
    bufferFilled = true;
  }

  // --- 计算中位数 ---
  float dist_median = 0.0;
  if (bufferFilled) {
    dist_median = getMedian(sampleBuffer, N_SAMPLES);
  } else {
    dist_median = getMedian(sampleBuffer, sampleIndex); // 填满前先用已有的
  }

  // --- 串口输出 ---
  Serial.print("raw:"); Serial.print(dist_raw);
  Serial.print(", median:"); Serial.print(dist_median);
  Serial.println("");

  // --- 控制 LED ---
  if ((dist_median < _DIST_MIN) || (dist_median > _DIST_MAX))
    digitalWrite(PIN_LED, HIGH);  // LED OFF
  else
    digitalWrite(PIN_LED, LOW);   // LED ON

  last_sampling_time += INTERVAL;
}

// --- 超声波测距函数 ---
float USS_measure(int TRIG, int ECHO)
{
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);

  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE; // unit: mm


  // Pulse duration to distance conversion example (target distance = 17.3m)
  // - pulseIn(ECHO, HIGH, timeout) returns microseconds (음파의 왕복 시간)
  // - 편도 거리 = (pulseIn() / 1,000,000) * SND_VEL / 2 (미터 단위)
  //   mm 단위로 하려면 * 1,000이 필요 ==>  SCALE = 0.001 * 0.5 * SND_VEL
  //
  // - 예, pusseIn()이 100,000 이면 (= 0.1초, 왕복 거리 34.6m)
  //        = 100,000 micro*sec * 0.001 milli/micro * 0.5 * 346 meter/sec
  //        = 100,000 * 0.001 * 0.5 * 346
  //        = 17,300 mm  ==> 17.3m
}
