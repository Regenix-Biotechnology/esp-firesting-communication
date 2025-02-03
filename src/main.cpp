#include <Arduino.h>

#define BUFF_SIZE (6)

char buff[BUFF_SIZE] = {0};

void setup() {
  // put your setup code here, to run once:
  // int result = myFunction(2, 3);
  Serial.begin(115200);
  Serial1.begin(115200);

  // while (!Serial1.available() || !Serial.available()){}
}

void loop() {
  // Serial.println("#LOGO");
  Serial1.println("#LOGO");
  Serial1.read(buff, BUFF_SIZE);

  for (uint8_t i = 0; i < BUFF_SIZE; i++) {
    Serial.print(buff[i]);
  }
  Serial.println("");

  delay(1500);
  
  // put your main code here, to run repeatedly:
}
