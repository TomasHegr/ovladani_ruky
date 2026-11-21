#include <stdbool.h>
#include <stm8s.h>
#include "stm8s_adc2.h" //knihovna pro převodník (asi?)
#include "adc_helper.h"
#include "main.h"
#include "milis.h"
//#include "delay.h"
#include "uart1.h"
#include <stdio.h>

//makra pro prsty - pwm a adc
#define Prstenicek_pwm_port GPIOD
#define Prstenicek_pwm_pin  GPIO_PIN_4

#define ADC_prstenicek_port GPIOB
#define ADC_prstenicek_pin GPIO_PIN_5


#define Prostrednicek_pwm_port GPIOD
#define Prostrednicek_pwm_pin  GPIO_PIN_3

#define ADC_prostrednicek_port GPIOB
#define ADC_prostrednicek_pin GPIO_PIN_4

#define Ukazovacek_pwm_port GPIOA
#define Ukazovacek_pwm_pin GPIO_PIN_3

#define ADC_ukazovacek_port  GPIOB
#define ADC_ukazovacek_pin GPIO_PIN_3

#define Palec_pwm_port GPIOD
#define Palec_pwm_pin GPIO_PIN_2

#define ADC_palec_port GPIOB
#define ADC_palec_pin   GPIO_PIN_2

#define Malicek_pwm_port GPIOD
#define Malicek_pwm_pin GPIO_PIN_0

#define ADC_malicek_port GPIOB
#define ADC_malicek_pin GPIO_PIN_1


//makro pro tlačítko na přepínaní gest
#define GESTO_PORT   GPIOB
#define GESTO_PIN    GPIO_PIN_6


