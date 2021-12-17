#include <led_light.h>

#define USE_BRIGHTNESS 1             //boolean for using brightness
#define MAX_LED 30                   // how many leds are on the strip
#define PI 3.14159265                //needed for calculating brightness

extern TIM_HandleTypeDef htim2; 
extern RNG_HandleTypeDef hrng;

uint16_t pwmData[(24*MAX_LED)+50];   //amount of data that's going to be sent in one go

uint8_t LED_Data[MAX_LED][4];
uint8_t LED_Mod[MAX_LED][4]; 

int temp1, temp2, temp3;           //variables for random color creation 
uint32_t gen;                      //variable for RNG
int ticks = 0;                     //timer         
uint16_t  effStep = 0;             //variable for rainbow effect



void set_LED(int LEDnum, int Red, int Green, int Blue){    //forms an array of data that's going to be pushed to led strip(acc. to documentation). it contains:
  LED_Data[LEDnum][0] = LEDnum;                            // number of led
  LED_Data[LEDnum][1] = Green;                             //amount of green on the right position
  LED_Data[LEDnum][2] = Red;                               //amount of red on the right position
  LED_Data[LEDnum][3] = Blue;                              //amount of blue on the right position
}


void set_Brightness(int brightness){                      //calculates the brightness if it is used
  #if USE_BRIGHTNESS
    if(brightness >45) brightness = 45;
      for(int i = 0; i < MAX_LED; i++){
        LED_Mod[i][0] = LED_Data[i][0];
        for(int j = 1; j<4; j++){
          float angle = 90-brightness;
          angle = angle*PI / 180;
          LED_Mod[i][j] = (LED_Data[i][j])/(tan(angle));
        }
      }
  #endif
}


void led_send (void){                                    //send all the data to led strip
  uint32_t indx = 0;
  uint32_t color;
  
  for(int i = 0; i < MAX_LED; i++){
    #if USE_BRIGHTNESS
      color = ((LED_Mod[i][1]<<16) | (LED_Mod[i][2]<<8) | (LED_Mod[i][3]));
    #else
      color = ((LED_Data[i][1]<<16) | (LED_Data[i][2]<<8) | (LED_Data[i][3]));
    #endif
    for(int i = 23; i >= 0; i--){
      if(color&(1<<i)){
        pwmData[indx] = 26;     
      }
      else 
        pwmData[indx] = 13;   
      indx++;
    }
  }
  for(int i = 0; i < 50; i++){
    pwmData[indx] = 0;
    indx++;
  }
  HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_1, (uint32_t *)pwmData, indx);
}


void send(int Green, int Red, int Blue){               //sends amount of data with colors
  uint32_t data = (Green<<16) | (Red<<8) | Blue;
  for(int i = 23; i >= 0; i--){
    if(data&(1<<i)) pwmData[i] = 26;
    else pwmData[i] = 13;    
  }
  HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_1, (uint32_t*)pwmData, 24);
}


void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim){ 
  HAL_TIM_PWM_Stop_DMA(&htim2, TIM_CHANNEL_1);
}


uint8_t rainbow_effect(void) {                                            //rainbow effect mode
  // Strip ID: 0 - Effect: Rainbow - LEDS: 30
  // Steps: 100 - Delay: 20
  // Colors: 3 (255.0.0, 0.255.0, 0.0.255)
  // Options: rainbowlen=60, toLeft=true, 
  //  if(millis() - strip_0.effStart < 20 * (strip_0.effStep)) return 0x00;
  float factor1, factor2;
  uint16_t ind;
  for(uint16_t j=0;j<30;j++) {
    ind = (int16_t)(effStep + j * 1.6666666666666667);
    switch((int)((ind % 100) / 33.333333333333336)) {
      case 0: factor1 = 1.0 - ((float)(ind % 100 - 0 * 33.333333333333336) / 33.333333333333336);
              factor2 = (float)((int)(ind - 0) % 100) / 33.333333333333336;
              set_LED(j, 255 * factor1 + 0 * factor2, 0 * factor1 + 255 * factor2, 0 * factor1 + 0 * factor2);
              led_send();
              break;
      case 1: factor1 = 1.0 - ((float)(ind % 100 - 1 * 33.333333333333336) / 33.333333333333336);
              factor2 = (float)((int)(ind - 33.333333333333336) % 100) / 33.333333333333336;
              set_LED(j, 0 * factor1 + 0 * factor2, 255 * factor1 + 0 * factor2, 0 * factor1 + 255 * factor2);
              led_send();
              break;
      case 2: factor1 = 1.0 - ((float)(ind % 100 - 2 * 33.333333333333336) / 33.333333333333336);
              factor2 = (float)((int)(ind - 66.66666666666667) % 100) / 33.333333333333336;
              set_LED(j, 0 * factor1 + 255 * factor2, 0 * factor1 + 0 * factor2, 255 * factor1 + 0 * factor2);       
              led_send();
              break;
    }
  }
  if(effStep >= 100) {effStep = 0; return 0x03; }
  else effStep++;
  return 0x01;
}



void normal_mode(void){               //just white light. for different type of leds probably is needed different settings (giving it all 255 returned me greenish light)
  for(int i = 0; i<30; i++){
    set_LED(i, 255, 255, 105);
  }
  led_send();
}


int change_mode(void){                                 //changes the colors to rundom ones every second
  for(int i = 0; i < 30; i++){
    HAL_RNG_GenerateRandomNumber(&hrng, &gen);            
    temp1 = (int)(gen %256);
    HAL_RNG_GenerateRandomNumber(&hrng, &gen);
    temp2 = (int)(gen %256);
    HAL_RNG_GenerateRandomNumber(&hrng, &gen);
    temp3 = (int)(gen %256);
    set_LED(i, temp1, temp2, temp3);
  }
  led_send();
  ticks = HAL_GetTick();
  while(HAL_GetTick()-ticks <= 1000){                  //timer. if the button is pressed, you'll quit this mode
    if(!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13)){
      HAL_Delay(200);
      return 1;
    }
  }
  return 0;
}


int fade_mode(void){                              //make leds fade and light up again
  for (int i=0; i<45; i++){                       //increase the brightness
    set_Brightness(i);
		led_send();
    ticks = HAL_GetTick();
    while(HAL_GetTick()-ticks <= 10){             //timer. if the button is pressed, you'll quit this mode
      if(!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13))
        return 1;                                 //needed for increasing variable in main function to change the mode
      }
	  }

	for (int i=45; i>=0; i--){                       //decrease the brightness
    set_Brightness(i);
		led_send();
    ticks = HAL_GetTick();
    while(HAL_GetTick()-ticks <= 10){              //timer. if the button is pressed, you'll quit this mode
      if(!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13))
        return 1;
      }
  }
  return 0;                                        //needed for increasing variable in main function to change the mode
}


void set_rgb_lights(void){               //set rgb pattern on strip   
  for(int i = 0; i<=27; i+=3){
    set_LED(i, 255, 0, 0);
    set_LED(i+1, 0, 255, 0);
    set_LED(i+2, 0, 0, 255);
  }
  led_send();
}