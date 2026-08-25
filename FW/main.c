
#include <stdio.h>
#include "pico/stdlib.h"
//#include "pico/stdio.h"
#include "hardware/clocks.h"
#include "hardware/pwm.h"
#include <stdlib.h>
#include <time.h>
#include "ibus.h"
#include"rear_light.h"
#include "turn_light.h"
#include "to_mfc.h"

#define DBG


#define LED_PIN 25




int main(void) {
    stdout_uart_init();
//    stdio_usb_init();
    printf("Starting...\n");
    gpio_set_function_masked((1<<LED_PIN),GPIO_FUNC_SIO);

    //pre AUX

    gpio_set_dir_out_masked((1<<LED_PIN));

    gpio_put(LED_PIN,1);
    busy_wait_ms(500);
    gpio_put(LED_PIN,0);
    busy_wait_ms(500);
    gpio_put(LED_PIN,1);
    busy_wait_ms(500);
    gpio_put(LED_PIN,0);
    busy_wait_ms(500);
    gpio_put(LED_PIN,1);


    //init iBUS uart
    ibus_init();
    while (!ibus_data_valid()) {
        ibus_service();
    }
    //rear_light_init();

    to_mfc_init();
    //turn_light_init();

    while(1) {
        //****************************************
        //              zadne svetla
        //****************************************
    //    rear_light_service();

        //****************************************
        //              smerovky
        //****************************************
    //    turn_light_service();

        //beacon

        //to mfc
        to_mfc_service();


    } //while 1

} //main
