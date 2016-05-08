
int LED_PIN = 13;
int ASOUND_PIN = A7;
int DSOUND_PIN = 44;

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);  // declare the ledPin as an OUTPUT:
  pinMode(DSOUND_PIN, INPUT);
//  intMax = analogRead(sensorPin) ; // read the analog pin and use the value for the level of background noise
}

int max_dig = 0;
int max_ana = 0;


  int min_detected_ = 6;
  int max_detected_;

void loop() {
  int ana_val = analogRead(ASOUND_PIN);
  int dig_val = digitalRead(DSOUND_PIN);
  if (ana_val > max_ana) {
    max_ana = ana_val;
  }
  if (dig_val > max_dig) {
    max_dig = dig_val;
  }
  
  Serial.print(ana_val);
  Serial.print("(");
  Serial.print(max_ana);
  Serial.print(")");
  
  Serial.print(dig_val);
  Serial.print("(");
  Serial.print(max_dig);  
  Serial.println(")");
  
  delay(50);
}
