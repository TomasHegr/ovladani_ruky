#include <stdbool.h>
#include <stm8s.h>
#include "stm8s_adc2.h" //knihovna pro převodník (asi?)
#include "adc_helper.h"
#include "main.h"
#include "milis.h"
//#include "delay.h"
#include "uart1.h"
#include <stdio.h>


#define PWM_PORT1 GPIOD
#define PWM_PIN1  GPIO_PIN_4

#define ADC_PORT1 GPIOB
#define ADC_PIN1 GPIO_PIN_5


#define PWM_PORT2 GPIOD
#define PWM_PIN2  GPIO_PIN_3

#define ADC_PORT2 GPIOB
#define ADC_PIN2 GPIO_PIN_4

 
void init(void)
{
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);      // taktovani MCU na 16MHz
    init_milis();
    init_uart1();

    GPIO_Init(PWM_PORT1, PWM_PIN1, GPIO_MODE_OUT_PP_LOW_SLOW);
    GPIO_Init(ADC_PORT1,ADC_PIN1,GPIO_MODE_IN_FL_NO_IT);

    GPIO_Init(PWM_PORT2, PWM_PIN2, GPIO_MODE_OUT_PP_LOW_SLOW);
    GPIO_Init(ADC_PORT2,ADC_PIN2,GPIO_MODE_IN_FL_NO_IT);

    


    //inicializace timeru
    uint16_t hodnota= 500;

    TIM2_TimeBaseInit(TIM2_PRESCALER_16, 20000 - 1); //f= 50 HZ

    //pwm první kanál

    TIM2_OC1Init(TIM2_OCMODE_PWM1,
            TIM2_OUTPUTSTATE_ENABLE,    //povolení fyzického výstupu
            2200,            //počáíteční hodnota compare registru
            TIM2_OCPOLARITY_HIGH    //spínám log 1
        );

        //pwm druhý kanál

        TIM2_OC2Init(TIM2_OCMODE_PWM1,
            TIM2_OUTPUTSTATE_ENABLE,    //povolení fyzického výstupu
            hodnota,            //počáíteční hodnota compare registru
            TIM2_OCPOLARITY_HIGH    //spínám log 1
        );
    

    TIM2_Cmd(ENABLE);
    TIM2_OC1PreloadConfig(ENABLE);
    TIM2_OC2PreloadConfig(ENABLE);



}
 //inicializace ADC převodníku
 void adc_init(void)
{
    // Pin - analog, vypnutí Schmittova klopného obvodu
    ADC2_SchmittTriggerConfig(ADC2_SCHMITTTRIG_CHANNEL5, DISABLE);

    ADC2_SchmittTriggerConfig(ADC2_SCHMITTTRIG_CHANNEL4, DISABLE);

    // ADC Clock musí být 1MHz - 6MHz (16MHz / 4)
    ADC2_PrescalerConfig(ADC2_PRESSEL_FCPU_D4);

    // Zarovnání výsledku doprava
    ADC2_AlignConfig(ADC2_ALIGN_RIGHT);

    // Před spuštěním ADC vyberu kanál, který budu používat
    ADC2_Select_Channel(ADC2_CHANNEL_5);
    ADC2_Select_Channel(ADC2_CHANNEL_4);

    // Zapnutí ADC
    ADC2_Cmd(ENABLE);

    // Čas potřebný pro stabilizaci ADC
    ADC2_Startup_Wait();
}


 //rovný stav adc= 513
//maximálně ohnuto stav adc= 720


#define adc_rovny1 580
#define adc_max_ohyb1   820
#define servo_max 2500
#define servo_min 500

#define adc_rovny2 447
#define adc_max_ohyb2 730


int main(void)
{
  
    uint32_t time = 0;
    uint16_t hodnota= 0;
    uint16_t adc_hodnota_prvni_k ;
    uint16_t adc_hodnota_druhy_k;

    uint16_t pwm_hodnota_prvni_k;
    uint16_t pwm_hodnota_druhy_k;
    init();
    adc_init();

    while (1) {
        if (milis() - time > 100) {
            time = milis();
            adc_hodnota_prvni_k=ADC_get(ADC2_CHANNEL_5); // 0 - 1024
            adc_hodnota_druhy_k =ADC_get(ADC2_CHANNEL_4);

            //pwm_hodnota= 500 + ((uint32_t)adc_hodnota) * (2500-500)/1023; // kód pro staré senzoy
            

            //už nové senzory (funkční)
            // pro serva které mají hodnotu rovného prstu v 0˚

            //pwm_hodnota=500 +((int32_t)adc_hodnota - adc_rovny) *(servo_max-servo_min)/(adc_max_ohyb-adc_rovny);

            // pro serva které mají hodnotu rovného prstu ve 180 ˚

            pwm_hodnota_prvni_k=2200 -((int32_t)adc_hodnota_prvni_k - adc_rovny1) *(servo_max   -  servo_min)/(adc_max_ohyb1     -    adc_rovny1);



            //druhý senzor rovný stav 425
            //druhý senzor ohnutý stav  780
            pwm_hodnota_druhy_k=500 +((int32_t)adc_hodnota_druhy_k - adc_rovny2) *(servo_max    -   servo_min)/(adc_max_ohyb2  -   adc_rovny2);




            if(pwm_hodnota_prvni_k > 2200){
                pwm_hodnota_prvni_k=2200;
            }
            if(pwm_hodnota_prvni_k < 500){
                pwm_hodnota_prvni_k=500;
            }

            if (pwm_hodnota_druhy_k > 2500) {
                pwm_hodnota_druhy_k = 2500;
            }
            if (pwm_hodnota_druhy_k < 500) {
                pwm_hodnota_druhy_k = 500;
            }

            TIM2_SetCompare1(pwm_hodnota_prvni_k);  //pin PD4
            TIM2_SetCompare2(pwm_hodnota_druhy_k);  //PIN PD3
            
            printf("ADC1: %u  ->  PWM1: %u us   ADC2: %u  ->  PWM2: %u us\n", adc_hodnota_prvni_k, pwm_hodnota_prvni_k,adc_hodnota_druhy_k, pwm_hodnota_druhy_k);
        }
        //delay_ms(333);
    }
}

/*-------------------------------  Assert -----------------------------------*/
#include "__assert__.h"
