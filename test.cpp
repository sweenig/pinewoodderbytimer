void setup()
{
  Serial.begin(115200);
  while(!Serial.available()) SPARK_WLAN_Loop(); // Open terminal and press ENTER
  Serial.println("Reading...");
  pinMode(D2, INPUT);
  pinMode(D3, INPUT);
  pinMode(D4, INPUT);
  pinMode(A6, INPUT);
  pinMode(A7, INPUT);
  
}

void loop()
{
    Serial.print("D2: ");Serial.println(digitalRead(D2));
    Serial.print("D3: ");Serial.println(digitalRead(D3));
    Serial.print("D4: ");Serial.println(digitalRead(D4));
    Serial.print("A6: ");Serial.println(digitalRead(A6));
    Serial.print("A7: ");Serial.println(digitalRead(A7));
    Serial.println("====================");
    delay(1000);
}
