//
// Created by all on 16. 12. 2025.
//

#include "to_mfc.h"
#include <stdio.h>
#include <time.h>

#include "ibus.h"
#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "hardware/pwm.h"


#define MFC_CHANEL_J4_PWM_OUT_PIN 2     //slice 6 B
#define MFC_CHANEL_J5_PWM_OUT_PIN 3     //slice 6 B
#define MFC_CHANEL_J6_PWM_OUT_PIN 4     //slice 6 B
#define MFC_CHANEL_J7_PWM_OUT_PIN 5     //slice 6 B

#define TIMER_FOR_MFC TIMER_IRQ_0





void timer_mfc_int_handler (void) {
    hw_clear_bits(&timer_hw->intr, 1u << TIMER_FOR_MFC); //clear interrupt
    static uint8_t mfc_active_chan = 4;
    static uint32_t t_all =0;
    uint32_t t;

    if (mfc_active_chan == 0) {
        gpio_put(MFC_CHANEL_J7_PWM_OUT_PIN, 0);
        timer_hw->alarm[TIMER_FOR_MFC] = time_us_32() + (20000-t_all) ; //pause 50hz
        mfc_active_chan = 4;
        t_all = 0;
        printf("A");
        return;
    }
    if (mfc_active_chan == 4) {
        t = ibus_get_channel(0);
        timer_hw->alarm[TIMER_FOR_MFC] = time_us_32() + t; //next time 1.chan
        gpio_put(MFC_CHANEL_J4_PWM_OUT_PIN, 1);
        mfc_active_chan = 5;
        t_all = t_all + t;
        printf("B");
        return;
    }
    if (mfc_active_chan == 5) {
        t = ibus_get_channel(2);
        gpio_put(MFC_CHANEL_J4_PWM_OUT_PIN, 0);
        timer_hw->alarm[TIMER_FOR_MFC] = time_us_32() + t; //next time 3.chan
        gpio_put(MFC_CHANEL_J5_PWM_OUT_PIN, 1);
        mfc_active_chan = 6;
        t_all = t_all + t;
        printf("C");
        return;
    }
    if (mfc_active_chan == 6) {
        t = ibus_get_channel(1);
        gpio_put(MFC_CHANEL_J5_PWM_OUT_PIN, 0);
        timer_hw->alarm[TIMER_FOR_MFC] = time_us_32() + t; //next time 2.chan
        gpio_put(MFC_CHANEL_J6_PWM_OUT_PIN, 1);
        mfc_active_chan = 7;
        t_all = t_all + t;
        printf("D");
        return;
    }
    if (mfc_active_chan == 7) {
        t = ibus_get_channel(3);
        gpio_put(MFC_CHANEL_J6_PWM_OUT_PIN, 0);
        timer_hw->alarm[TIMER_FOR_MFC] = time_us_32() + t; //next time 2.chan
        gpio_put(MFC_CHANEL_J7_PWM_OUT_PIN, 1);
        mfc_active_chan = 0;
        t_all = t_all + t;
        printf("F");
        return;
    }





}

void to_mfc_init(void) {
    gpio_set_function_masked((1<<MFC_CHANEL_J4_PWM_OUT_PIN) |
       (1<<MFC_CHANEL_J5_PWM_OUT_PIN) |
       (1<<MFC_CHANEL_J6_PWM_OUT_PIN) |
       (1<<MFC_CHANEL_J7_PWM_OUT_PIN),
    GPIO_FUNC_SIO);
    gpio_set_dir_out_masked((1<<MFC_CHANEL_J4_PWM_OUT_PIN) |
       (1<<MFC_CHANEL_J5_PWM_OUT_PIN) |
       (1<<MFC_CHANEL_J6_PWM_OUT_PIN) |
       (1<<MFC_CHANEL_J7_PWM_OUT_PIN));
    /*
    pwm_slice_num_mfc = pwm_gpio_to_slice_num(MFC_CHANEL_PWM_OUT_PIN);

    pwm_chan_num_mfc = pwm_gpio_to_channel(MFC_CHANEL_PWM_OUT_PIN);

    //to iste ale RX chanel ten kanal co riadi svetla
    pwm_config c = pwm_get_default_config();
    // ovladanie ako servo, 1 kanaly , jedna slice
    //pre PWM 50Hz: f = fsys/((19999+1) x clkdiv) = 125000000/((19999+1) x 125)
    //perioda = 20ms -> 20000
    //rozsah 1 - 2ms -> 1000 - 2000
    pwm_config_set_clkdiv_int(&c, 125);
    pwm_config_set_clkdiv_mode(&c, PWM_DIV_FREE_RUNNING);
    pwm_config_set_wrap(&c, 19999);
    pwm_config_set_output_polarity(&c,false,false);
    pwm_init(pwm_slice_num_mfc, &c, true);
    pwm_set_chan_level(pwm_slice_num_mfc, pwm_chan_num_mfc, 1500); //stred
    */

    hardware_alarm_claim(TIMER_FOR_MFC);

    irq_set_exclusive_handler(TIMER_FOR_MFC, timer_mfc_int_handler);

    hw_set_bits(&timer_hw->inte, (1u << TIMER_FOR_MFC));
    irq_set_enabled(TIMER_FOR_MFC, true);
    timer_hw->alarm[TIMER_FOR_MFC] = time_us_32() + 2000000;
}


void to_mfc_service (void) {

}

