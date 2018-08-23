void setup() {
  Serial.begin(115200);
  Serial.print("XXX");
}

void loop() {
  delay(4000);
  
  Serial.print(">M");
  Serial.write(1);
  
  Serial.print(">s");
  Serial.write(2);
}
