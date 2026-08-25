//
// Created by all on 16. 12. 2025.
//

#include "to_mfc.h"
#include <time.h>

#include "ibus.h"
#include "pico/stdlib.h"
#include "hardware/clocks.h"
//#include "hardware/pwm.h"


#define MFC_CHANEL_J4_PWM_OUT_PIN 2
#define MFC_CHANEL_J5_PWM_OUT_PIN 3
#define MFC_CHANEL_J6_PWM_OUT_PIN 4
#define MFC_CHANEL_J7_PWM_OUT_PIN 5

#define TIMER_FOR_MFC TIMER_IRQ_0


void timer_mfc_int_handler (void) {
    hw_clear_bits(&timer_hw->intr, 1u << TIMER_FOR_MFC); //clear interrupt
    //mfc_active_chan - mfc channel number J4, J5, J6, J7
    static uint8_t mfc_active_chan = 4;
    static uint32_t t_all =0; //sum of pulse width of all 4 channels
    uint32_t t;

    if (mfc_active_chan == 0) {
        gpio_put(MFC_CHANEL_J7_PWM_OUT_PIN, 0);
        timer_hw->alarm[TIMER_FOR_MFC] = time_us_32() + (20000-t_all) ; //pause 50hz
        mfc_active_chan = 4;
        t_all = 0;
        return;
    }
    if (mfc_active_chan == 4) {
        t = ibus_get_channel(0);  //ibus channl 0 -> J4 MFC
        timer_hw->alarm[TIMER_FOR_MFC] = time_us_32() + t;
        gpio_put(MFC_CHANEL_J4_PWM_OUT_PIN, 1);
        mfc_active_chan = 5;
        t_all = t_all + t;
        return;
    }
    if (mfc_active_chan == 5) {
        t = ibus_get_channel(2); //ibus channl 2 -> J5 MFC
        gpio_put(MFC_CHANEL_J4_PWM_OUT_PIN, 0);
        timer_hw->alarm[TIMER_FOR_MFC] = time_us_32() + t;
        gpio_put(MFC_CHANEL_J5_PWM_OUT_PIN, 1);
        mfc_active_chan = 6;
        t_all = t_all + t;
        return;
    }
    if (mfc_active_chan == 6) {
        t = ibus_get_channel(1);  //ibus channl 1 -> J6 MFC
        gpio_put(MFC_CHANEL_J5_PWM_OUT_PIN, 0);
        timer_hw->alarm[TIMER_FOR_MFC] = time_us_32() + t;
        gpio_put(MFC_CHANEL_J6_PWM_OUT_PIN, 1);
        mfc_active_chan = 7;
        t_all = t_all + t;
        return;
    }
    if (mfc_active_chan == 7) {
        t = ibus_get_channel(3);  //ibus channl 3 -> J7 MFC
        gpio_put(MFC_CHANEL_J6_PWM_OUT_PIN, 0);
        timer_hw->alarm[TIMER_FOR_MFC] = time_us_32() + t;
        gpio_put(MFC_CHANEL_J7_PWM_OUT_PIN, 1);
        mfc_active_chan = 0;
        t_all = t_all + t;
        return;
    }

}
/*
 * init J4 J5 J6 J7 pins output
 * set timer irq0
 */


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


    hardware_alarm_claim(TIMER_FOR_MFC);
    irq_set_exclusive_handler(TIMER_FOR_MFC, timer_mfc_int_handler);

    hw_set_bits(&timer_hw->inte, (1u << TIMER_FOR_MFC));
    irq_set_enabled(TIMER_FOR_MFC, true);
    timer_hw->alarm[TIMER_FOR_MFC] = time_us_32() + 2000000;
}


