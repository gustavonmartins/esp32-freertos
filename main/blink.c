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
#include <driver/ledc.h>

/* Can use project configuration menu (idf.py menuconfig) to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/
#define MOTOR 2
#define LED 4



// PWM

void pwmtask(void* ignore){
    ledc_timer_config_t timer_conf;
    timer_conf.freq_hz=50;
    timer_conf.duty_resolution=LEDC_TIMER_10_BIT;
    timer_conf.timer_num=LEDC_TIMER_0;
    timer_conf.speed_mode=LEDC_HIGH_SPEED_MODE;

    ledc_timer_config(&timer_conf);

    int duty=500;// 20/1024*62=1,25 ms

    ledc_channel_config_t ledc_conf;
    ledc_conf.channel=LEDC_CHANNEL_0;
    ledc_conf.duty=duty; 
    ledc_conf.gpio_num=MOTOR;
    ledc_conf.intr_type=LEDC_INTR_DISABLE;
    ledc_conf.speed_mode=LEDC_HIGH_SPEED_MODE;
    ledc_conf.timer_sel=LEDC_TIMER_0;
    ledc_channel_config(&ledc_conf);

    
    int step=100;
    while(1){
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, duty);
	ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
    
    duty+=step;
    if ( (duty <=0) || (duty >= 900)) {
        step=-step;
    }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        printf("PWM here: %d\n",duty);
    }
}

//END PWM

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
    //LEDParams l1={        .led=MOTOR,    .ticks_on=2000,    .ticks_off=2000,.reverted=true};
    LEDParams l2={        .led=LED,    .ticks_on=2000,    .ticks_off=2000,.reverted=false};
    printf("portTICK_PERIOD_MS: %d\n",portTICK_PERIOD_MS);
 
   //xTaskCreate(task1,"MOTOR",1024*2,&l1,5,NULL);
   //xTaskCreatePinnedToCore(pwmtask,"MOTOR",1024*5,NULL,5,NULL,0);
   xTaskCreatePinnedToCore(task1,"LED",1024*5,&l2,5,NULL,0);
    
   while(true){
       vTaskDelay(1000/portTICK_PERIOD_MS);
   }
}