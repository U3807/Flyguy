/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include "main_functions.h"
#include "hx_drv_tflm.h"
#include "synopsys_wei_gpio.h"
#include "synopsys_wei_delay.h"

// This is the default main used on systems that have the standard C entry
// point. Other devices (for example FreeRTOS or ESP32) that have different
// requirements for entry code (like an app_main function) should specialize
// this main.cc file in a target-specific subfolder.
hx_drv_gpio_config_t hal_gpio_0;
hx_drv_gpio_config_t hal_gpio_1;
hx_drv_gpio_config_t hal_gpio_2;
hx_drv_gpio_config_t hal_led_r;
hx_drv_gpio_config_t hal_led_g;
hx_drv_sensor_image_config_t pimg_config;
#define image_rowsize 85
#define image_colsize 85
#define delay_time 3

void GPIO_INIT(void)
{
  if(hal_gpio_init(&hal_gpio_0, HX_DRV_PGPIO_0, HX_DRV_GPIO_INPUT, GPIO_PIN_RESET) != HAL_OK)
    hx_drv_uart_print("GPIO0 Initialized: Error\n");

  if(hal_gpio_init(&hal_gpio_1, HX_DRV_PGPIO_1, HX_DRV_GPIO_INPUT, GPIO_PIN_RESET) != HAL_OK)
    hx_drv_uart_print("GPIO1 Initialized: Error\n");

  if(hal_gpio_init(&hal_gpio_2, HX_DRV_PGPIO_2, HX_DRV_GPIO_INPUT, GPIO_PIN_RESET) != HAL_OK)
    hx_drv_uart_print("GPIO2 Initialized: Error\n");
}

void GPIO_INIT_OUT(void)
{
  if(hal_gpio_init(&hal_gpio_0, HX_DRV_PGPIO_0, HX_DRV_GPIO_OUTPUT, GPIO_PIN_RESET) == HAL_OK)
    hx_drv_uart_print("GPIO0 OUT Initialized: OK\n");
  else
    hx_drv_uart_print("GPIO0 OUT Initialized: Error\n");

  if(hal_gpio_init(&hal_gpio_1, HX_DRV_PGPIO_1, HX_DRV_GPIO_OUTPUT, GPIO_PIN_RESET) == HAL_OK)
    hx_drv_uart_print("GPIO1 OUT Initialized: OK\n");
  else
    hx_drv_uart_print("GPIO1 OUT Initialized: Error\n");

  if(hal_gpio_init(&hal_gpio_2, HX_DRV_PGPIO_2, HX_DRV_GPIO_OUTPUT, GPIO_PIN_RESET) == HAL_OK)
    hx_drv_uart_print("GPIO2 OUT Initialized: OK\n");
  else
    hx_drv_uart_print("GPIO2 OUT Initialized: Error\n");
}



 int8_t img2_arr[image_rowsize*image_colsize]={0};
 int8_t *img2_ptr;
volatile  int pic_result =0;
volatile  int gesture_result =0;
volatile  int send =0;



