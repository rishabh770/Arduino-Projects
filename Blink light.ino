void setup()
{
  pinMode(13, OUTPUT);
  pinMode(12,OUTPUT);
  pinMode(11,OUTPUT);
  
}

void loop()
{
  //RED Light is on 
  digitalWrite(13, HIGH);
  delay(5000); 
  digitalWrite(13, LOW);
  
  //Green Light if on
  digitalWrite(11,HIGH);
  delay(2000);
  digitalWrite(11,LOW);
  
  //Yellow Light is on 
  digitalWrite(12,HIGH);
  delay(1000);
  digitalWrite(12,LOW);
}