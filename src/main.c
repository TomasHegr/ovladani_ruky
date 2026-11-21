#include <stdbool.h>
#include <stm8s.h>
#include "stm8s_adc2.h" //knihovna pro převodník (asi?)
#include "adc_helper.h"
#include "main.h"
#include "milis.h"
//#include "delay.h"
#include "uart1.h"
#include <stdio.h>


#define Prstenicek_pwm_port GPIOD
#define Prstenicek_pwm_pin  GPIO_PIN_4

#define ADC_prstenicek_port GPIOB
#define ADC_prstenicek_pin GPIO_PIN_5


#define Prostrednicek_pwm_port GPIOD
#define Prostrednicek_pwm_pin  GPIO_PIN_3

#define ADC_prostrednicek_port GPIOB
#define ADC_prostrednicek_pin GPIO_PIN_4

 
void init(void)
{
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);      // taktovani MCU na 16MHz
    init_milis();
    init_uart1();

    GPIO_Init(Prstenicek_pwm_port, Prstenicek_pwm_pin, GPIO_MODE_OUT_PP_LOW_SLOW);
    GPIO_Init(ADC_prstenicek_port,ADC_prstenicek_pin,GPIO_MODE_IN_FL_NO_IT);

    GPIO_Init(Prostrednicek_pwm_port, Prostrednicek_pwm_pin, GPIO_MODE_OUT_PP_LOW_SLOW);
    GPIO_Init(ADC_prostrednicek_port,ADC_prostrednicek_pin,GPIO_MODE_IN_FL_NO_IT);

    


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
 #define Prostrednicek ADC2_CHANNEL_4
 #define Prstenicek ADC2_CHANNEL_5
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
    ADC2_Select_Channel(Prstenicek);
    ADC2_Select_Channel(Prostrednicek);

    // Zapnutí ADC
    ADC2_Cmd(ENABLE);

    // Čas potřebný pro stabilizaci ADC
    ADC2_Startup_Wait();
}


 //rovný stav adc= 513
//maximálně ohnuto stav adc= 720


#define Prstenicek_rovny_adc 580
#define Prstenick_max_adc   820
#define servo_max 2500
#define servo_min 500

#define Prostrednicek_rovny_adc 447
#define Prostrednicek_max_adc 730


int main(void)
{
  
    uint32_t time = 0;
    uint16_t hodnota= 0;
    uint16_t adc_hodnota_prstenicek ;
    uint16_t adc_hodnota_prostrednicek;

    uint16_t pwm_hodnota_prstenicek;
    uint16_t pwm_hodnota_prostrednicek;
    init();
    adc_init();

    while (1) {
        if (milis() - time > 100) {
            time = milis();
            adc_hodnota_prstenicek=ADC_get(Prstenicek); // 0 - 1024
            adc_hodnota_prostrednicek =ADC_get(Prostrednicek);

            //pwm_hodnota= 500 + ((uint32_t)adc_hodnota) * (2500-500)/1023; // kód pro staré senzoy
            

            //už nové senzory (funkční)
            // pro serva které mají hodnotu rovného prstu v 0˚

            //pwm_hodnota=500 +((int32_t)adc_hodnota - adc_rovny) *(servo_max-servo_min)/(adc_max_ohyb-adc_rovny);

            // pro serva které mají hodnotu rovného prstu ve 180 ˚

            pwm_hodnota_prstenicek=2200 -
            ((int32_t)adc_hodnota_prstenicek - Prstenicek_rovny_adc) *(servo_max   -  servo_min)/(Prstenick_max_adc     -    Prstenicek_rovny_adc);



            

            pwm_hodnota_prostrednicek=500 +((int32_t)adc_hodnota_prostrednicek - Prostrednicek_rovny_adc) *(servo_max    -   servo_min)/(Prostrednicek_max_adc  -   Prostrednicek_rovny_adc);




            if(pwm_hodnota_prstenicek > 2200){
                pwm_hodnota_prstenicek=2200;
            }
            if(pwm_hodnota_prstenicek < 500){
                pwm_hodnota_prstenicek=500;
            }

            if (pwm_hodnota_prostrednicek > 2500) {
                pwm_hodnota_prostrednicek = 2500;
            }
            if (pwm_hodnota_prostrednicek < 500) {
                pwm_hodnota_prostrednicek = 500;
            }

            TIM2_SetCompare1(pwm_hodnota_prstenicek);  //pin PD4
            TIM2_SetCompare2(pwm_hodnota_prostrednicek);  //PIN PD3
            
            printf("ADC1: %u  ->  PWM1: %u us   ADC2: %u  ->  PWM2: %u us\n", adc_hodnota_prstenicek, pwm_hodnota_prstenicek,adc_hodnota_prostrednicek, pwm_hodnota_prostrednicek);
        }
        //delay_ms(333);
    }
}

/*-------------------------------  Assert -----------------------------------*/
#include "__assert__.h"