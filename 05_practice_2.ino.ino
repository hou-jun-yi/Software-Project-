#define LED_PIN 7

void setup() {
  pinMode(LED_PIN, OUTPUT); // 设置GPIO 7为输出模式
}

void loop() {
  // 第一步：点亮 LED 1 秒
  digitalWrite(LED_PIN, HIGH);
  delay(1000);

  // 第二步：1 秒内闪烁 5 次 (每次 0.2秒，亮0.1秒灭0.1秒)
  for (int i = 0; i < 5; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(100); // LED 亮 0.1 秒
    digitalWrite(LED_PIN, LOW);
    delay(100); // LED 灭 0.1 秒
  }

  // 第三步：关闭 LED
  digitalWrite(LED_PIN, LOW);

  // 无限循环，结束程序
  while (1) {
    // 什么也不做
  }
}
