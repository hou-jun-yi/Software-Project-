int ledPin = 9;        
int brightness = 0;    
int fadeAmount = 5;    

void setup() {
  pinMode(ledPin, OUTPUT); 
}

void loop() {
  analogWrite(ledPin, brightness);
  brightness = brightness + fadeAmount;

  if (brightness <= 0 || brightness >= 255) {
    fadeAmount = -fadeAmount; // 反转变化方向
  }

  
  delay(30);
}
