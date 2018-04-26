void setup() {
  Serial.begin(9600);
}

void loop() {
  int i = Serial.read();
  if(i != -1)
    Serial.write((char)i);
}
