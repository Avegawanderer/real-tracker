/**
    Project: Real Tracker
    Module:
    Author: avegawanderer@gmail.com
    Creation date:
    Description:

    Implementation of PWM servo driver
*/

#include "driver/mcpwm.h"

#include "drv_pwm.h"
#include "config.h"

//------------ Definitions ----------//

//------------ Variables ------------//

//------------ Externals ------------//

//------------ Prototypes -----------//

//--------- Implementation ----------//


/**
    @brief  Init PWM driver.
            All servo outputs get duty 0 after init, which disables them
            To move a servo, set normal duty, typically 800 - 2200 us
    @param  None
    @return None
*/
void drvPwm_Init(void)
{
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, PAN_SERVO_PULSE_GPIO);
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0B, TILT_SERVO_PULSE_GPIO);
    mcpwm_config_t pwm_config = {
        .frequency = 50,            // frequency = 50Hz, i.e. for every servo motor time period should be 20ms
        .cmpr_a = 0,                // duty cycle of PWMxA = 0
        .cmpr_b = 0,                // duty cycle of PWMxB = 0
        .duty_mode = MCPWM_DUTY_MODE_0,
        .counter_mode = MCPWM_UP_COUNTER
    };
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);
    mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_A, 0);
    mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_B, 0);

    //MCPWM0.int_ena.val = 0;  // disable all interrupts
    //MCPWM0.int_clr.val = 0;  // clear all interrupts
    //mcpwm_isr_register(MCPWM_UNIT_0, toto, NULL, ESP_INTR_FLAG_IRAM | ESP_INTR_FLAG_SHARED, NULL);  //Set ISR Handler (active interrupts will be set later)
    //mcpwm->int_ena.val = BIT(0); //Interrupt when timer 0 stops//
    //MCPWM0.int_ena.op2_teb_int_ena = 1;
}


/**
    @brief  Set duty for PAN servo
            Duty value 0 effectively disables servo (no pulses)
    @param  None
    @return None
*/
void drvPwm_SetPanDuty(uint16_t us)
{
    mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_A, us);
}


/**
    @brief  Set duty for TILT servo
            Duty value 0 effectively disables servo (no pulses)
    @param  None
    @return None
*/
void drvPwm_SetTiltDuty(uint16_t us)
{
    mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_B, us);
}

