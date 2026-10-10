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

#define PWM2_PORT GPIOD
#define PWM2_PIN  GPIO_PIN_3

#define ADC_PORT GPIOB
#define ADC_PIN GPIO_PIN_5
 

#define ADC2_PORT GPIOB
#define ADC2_PIN GPIO_PIN_4
void init(void)
{
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);      // taktovani MCU na 16MHz
    init_milis();
    init_uart1();

    GPIO_Init(PWM_PORT, PWM_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
    GPIO_Init(ADC_PORT,ADC_PIN,GPIO_MODE_IN_FL_NO_IT);
    
 GPIO_Init(PWM2_PORT, PWM2_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
    GPIO_Init(ADC2_PORT,ADC2_PIN,GPIO_MODE_IN_FL_NO_IT);
    

    //inicializace timeru
    uint16_t hodnota= 500;

    TIM2_TimeBaseInit(TIM2_PRESCALER_16, 20000 - 1); //f= 50 HZ

    //Inicializace timeru pro PWM - servo 1
    TIM2_OC1Init(TIM2_OCMODE_PWM1,
            TIM2_OUTPUTSTATE_ENABLE,    //povolení fyzického výstupu
            hodnota,            //počáíteční hodnota compare registru
            TIM2_OCPOLARITY_HIGH    //spínám log 1
        );

    TIM2_Cmd(ENABLE);
    TIM2_OC1PreloadConfig(ENABLE);


    //inicializace timeru pro PWM2 -servo 2
    TIM2_OC2Init(TIM2_OCMODE_PWM1, TIM2_OUTPUTSTATE_ENABLE, hodnota, TIM2_OCPOLARITY_HIGH);
    TIM2_OC2PreloadConfig(ENABLE);


   



}
 //inicializace ADC převodníku
 
void ADC_init(void){
// na pinech/vstupech ADC_IN2 (PB2) a ADC_IN3 (PB3) vypneme vstupní buffer
ADC2_SchmittTriggerConfig(ADC2_SCHMITTTRIG_CHANNEL5,DISABLE); //pro ADC - servo 1
ADC2_SchmittTriggerConfig(ADC2_SCHMITTTRIG_CHANNEL4, DISABLE); // pro ADC2 - servo 2

//ADC2_SchmittTriggerConfig(ADC2_SCHMITTTRIG_CHANNEL3,DISABLE);
// nastavíme clock pro ADC (16MHz / 4 = 4MHz)
ADC2_PrescalerConfig(ADC2_PRESSEL_FCPU_D4);

// volíme zarovnání výsledku (typicky vpravo, jen vyjmečně je výhodné vlevo)
ADC2_AlignConfig(ADC2_ALIGN_RIGHT);

// nasatvíme multiplexer na některý ze vstupních kanálů
ADC2_Select_Channel(ADC2_CHANNEL_5);


// rozběhneme AD převodník
ADC2_Cmd(ENABLE);


}

uint16_t adc_reader(ADC2_Channel_TypeDef kanal){

    // přepnout multiplexer na zvolený kanál
   ADC2->CSR = (ADC2->CSR & ~ADC2_CSR_CH) | kanal;

      ADC2_StartConversion();
    while (ADC2_GetFlagStatus() == RESET);
    ADC2_ClearFlag();


    ADC2_StartConversion();
    while (ADC2_GetFlagStatus() == RESET);
    uint16_t vysledek = ADC2_GetConversionValue();
    ADC2_ClearFlag();
    return vysledek;

    /*
    
  
    ADC2_StartConversion();
    while (ADC2_GetFlagStatus() == RESET);   
    ADC2_ClearFlag();
    return ADC2_GetConversionValue();        // 0–1023 
    
     */
}
int main(void)
{
  
    uint32_t time = 0;
    uint16_t hodnota= 0;
    uint16_t adc_hodnota ; //první potak
    uint16_t pwm_hodnota;   //první servo

    uint16_t adc_hodnota2;   // druhý potenciometr
    uint16_t pwm_hodnota2;   // druhé servo
    init();
    ADC_init();

    while (1) {
        if (milis() - time > 100) {
            time = milis();

            //PRVNÍ SERVO A POTENCIOMETR
            adc_hodnota=adc_reader(ADC2_CHANNEL_5);
            pwm_hodnota= 500 + ((uint32_t)adc_hodnota) * (2500-500)/1023;

            TIM2_SetCompare1(pwm_hodnota);

            //druhé servo a potenciometr
            adc_hodnota2 = adc_reader(ADC2_CHANNEL_4);
            pwm_hodnota2 = 500 + ((uint32_t)adc_hodnota2) * (2500 - 500) / 1023;
            TIM2_SetCompare2(pwm_hodnota2);

            
            //printf("ADC: %u  ->  PWM: %u us\n", adc_hodnota, pwm_hodnota); // print jen pro jedno servo a potenciometr

            printf("S1: ADC %u -> %u us | S2: ADC %u -> %u us\n", adc_hodnota, pwm_hodnota, adc_hodnota2, pwm_hodnota2); // print pro 2 serva a 2 potenciometry
        }
        //delay_ms(333);
    }
}

/*-------------------------------  Assert -----------------------------------*/
#include "__assert__.h"
