
#define PIN_IR A0
#define ANALOG_MAX 1023

void setup() {
  Serial.begin(115200);
  while (!Serial) {;}
  Serial.println("Ready.");
}

void loop() {

  unsigned int n = 11;       // 测量次数（示例）
  float position = 0.5f;     // 中位数
  int verbose = 1;           // 0=静默, 1=打印过程, 2=时间测量

  Serial.println("ir_sensor_filtered_store_sort()");

  unsigned long t0 = micros();
  unsigned int v = ir_sensor_filtered_store_sort(n, position, verbose);
  unsigned long t1 = micros();

  Serial.print("Final result = ");
  Serial.println(v);

  Serial.print("Total execution time (us) = ");
  Serial.println(t1 - t0);

  delay(3000);  // 每 3 秒执行一次
}

unsigned int ir_sensor_filtered_store_sort(unsigned int n, float position, int verbose) {
  if (n == 0) return 0;
  if (n > 100) n = 100; // 安全限制

  unsigned int samples[100];

  unsigned long read_time_total = 0;  // verbose = 2 用

  for (unsigned int i = 0; i < n; ++i) {
    unsigned long t_start = micros();    // 用于 verbose==2 的测量
    int raw = analogRead(PIN_IR);
    unsigned long t_end = micros();

    if (raw < 0) raw = 0;
    if (raw > ANALOG_MAX) raw = ANALOG_MAX;

    samples[i] = (unsigned int)raw;

    if (verbose == 1) {
      Serial.print("sample[");
      Serial.print(i);
      Serial.print("] = ");
      Serial.println(samples[i]);
    }

    if (verbose == 2) {
      read_time_total += (t_end - t_start);
    }

    delay(5); // 防止传感器连续读取干扰
  }

  if (verbose == 2) {
    Serial.print("Total ADC read time (us): ");
    Serial.println(read_time_total);
  }

  for (unsigned int i = 1; i < n; ++i) {
    unsigned int key = samples[i];
    int j = i - 1;

    while (j >= 0 && samples[j] > key) {
      samples[j + 1] = samples[j];
      j--;
    }
    samples[j + 1] = key;
  }

  if (verbose == 1) {
    Serial.print("sorted: ");
    for (unsigned int i = 0; i < n; ++i) {
      Serial.print(samples[i]);
      if (i + 1 < n) Serial.print(", ");
    }
    Serial.println();
  }

  if (position < 0.0f) position = 0.0f;
  if (position > 1.0f) position = 1.0f;

  unsigned int idx = (unsigned int)(position * (n - 1) + 0.5f);
  if (idx >= n) idx = n - 1;

  if (verbose == 1) {
    Serial.print("selected index = ");
    Serial.print(idx);
    Serial.print(", value = ");
    Serial.println(samples[idx]);
  }

  return samples[idx];
}
