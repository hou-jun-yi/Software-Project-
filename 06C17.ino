int ledPin = 7;          
int period = 1000;       // PWM 주기, 단위: 마이크로초(us), 기본값은 1000us (10000/100으로도 설정 가능)
int duty = 0;            // 듀티 사이클 duty (0~100), HIGH 신호가 차지하는 비율(%)
int fadeAmount = 1;      // duty 값이 변하는 단계 크기

// 주기 설정
void set_period(int p) {
  period = p;
}

// 듀티 사이클 설정 (범위: 0~100)
// duty=0 → 완전히 꺼짐, duty=100 → 최대 밝기
void set_duty(int d) {
  if (d < 0) d = 0;        // 최소값 제한
  if (d > 100) d = 100;    // 최대값 제한
  duty = d;
}

// 하나의 PWM 파형 주기를 생성
// 현재 duty 값에 따라 HIGH/LOW 시간을 계산
// digitalWrite()로 파형 출력
void softPWM() {
  // HIGH 신호 시간 = 주기 × duty
  int highTime = period * duty / 100;   

  // LOW 신호 시간 = 전체 주기 - HIGH 시간
  int lowTime  = period - highTime;     

  // duty > 0 이면 HIGH 출력 유지
  if (highTime > 0) {
    digitalWrite(ledPin, HIGH);          // 핀을 HIGH로 설정
    delayMicroseconds(highTime);         // HIGH 유지 시간
  }

  // duty < 100 이면 LOW 출력 유지
  if (lowTime > 0) {
    digitalWrite(ledPin, LOW);           // 핀을 LOW로 설정
    delayMicroseconds(lowTime);          // LOW 유지 시간
  }
}

void setup() {
  pinMode(ledPin, OUTPUT);   
  set_period(1000);         // 주기를 1000us로 설정
}

void loop() {
  // 현재 duty 값 설정
  set_duty(duty);
  // 하나의 PWM 주기를 출력
  softPWM();   
  // duty 값 갱신 (삼각파 형태의 밝기 변화)
  // fadeAmount 값을 이용해 duty가 증가하거나 감소하도록 설정
  duty += fadeAmount;   
  // duty 값이 경계에 도달하면 방향 반전
  if (duty <= 0 || duty >= 100) {
    fadeAmount = -fadeAmount;   // duty 변화 방향 반전
  }
}