//dočasné použití tlačítka na stm8 nucleo kitu
#define BTN_PORT GPIOB
#define BTN_PIN  GPIO_PIN_7

 
void init(void)
{
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);      // taktovani MCU na 16MHz
    init_milis();
    init_uart1();
    //tlačítko pro režimy a gesta
    GPIO_Init(BTN_PORT, BTN_PIN, GPIO_MODE_IN_PU_NO_IT);
    GPIO_Init(GESTO_PORT, GESTO_PIN, GPIO_MODE_IN_PU_NO_IT);

    //Inicializace pinů pro prsty
    GPIO_Init(Prstenicek_pwm_port, Prstenicek_pwm_pin, GPIO_MODE_OUT_PP_LOW_SLOW);
    GPIO_Init(ADC_prstenicek_port,ADC_prstenicek_pin,GPIO_MODE_IN_FL_NO_IT);

    GPIO_Init(Prostrednicek_pwm_port, Prostrednicek_pwm_pin, GPIO_MODE_OUT_PP_LOW_SLOW);
    GPIO_Init(ADC_prostrednicek_port,ADC_prostrednicek_pin,GPIO_MODE_IN_FL_NO_IT);

    GPIO_Init(Ukazovacek_pwm_port,Ukazovacek_pwm_pin,GPIO_MODE_OUT_PP_LOW_SLOW);
    GPIO_Init(ADC_ukazovacek_port,ADC_ukazovacek_pin,GPIO_MODE_IN_FL_NO_IT);

    GPIO_Init(Palec_pwm_port,Palec_pwm_pin,GPIO_MODE_OUT_PP_LOW_SLOW);
    GPIO_Init(ADC_palec_port,ADC_palec_pin,GPIO_MODE_IN_FL_NO_IT);

    GPIO_Init(Malicek_pwm_port,Malicek_pwm_pin,GPIO_MODE_OUT_PP_LOW_SLOW);
    GPIO_Init(ADC_malicek_port,ADC_malicek_pin,GPIO_MODE_IN_FL_NO_IT);
    
    


    //inicializace timeru - TIM2 -> 3 kanály
   

    TIM2_TimeBaseInit(TIM2_PRESCALER_16, 20000 - 1); //f= 50 HZ

        //pwm první kanál - prsteníček

        TIM2_OC1Init(TIM2_OCMODE_PWM1,
            TIM2_OUTPUTSTATE_ENABLE,    //povolení fyzického výstupu
            2200,            //počáíteční hodnota compare registru
            TIM2_OCPOLARITY_HIGH    //spínám log 1
        );

        //pwm druhý kanál - prostředníček

        TIM2_OC2Init(TIM2_OCMODE_PWM1,
            TIM2_OUTPUTSTATE_ENABLE,    
            500,            
            TIM2_OCPOLARITY_HIGH    
        );

        //pwm třetí kanál - ukazováček
        TIM2_OC3Init(TIM2_OCMODE_PWM1,
            TIM2_OUTPUTSTATE_ENABLE,    
            500,            
            TIM2_OCPOLARITY_HIGH    
        );

    
        //inicializace tim3 -> 2 kanály

         TIM3_TimeBaseInit(TIM3_PRESCALER_16, 20000 - 1); // opět navrhnuto pro frekvenci 50 Hz

         //první pwm kanál timeru 3 - palec
         TIM3_OC1Init(TIM3_OCMODE_PWM1,
            TIM3_OUTPUTSTATE_ENABLE,    
            500,            
            TIM3_OCPOLARITY_HIGH    
        );

        
        //druhý pwm kanál timeru 3 - malíček
         TIM3_OC2Init(TIM3_OCMODE_PWM1,
            TIM3_OUTPUTSTATE_ENABLE,    
            2200,            
            TIM3_OCPOLARITY_HIGH    
        );

    
    //timer 2 povolení
    TIM2_Cmd(ENABLE);

    TIM2_OC1PreloadConfig(ENABLE);
    TIM2_OC2PreloadConfig(ENABLE);
    TIM2_OC3PreloadConfig(ENABLE);

    //timer 3 povolení
    TIM3_Cmd(ENABLE);

    TIM3_OC1PreloadConfig(ENABLE);
    TIM3_OC2PreloadConfig(ENABLE);



}
 //inicializace ADC převodníku
 #define Prostrednicek ADC2_CHANNEL_4
 #define Prstenicek ADC2_CHANNEL_5
 #define Ukazovacek ADC2_CHANNEL_3
 #define Palec ADC2_CHANNEL_2

 #define Malicek ADC2_CHANNEL_1
 void adc_init(void)
{
    // vypnutí Schmittova klopného obvodu
    ADC2_SchmittTriggerConfig(ADC2_SCHMITTTRIG_CHANNEL5, DISABLE);

    ADC2_SchmittTriggerConfig(ADC2_SCHMITTTRIG_CHANNEL4, DISABLE);

    ADC2_SchmittTriggerConfig(ADC2_SCHMITTTRIG_CHANNEL3, DISABLE)  ;

    ADC2_SchmittTriggerConfig(ADC2_SCHMITTTRIG_CHANNEL2, DISABLE);

    ADC2_SchmittTriggerConfig(ADC2_SCHMITTTRIG_CHANNEL1, DISABLE);



    //prescaler
    ADC2_PrescalerConfig(ADC2_PRESSEL_FCPU_D4);

    // Zarovnání výsledku doprava
    ADC2_AlignConfig(ADC2_ALIGN_RIGHT);

    // Před spuštěním ADC vyberu kanály, které budu používat
    ADC2_Select_Channel(Prstenicek);
    ADC2_Select_Channel(Prostrednicek);
    ADC2_Select_Channel(Ukazovacek);
    ADC2_Select_Channel(Palec);
     ADC2_Select_Channel(Malicek);

    // Zapnutí ADC
    ADC2_Cmd(ENABLE);

    // Čas potřebný pro stabilizaci ADC
    ADC2_Startup_Wait();
}

#define servo_max 2500
#define servo_min 500

#define Prstenicek_rovny_adc 580
#define Prstenicek_max_adc   820

#define Prostrednicek_rovny_adc 447
#define Prostrednicek_max_adc 730

#define Ukazovacek_rovny_adc 436
#define Ukazovacek_max_adc  735

#define Palec_rovny_adc 468
#define Palec_max_adc   780

#define Malicek_rovny_adc 420
#define Malicek_max_adc 703

