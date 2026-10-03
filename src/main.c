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
 
void ADC_init(void){
// na pinech/vstupech ADC_IN2 (PB2) a ADC_IN3 (PB3) vypneme vstupní buffer
ADC2_SchmittTriggerConfig(ADC2_SCHMITTTRIG_CHANNEL5,DISABLE);
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

uint16_t adc_reader(void){
    ADC2_StartConversion();
    while (ADC2_GetFlagStatus() == RESET);   
    ADC2_ClearFlag();
    return ADC2_GetConversionValue();        // 0–1023
}
int main(void)
{
  
    uint32_t time = 0;
    uint16_t hodnota= 0;
    uint16_t adc_hodnota ;
    uint16_t pwm_hodnota;
    init();
    ADC_init();

    while (1) {
        if (milis() - time > 100) {
            time = milis();
            adc_hodnota=adc_reader();
            pwm_hodnota= 500 + ((uint32_t)adc_hodnota) * (2500-500)/1023;

            TIM2_SetCompare1(pwm_hodnota);
            
            printf("ADC: %u  ->  PWM: %u us\n", adc_hodnota, pwm_hodnota);
        }
        //delay_ms(333);
    }
}

/*-------------------------------  Assert -----------------------------------*/
#include "__assert__.h"
