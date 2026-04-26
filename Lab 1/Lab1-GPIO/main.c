    #include <stdint.h>
    #include "GPIO_stm32.h"
    #include "led_driver.h"
    #include "button_driver.h"

    /*
    #if !defined(__SOFT_FP__) && defined(__ARM_FP)
    #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
    #endif
    */

    #define S0 0U
    #define S1 1U
    #define S2 2U
    #define S3 3U

    int main(void)

    {

        uint32_t *CPACR = (uint32_t *)0xE000ED88;
        *CPACR |= (0xF << 20);

        if (gpio_init()!= GPIO_OK){return 0;}
        led_init();
        button_init();

        uint8_t state = 0;
        uint8_t t_output = 0;

        while(1){

            button_get_state(&state);

            switch(t_output){

                case S0:
                    if(state == 1){
                        t_output = S1;
                    }
                    break;

                case S1:
                    led_on();
                    if(state == 0){
                        t_output = S2;
                    }
                    break;

                case S2:
                    if(state == 1){
                        t_output = S3;
                    }
                    break;

                case S3:
                    led_off();
                    if(state == 0){
                        t_output = S0;
                    }
                    break;

                default:
                    t_output = S0;
                    break;
            }
        }
        return 0;

    }