int main(void)
{
  
    uint32_t time = 0;
    uint8_t rezim= 0;
    uint8_t gesta_rezimy=0;
    bool btn_press;
    bool gesto_press;


    uint16_t hodnota= 0;
    uint16_t adc_hodnota_prstenicek ;
    uint16_t adc_hodnota_prostrednicek;
    uint16_t adc_hodnota_ukazovacek;
    uint16_t adc_hodnota_palec;
    uint16_t adc_hodnota_malicek;

    uint16_t pwm_hodnota_prstenicek;
    uint16_t pwm_hodnota_prostrednicek;
    uint16_t pwm_hodnota_ukazovacek;
    uint16_t pwm_hodnota_palec;
    uint16_t pwm_hodnota_malicek;
    init();
    adc_init();

    while (1) {
        


        if (milis() - time > 100) {
            time = milis();

            if(PUSH(BTN) && !btn_press){
                rezim+=1;
            }

            if(PUSH(GESTO) && !gesto_press){
                gesta_rezimy+=1;

            }

            gesto_press=PUSH(GESTO);
            btn_press=PUSH(BTN);


            //zatím jenom 2 režimy, gesta a manuální

            if(rezim > 1){
                rezim=0;
            }

            if(gesta_rezimy> 5){
                gesta_rezimy=0;
            }


            if(rezim == 0){ 

            //prsteníček

            adc_hodnota_prstenicek=ADC_get(Prstenicek); // 0 - 1024
            
            pwm_hodnota_prstenicek=2200 -
            ((int32_t)adc_hodnota_prstenicek - Prstenicek_rovny_adc) *(servo_max -servo_min)/(Prstenicek_max_adc - Prstenicek_rovny_adc);


             if(pwm_hodnota_prstenicek > 2200){
                pwm_hodnota_prstenicek=2200;
            }
            if(pwm_hodnota_prstenicek < 500){
                pwm_hodnota_prstenicek=500;
            }

            TIM2_SetCompare1(pwm_hodnota_prstenicek);  //pin PD4

            //------------------------------------------------------------------------------------------------ Prostředníček

            adc_hodnota_prostrednicek =ADC_get(Prostrednicek);

            pwm_hodnota_prostrednicek=500 +
            ((int32_t)adc_hodnota_prostrednicek - Prostrednicek_rovny_adc) *(servo_max - servo_min)/(Prostrednicek_max_adc - Prostrednicek_rovny_adc);
            
            if (pwm_hodnota_prostrednicek > 2500) {
                pwm_hodnota_prostrednicek = 2500;
            }
            if (pwm_hodnota_prostrednicek < 500) {
                pwm_hodnota_prostrednicek = 500;
            }

            TIM2_SetCompare2(pwm_hodnota_prostrednicek);  //PIN PD3

            //------------------------------------------------------------------------------------------------ Ukazováček

            adc_hodnota_ukazovacek=ADC_get(Ukazovacek);

            pwm_hodnota_ukazovacek=500 +
            ((int32_t)adc_hodnota_ukazovacek - Ukazovacek_rovny_adc) *(servo_max - servo_min)/(Ukazovacek_max_adc - Ukazovacek_rovny_adc);

             if (pwm_hodnota_ukazovacek > 2500) {
                pwm_hodnota_ukazovacek = 2500;
            }
            if (pwm_hodnota_ukazovacek < 500) {
                pwm_hodnota_ukazovacek = 500;
            }


            TIM2_SetCompare3(pwm_hodnota_ukazovacek); 
            
            //------------------------------------------------------------------------------------------------ Palec

            adc_hodnota_palec=ADC_get(Palec);

            pwm_hodnota_palec = 500 +
            ((int32_t)adc_hodnota_palec - Palec_rovny_adc) *(servo_max - servo_min)/(Palec_max_adc - Palec_rovny_adc);

            if (pwm_hodnota_palec > 2500) {
                pwm_hodnota_palec = 2500;
            }
            if (pwm_hodnota_palec < 500) {
                pwm_hodnota_palec = 500;
            }

            TIM3_SetCompare1(pwm_hodnota_palec);


            //------------------------------------------------------------------------------------------------ Malíček

            adc_hodnota_malicek= ADC_get(Malicek);

            pwm_hodnota_malicek= 2200 -
            ((int32_t)adc_hodnota_malicek - Malicek_rovny_adc) *(servo_max -servo_min)/(Malicek_max_adc - Malicek_rovny_adc);

              if(pwm_hodnota_malicek > 2200){
                pwm_hodnota_malicek=2200;
            }
            if(pwm_hodnota_malicek < 500){
                pwm_hodnota_malicek=500;
            }

            TIM3_SetCompare2(pwm_hodnota_malicek);


           

        
            printf("Prtstenicek adc: %u -> PWM: %u us Prostrednicek ADC: %u -> PWM: %u us Ukazovacek adc: %u -> pwm: %u us Palec adc: %u -> pwm: %u Malicek adc: %u -> pwm: %u us\n ", 
                adc_hodnota_prstenicek, pwm_hodnota_prstenicek,adc_hodnota_prostrednicek, pwm_hodnota_prostrednicek,
                adc_hodnota_ukazovacek,pwm_hodnota_ukazovacek,adc_hodnota_palec,pwm_hodnota_palec,adc_hodnota_malicek,pwm_hodnota_malicek);
        }

        if(rezim == 1){ //režim gest

            printf("Režim přepnut\n");
            
            if(gesta_rezimy==0){    //výchozí poloha

                pwm_hodnota_palec =pwm_hodnota_ukazovacek =pwm_hodnota_prostrednicek = 500;
                pwm_hodnota_prstenicek=pwm_hodnota_malicek= 2200;
                
                TIM2_SetCompare1(pwm_hodnota_prstenicek);
                TIM2_SetCompare2(pwm_hodnota_prostrednicek);
                TIM2_SetCompare3(pwm_hodnota_ukazovacek);
                
                TIM3_SetCompare1(pwm_hodnota_palec);
                TIM3_SetCompare2(pwm_hodnota_malicek);

            }
            if(gesta_rezimy==1){    //pěst
                
                pwm_hodnota_ukazovacek =pwm_hodnota_prostrednicek = 2200;
                pwm_hodnota_prstenicek=pwm_hodnota_malicek= 500;
                pwm_hodnota_palec =2000;
                
                TIM2_SetCompare1(pwm_hodnota_prstenicek);
                TIM2_SetCompare2(pwm_hodnota_prostrednicek);
                TIM2_SetCompare3(pwm_hodnota_ukazovacek);
                
                TIM3_SetCompare1(pwm_hodnota_palec);
                TIM3_SetCompare2(pwm_hodnota_malicek);

            }
               if(gesta_rezimy==2){    //peace sign
                
                pwm_hodnota_palec =2200;

                pwm_hodnota_prstenicek=500;
                pwm_hodnota_malicek=500;
                pwm_hodnota_ukazovacek=pwm_hodnota_prostrednicek= 500;
                
                TIM2_SetCompare1(pwm_hodnota_prstenicek);
                TIM2_SetCompare2(pwm_hodnota_prostrednicek);
                TIM2_SetCompare3(pwm_hodnota_ukazovacek);
                
                TIM3_SetCompare1(pwm_hodnota_palec);
                TIM3_SetCompare2(pwm_hodnota_malicek);

            }
            if(gesta_rezimy==3){    //palec nahoru
                
                pwm_hodnota_palec =500;

                pwm_hodnota_prstenicek=pwm_hodnota_malicek=500;
                pwm_hodnota_ukazovacek=pwm_hodnota_prostrednicek= 2200;
                
                TIM2_SetCompare1(pwm_hodnota_prstenicek);
                TIM2_SetCompare2(pwm_hodnota_prostrednicek);
                TIM2_SetCompare3(pwm_hodnota_ukazovacek);
                
                TIM3_SetCompare1(pwm_hodnota_palec);
                TIM3_SetCompare2(pwm_hodnota_malicek);

            }

            if(gesta_rezimy==4){    //prostrednicek
                
                pwm_hodnota_palec = 2200;
                pwm_hodnota_ukazovacek=2200;

                pwm_hodnota_prstenicek=500;
                pwm_hodnota_malicek=500;
                pwm_hodnota_prostrednicek=500;
               
                
                TIM2_SetCompare1(pwm_hodnota_prstenicek);
                TIM2_SetCompare2(pwm_hodnota_prostrednicek);
                TIM2_SetCompare3(pwm_hodnota_ukazovacek);
                
                TIM3_SetCompare1(pwm_hodnota_palec);
                TIM3_SetCompare2(pwm_hodnota_malicek);

            }
              if(gesta_rezimy==5){    //metal
                
                pwm_hodnota_prostrednicek=2500;
                pwm_hodnota_malicek=2200;
                pwm_hodnota_ukazovacek =500;
                pwm_hodnota_prstenicek=500;
                pwm_hodnota_palec=500;
               
                
                TIM2_SetCompare1(pwm_hodnota_prstenicek);
                TIM2_SetCompare2(pwm_hodnota_prostrednicek);
                TIM2_SetCompare3(pwm_hodnota_ukazovacek);
                
                TIM3_SetCompare1(pwm_hodnota_palec);
                TIM3_SetCompare2(pwm_hodnota_malicek);

            }
            


        }

        }


        //delay_ms(333);
    }
    }

/*-------------------------------  Assert -----------------------------------*/
#include "__assert__.h"