int main(int argc, char* argv[]) {
  uint8_t gpio0_data;
  uint8_t gpio1_data;
  uint8_t gpio2_data;
  hx_drv_uart_initial(UART_BR_115200);
  gesture_setup();
  pic_setup();
  if(hx_drv_sensor_initial(&pimg_config) == HX_DRV_LIB_PASS)
	 	  hx_drv_uart_print("Camera Initialize Successful\n");

//
while(1)
{
  //TRANSFER GPIO IN
  GPIO_INIT();
  
  hal_gpio_get(&hal_gpio_0, &gpio0_data);
  hal_gpio_get(&hal_gpio_1, &gpio1_data);
  hal_gpio_get(&hal_gpio_2, &gpio2_data);

  //if(gpio0_data == 0)
  //  hx_drv_uart_print("GPIO_0 Logic: Low\n\n");
  //else
  //  hx_drv_uart_print("GPIO_0 Logic: High\n\n");
//
  //if(gpio1_data == 0)
  //  hx_drv_uart_print("GPIO_1 Logic: Low\n\n");
  //else
  //  hx_drv_uart_print("GPIO_1 Logic: High\n\n");
//
  //if(gpio2_data == 0)
  //  hx_drv_uart_print("GPIO_2 Logic: Low\n\n");
  //else
  //  hx_drv_uart_print("GPIO_2 Logic: High\n\n");     
 //Take pic Enable 1 0 1
 if(gpio0_data!=0 && gpio1_data==0 && gpio2_data!=0)
  {
      GPIO_INIT_OUT();
      hal_gpio_set(&hal_gpio_0, GPIO_PIN_RESET);
      hal_gpio_set(&hal_gpio_1, GPIO_PIN_RESET);
      hal_gpio_set(&hal_gpio_2, GPIO_PIN_RESET);
      hx_drv_uart_print("Mode Take_pic\n");     
      //Take Pixtures
			img2_ptr=&img2_arr[0];
			hx_drv_sensor_capture(&pimg_config);
			hx_drv_image_rescale((uint8_t*)pimg_config.raw_address,pimg_config.img_width, pimg_config.img_height,img2_ptr, image_colsize, image_rowsize);
      
      //INVOKE
      pic_result =pic_loop();
    
      //USING GPIO TO SEND DATA
      if(pic_result == 1 )
       {
         for(int i =0;i<image_rowsize;i++)
         {
          for(int j =0;j<image_colsize;j++)
          {
            hx_drv_uart_print("%3d",img2_arr[i*image_rowsize+j]);
          }
            hx_drv_uart_print("\n");
         }

         hx_drv_uart_print("gpio start send\n");
          //Change GPIO from input to output
         hal_gpio_set(&hal_gpio_2, GPIO_PIN_SET);
        //hal_delay_ms(delay_time);
         
        for(uint32_t heigth_cnt = 0; heigth_cnt < image_rowsize; heigth_cnt ++)
		      {
		         for(uint32_t width_cnt = 0; width_cnt < image_colsize; width_cnt ++)
		         {
               for (int8_t i = 0; i < 8; i++)
               {
                 int8_t trans_bit = ((1 << (7 - i)) & *img2_ptr) >> (7 - i);
                 if (trans_bit == 1)
                 {
                   hal_gpio_set(&hal_gpio_0, GPIO_PIN_SET);
                   hal_gpio_set(&hal_gpio_1, GPIO_PIN_RESET);
                   hal_delay_ms(delay_time);
                   hal_gpio_set(&hal_gpio_1, GPIO_PIN_SET);
                   hal_delay_ms(delay_time);
                 }
                 else
                 {
                   hal_gpio_set(&hal_gpio_0, GPIO_PIN_RESET);
                   hal_gpio_set(&hal_gpio_1, GPIO_PIN_RESET);
                   hal_delay_ms(delay_time);
                   hal_gpio_set(&hal_gpio_1, GPIO_PIN_SET);
                   hal_delay_ms(delay_time);
                 }
               }
               //hx_drv_uart_print("CHECK %3d\n", *img2_ptr);
		         	img2_ptr = img2_ptr + 1;
		         }
           }
         hal_gpio_set(&hal_gpio_2, GPIO_PIN_RESET);
         hal_gpio_set(&hal_gpio_0, GPIO_PIN_RESET);
         hal_gpio_set(&hal_gpio_1, GPIO_PIN_RESET);
         
         hx_drv_uart_print("SEND ALL \n");         
       }
       else{
        hal_gpio_set(&hal_gpio_0, GPIO_PIN_SET);
        hal_gpio_set(&hal_gpio_1, GPIO_PIN_SET);
        hal_gpio_set(&hal_gpio_2, GPIO_PIN_RESET);
        hal_delay_ms(100);
         hx_drv_uart_print("NO person found \n");       
       }
  }
  
  // 0 1 1 gesture
  else if(gpio0_data==0 && gpio1_data!=0 && gpio2_data!=0)
  {
      //hal_delay_ms(100);
      GPIO_INIT_OUT();
      hal_gpio_set(&hal_gpio_0, GPIO_PIN_RESET);
      hal_gpio_set(&hal_gpio_1, GPIO_PIN_RESET);
      hal_gpio_set(&hal_gpio_2, GPIO_PIN_RESET);
      hx_drv_uart_print("Mode Gesture\n");  
       //INVOKE
      gesture_result =gesture_loop();  

      hal_gpio_set(&hal_gpio_2, GPIO_PIN_SET);
      
      //up 001
      if(gesture_result==0){
          //0
          
          hal_gpio_set(&hal_gpio_0, GPIO_PIN_RESET);
          hal_gpio_set(&hal_gpio_1, GPIO_PIN_RESET);
          hal_delay_ms(delay_time);
          hal_gpio_set(&hal_gpio_1, GPIO_PIN_SET);
          hal_delay_ms(delay_time);
          //0
          hal_gpio_set(&hal_gpio_0, GPIO_PIN_RESET);
          hal_gpio_set(&hal_gpio_1, GPIO_PIN_RESET);
          hal_delay_ms(delay_time);
          hal_gpio_set(&hal_gpio_1, GPIO_PIN_SET);
          hal_delay_ms(delay_time);
          //1
          hal_gpio_set(&hal_gpio_0, GPIO_PIN_SET);
          hal_gpio_set(&hal_gpio_1, GPIO_PIN_RESET);
          hal_delay_ms(delay_time);
          hal_gpio_set(&hal_gpio_1, GPIO_PIN_SET);
          hal_delay_ms(delay_time);
      }
      //down 010
      else if(gesture_result==1){
         //0
          hal_gpio_set(&hal_gpio_0, GPIO_PIN_RESET);
          hal_gpio_set(&hal_gpio_1, GPIO_PIN_RESET);
          hal_delay_ms(delay_time);
          hal_gpio_set(&hal_gpio_1, GPIO_PIN_SET);
         hal_delay_ms(delay_time);
          //1
          hal_gpio_set(&hal_gpio_0, GPIO_PIN_SET);
          hal_gpio_set(&hal_gpio_1, GPIO_PIN_RESET);
          hal_delay_ms(delay_time);
          hal_gpio_set(&hal_gpio_1, GPIO_PIN_SET);
        hal_delay_ms(delay_time);
          //0
          hal_gpio_set(&hal_gpio_0, GPIO_PIN_RESET);
          hal_gpio_set(&hal_gpio_1, GPIO_PIN_RESET);
          hal_delay_ms(delay_time);
          hal_gpio_set(&hal_gpio_1, GPIO_PIN_SET);
         hal_delay_ms(delay_time);
      }
      //left 011
      else if(gesture_result==2){
         //0
          hal_gpio_set(&hal_gpio_0, GPIO_PIN_RESET);
          hal_gpio_set(&hal_gpio_1, GPIO_PIN_RESET);
        hal_delay_ms(delay_time);
          hal_gpio_set(&hal_gpio_1, GPIO_PIN_SET);
         hal_delay_ms(delay_time);
          //1
          hal_gpio_set(&hal_gpio_0, GPIO_PIN_SET);
          hal_gpio_set(&hal_gpio_1, GPIO_PIN_RESET);
         hal_delay_ms(delay_time);
          hal_gpio_set(&hal_gpio_1, GPIO_PIN_SET);
          hal_delay_ms(delay_time);
          //1
          hal_gpio_set(&hal_gpio_0, GPIO_PIN_SET);
          hal_gpio_set(&hal_gpio_1, GPIO_PIN_RESET);
         hal_delay_ms(delay_time);
          hal_gpio_set(&hal_gpio_1, GPIO_PIN_SET);
          hal_delay_ms(delay_time);
      }
      //right 100
      else if(gesture_result==3){
          //1
          hal_gpio_set(&hal_gpio_0, GPIO_PIN_SET);
          hal_gpio_set(&hal_gpio_1, GPIO_PIN_RESET);
         hal_delay_ms(delay_time);
          hal_gpio_set(&hal_gpio_1, GPIO_PIN_SET);
          hal_delay_ms(delay_time);
         //0
          hal_gpio_set(&hal_gpio_0, GPIO_PIN_RESET);
          hal_gpio_set(&hal_gpio_1, GPIO_PIN_RESET);
          hal_delay_ms(delay_time);
          hal_gpio_set(&hal_gpio_1, GPIO_PIN_SET);
         hal_delay_ms(delay_time);
          //0
          hal_gpio_set(&hal_gpio_0, GPIO_PIN_RESET);
          hal_gpio_set(&hal_gpio_1, GPIO_PIN_RESET);
          hal_delay_ms(delay_time);
          hal_gpio_set(&hal_gpio_1, GPIO_PIN_SET);
          hal_delay_ms(delay_time);
      }
      //nothing 101
      else if(gesture_result==4){
          //1
          hal_gpio_set(&hal_gpio_0, GPIO_PIN_SET);
          hal_gpio_set(&hal_gpio_1, GPIO_PIN_RESET);
          hal_delay_ms(delay_time);
          hal_gpio_set(&hal_gpio_1, GPIO_PIN_SET);
          hal_delay_ms(delay_time);
         //0
          hal_gpio_set(&hal_gpio_0, GPIO_PIN_RESET);
          hal_gpio_set(&hal_gpio_1, GPIO_PIN_RESET);
          hal_delay_ms(delay_time);
          hal_gpio_set(&hal_gpio_1, GPIO_PIN_SET);
          hal_delay_ms(delay_time);
          //1
          hal_gpio_set(&hal_gpio_0, GPIO_PIN_SET);
          hal_gpio_set(&hal_gpio_1, GPIO_PIN_RESET);
          hal_delay_ms(delay_time);
          hal_gpio_set(&hal_gpio_1, GPIO_PIN_SET);
          hal_delay_ms(delay_time);
      }
      hal_delay_ms(delay_time);
      hal_gpio_set(&hal_gpio_0, GPIO_PIN_RESET);
      hal_gpio_set(&hal_gpio_1, GPIO_PIN_RESET);
      hal_gpio_set(&hal_gpio_2, GPIO_PIN_RESET);
  }
  
}
  
}