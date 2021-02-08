/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

/* Can use project configuration menu (idf.py menuconfig) to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/
#define MOTOR 2
#define LED 4

typedef struct {
    int led;
    int ticks_on;
    int ticks_off;
    bool reverted;
} LEDParams;



void task1(void* params){

    LEDParams *p = (LEDParams*) params;

    gpio_pad_select_gpio(p->led);
    gpio_set_direction(p->led,GPIO_MODE_OUTPUT);

    int on=(p->reverted) ? 0 : 1;
    int off=(on==1) ? 0:1;

    while(1){
        printf("Turning on port %d\n",p->led);
        gpio_set_level(p->led, on);
        vTaskDelay(p->ticks_on / portTICK_PERIOD_MS);
        printf("Turning off port %d\n",p->led);
        gpio_set_level(p->led, off);
        vTaskDelay(p->ticks_off / portTICK_PERIOD_MS);
    }
}


void app_main()
{
    /* Configure the IOMUX register for pad MOTOR (some pads are
       muxed to GPIO on reset already, but some default to other
       functions and need to be switched to GPIO. Consult the
       Technical Reference for a list of pads and their default
       functions.)
    */
    LEDParams l1={        .led=MOTOR,    .ticks_on=2000,    .ticks_off=2000,.reverted=true};
    LEDParams l2={        .led=LED,    .ticks_on=500,    .ticks_off=500,.reverted=false};
    printf("portTICK_PERIOD_MS: %d\n",portTICK_PERIOD_MS);
 
   xTaskCreate(task1,"MOTOR",1024*2,&l1,5,NULL);
   xTaskCreate(task1,"LED",1024*2,&l2,5,NULL);
    
   while(true){
       vTaskDelay(1000/portTICK_PERIOD_MS);
   }
}