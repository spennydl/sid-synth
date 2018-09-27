void setup() {
  // put your setup code here, to run once:
  Serial.begin(31250);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, 1);
    delay(1000);
    digitalWrite(LED_BUILTIN, 0);
    delay(1000);
}

void loop() {
  // put your main code here, to run repeatedly:
  int b;
  while (Serial.available()) {
    b = Serial.read();
    
    while (b) {
        digitalWrite(LED_BUILTIN, b & 1);
        b >>= 1;
        delay(200);
    }
  }
}
