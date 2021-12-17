#include <func.h>

int temp = 0;                              //variable for changing mods
int brightness = 5;                        //variable for changing brightness inside the program 
  
void func(void){     
  while(1){
    
    switch(temp){
      case 0:                              //turn of the lights
        set_Brightness(0);
        led_send();
        break;
      case 1:                              //just a light
        set_Brightness(brightness);
        normal_mode();
        break;
      case 2:                              //leds run with rainbow effect 
        set_Brightness(brightness);        
        rainbow_effect();
        break;
      case 3:                              //leds colors change every second 
        set_Brightness(brightness);
        temp+=change_mode();
        break;
      case 4:                              //rgb pattern   
        set_Brightness(brightness);         
        set_rgb_lights();
        break;
      case 5:                              //colors are fading and light up again
        temp+=fade_mode();
        break;
      default:                             //make temp variable suitable for switch cases
        temp = 0;
        break;
    }

    if(!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13)){    //read if the button is pressed. If it is then change the mode
      HAL_Delay(200);
      temp++;
    }
  }
}