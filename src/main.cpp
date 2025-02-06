#include <Arduino.h>

#define REQUEST_MEASURE_SIZE (9)
#define MAX_RX_BUFF_SIZE (128)

#define TIMEOUT_READ (1500)

#define REQUEST_MEASUREMENT_HEADER ("MEA")
#define FIRESTING_CHAN_NUM ("4")
#define FIRESTING_MEASUREMENT_PARAM ("47")

#define TEMP_POS_IN_MSG (8)
#define DO_POS_IN_MSG (9)
#define PH_POS_IN_MSG (17)

#define TEMP_DIVIDER (1000.0)
#define DO_DIVIDER (1000.0)
#define PH_DIVIDER (100.0)

char tx_buff[REQUEST_MEASURE_SIZE] = {0};
char rx_buff[MAX_RX_BUFF_SIZE] = {0};

/*
MEA 4 47 35 -165727 0 0 0 -300000 25890 0 1082 985128 21335 0 0 0 0 0 0 0
yo2
yo
MEA 4 47 35 172234 0 0 0 -300000 25890 0 559 985155 21358 0 0 0 0 0 0 0
*/

typedef struct __attribute__((packed)){
  char cmd[4];
  float_t temperature;
  float_t oxy_dissous;
  float_t ph;
} sRxData;

uint32_t start_read_time = 0;

void parseRxData(sRxData *data, char *dataStr, uint8_t size);
float_t getFloatAt(char *dataStr, uint8_t pos, uint8_t size);

void setup() {
  // put your setup code here, to run once:
  // int result = myFunction(2, 3);
  Serial.begin(115200);
  Serial1.begin(115200);



  sprintf(tx_buff, "%s %s %s", REQUEST_MEASUREMENT_HEADER, FIRESTING_CHAN_NUM, FIRESTING_MEASUREMENT_PARAM);

  Serial.println("Start");
}

void loop() {
  // Serial1.println("#LOGO");

  memset(rx_buff, 0, MAX_RX_BUFF_SIZE);// reset rx buff

  Serial1.println(tx_buff);//request measurement

  uint16_t read_size = 0;

  #define SIMUL_DATA //remove just for test when pyroscience not connect
  #if defined(SIMUL_DATA) //remove just for test when pyroscience not connect

  start_read_time = millis();
  while(true){
    //verification
    if(read_size>MAX_RX_BUFF_SIZE-1){
      Serial.println("error rx string too long");
      break;
    }
    if (start_read_time+TIMEOUT_READ < millis()){
      Serial.println("read timeout");
      break;
    }
    
    //read data
    rx_buff[read_size] = Serial1.read();
    
    if(rx_buff[read_size] == '\r'){
      // end of transmission
      break;
    }

    read_size++;
  }
  #else
  strcpy(rx_buff, "MEA 4 47 35 -165727 0 0 0 -300000 25890 0 1082 985128 21335 0 0 0 0 0 0 0\r");//remove only for simul
  read_size = strlen(rx_buff);//remove only for simul
  #endif


  Serial.println(rx_buff);// for debug
  Serial.println(read_size);// for debug

  sRxData data;
  parseRxData(&data, rx_buff, read_size);

  Serial.print("Temperature: ");
  Serial.println(data.temperature);
  Serial.print("DO: ");
  Serial.println(data.oxy_dissous);
  Serial.print("Ph: ");
  Serial.println(data.ph);  

  delay(1500);
  
  // put your main code here, to run repeatedly:
}

void parseRxData(sRxData *data, char *dataStr, uint8_t size){

  //read command name
  memcpy(data->cmd, dataStr, 3);
  data->cmd[4] = '\n';

  //read temperature
  data->temperature = getFloatAt(dataStr, TEMP_POS_IN_MSG, size) / TEMP_DIVIDER;

  //read do
  data->oxy_dissous = getFloatAt(dataStr, DO_POS_IN_MSG, size) / DO_DIVIDER;

  //read ph
  data->ph = getFloatAt(dataStr, PH_POS_IN_MSG, size) / PH_DIVIDER;
  
}

float_t getFloatAt(char *dataStr, uint8_t pos, uint8_t size){
  uint8_t wordCount = 0;
  uint8_t indexStartWord = 0;

  for (uint8_t i = 0; i < size; i++){
    if(dataStr[i] == ' ' || dataStr[i] == '\r' || dataStr[i] == '\n'){
      wordCount++;
      if(wordCount == pos){
        indexStartWord = i+1;
      }
      else if(wordCount == pos+1){
        char* pEnd = &(dataStr[i]); 
        return strtof(&(dataStr[indexStartWord]), &pEnd);
      }
    }
  }
  Serial.println("position not found");
  return 0.0;
}