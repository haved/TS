void setup() {
  Serial.begin(115200);
  Serial.print("XXX");
  
  Serial.print(">M");
  Serial.write(1);
}

void loop() {
  delay(4000);
  
  Serial.print(">s");
  Serial.write(2);
}
