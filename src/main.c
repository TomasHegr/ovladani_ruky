#include <stdbool.h>
#include <stm8s.h>
#include "stm8s_adc2.h" //knihovna pro převodník (asi?)
#include "adc_helper.h"
#include "main.h"
#include "milis.h"
//#include "delay.h"
#include "uart1.h"
#include <stdio.h>


#define PWM_PORT GPIOD
#define PWM_PIN  GPIO_PIN_4

#define ADC_PORT GPIOB
#define ADC_PIN GPIO_PIN_5
 
void init(void)
{
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);      // taktovani MCU na 16MHz
    init_milis();
    init_uart1();

    GPIO_Init(PWM_PORT, PWM_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
    GPIO_Init(ADC_PORT,ADC_PIN,GPIO_MODE_IN_FL_NO_IT);
    


    //inicializace timeru
    uint16_t hodnota= 500;

    TIM2_TimeBaseInit(TIM2_PRESCALER_16, 20000 - 1); //f= 50 HZ

    TIM2_OC1Init(TIM2_OCMODE_PWM1,
            TIM2_OUTPUTSTATE_ENABLE,    //povolení fyzického výstupu
            hodnota,            //počáíteční hodnota compare registru
            TIM2_OCPOLARITY_HIGH    //spínám log 1
        );

    TIM2_Cmd(ENABLE);
    TIM2_OC1PreloadConfig(ENABLE);



}
 //inicializace ADC převodníku
 void adc_init(void)
{
    // Pin - analog, vypnutí Schmittova klopného obvodu
    ADC2_SchmittTriggerConfig(ADC2_SCHMITTTRIG_CHANNEL14, DISABLE);

    // ADC Clock musí být 1MHz - 6MHz (16MHz / 4)
    ADC2_PrescalerConfig(ADC2_PRESSEL_FCPU_D4);

    // Zarovnání výsledku doprava
    ADC2_AlignConfig(ADC2_ALIGN_RIGHT);

    // Před spuštěním ADC vyberu kanál, který budu používat
    ADC2_Select_Channel(ADC2_CHANNEL_5);

    // Zapnutí ADC
    ADC2_Cmd(ENABLE);

    // Čas potřebný pro stabilizaci ADC
    ADC2_Startup_Wait();
}


 //rovný stav adc= 513
//maximálně ohnuto stav adc= 720


#define adc_rovny 580
#define adc_max_ohyb 820
#define servo_max 2500
#define servo_min 500

int main(void)
{
  
    uint32_t time = 0;
    uint16_t hodnota= 0;
    uint16_t adc_hodnota ;
    uint16_t pwm_hodnota;
    init();
    adc_init();

    while (1) {
        if (milis() - time > 100) {
            time = milis();
            adc_hodnota=ADC_get(ADC2_CHANNEL_5); // 0 - 1024

            //pwm_hodnota= 500 + ((uint32_t)adc_hodnota) * (2500-500)/1023; // kód pro staré senzoy
            

            //už nové senzory (funkční)
            // pro serva které mají hodnotu rovného prstu v 0˚

            //pwm_hodnota=500 +((int32_t)adc_hodnota - adc_rovny) *(servo_max-servo_min)/(adc_max_ohyb-adc_rovny);

            // pro serva které mají hodnotu rovného prstu ve 180 ˚

            pwm_hodnota=2200 -((int32_t)adc_hodnota - adc_rovny) *(servo_max-servo_min)/(adc_max_ohyb-adc_rovny);



            if(pwm_hodnota > 2200){
                pwm_hodnota=2200;
            }
            if(pwm_hodnota < 500){
                pwm_hodnota=500;
            }

            TIM2_SetCompare1(pwm_hodnota);
            
            printf("ADC: %u  ->  PWM: %u us\n", adc_hodnota, pwm_hodnota);
        }
        //delay_ms(333);
    }
}

/*-------------------------------  Assert -----------------------------------*/
#include "__assert__.h"
