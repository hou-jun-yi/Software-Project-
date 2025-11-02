#include <Servo.h>

// 핀 설정
#define PIN_SERVO 10

Servo myservo;

// 실험용 속도 변수
float _SERVO_SPEED;  // 각도당 지연시간(ms)

// 함수: 지정 속도로 서보 이동
void moveServoSmoothly(int startAngle, int endAngle, float speedDelay) {
  if (startAngle < endAngle) {
    for (int angle = startAngle; angle <= endAngle; angle++) {
      myservo.write(angle);
      delay(speedDelay);
    }
  } else {
    for (int angle = startAngle; angle >= endAngle; angle--) {
      myservo.write(angle);
      delay(speedDelay);
    }
  }
}

void setup() {
  myservo.attach(PIN_SERVO);
  myservo.write(0);
  delay(2000); // 시작 전 안정 대기
}

void loop() {
  // 실험 1: SERVO_SPEED = 3 (60초 동안 180도 이동)
  _SERVO_SPEED = 3; // 180도 × 3ms ≈ 540ms -> 너무 빠름 -> 실제로는 333ms 정도 필요
  // 정확히 60초에 맞추려면: 60000ms / 180 = 333.3ms
  moveServoSmoothly(0, 180, 333);
  delay(3000);
  // 실험 2: SERVO_SPEED = 0.3 (300초 동안 90도 이동)
  // 정확히 300초에 맞추려면: 300000ms / 90 = 3333.3ms
  _SERVO_SPEED = 0.3;
  moveServoSmoothly(0, 90, 3333);

  // 실험 완료 후 반복 중지
  while (true);
}
