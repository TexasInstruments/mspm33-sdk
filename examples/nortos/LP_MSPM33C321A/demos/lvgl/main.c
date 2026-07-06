/*
 * Copyright (c) 2025, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "ti_msp_dl_config.h"

#include "porting/lv_port_disp.h"
#include "porting/lv_port_indev.h"
#include "lvgl.h"
#include "spi/spi.h"
#include <stdbool.h>
#include <string.h>

#include "demos/cluster/Speedo_demo.h"

// Master switch to enable all features at once
#ifndef ENABLE_ALL_FEATURES
#define ENABLE_ALL_FEATURES 1
#endif

// Animation feature toggles for cluster demo
#define ENABLE_ARROW_BLINK_ANIMATION            0
#define ENABLE_CENTER_MESSAGE_ANIMATION         0
#define ENABLE_RPM_ANIMATION                    0
#define ENABLE_SPEED_ANIMATION                  0
#define ENABLE_FUEL_ANIMATION                   0
#define ENABLE_GEAR_ANIMATION                   0
#define ENABLE_ODO_ANIMATION                    0
#define ENABLE_TIME_ANIMATION                   0

#if ENABLE_ALL_FEATURES
#undef ENABLE_ARROW_BLINK_ANIMATION
#define ENABLE_ARROW_BLINK_ANIMATION 1
#undef ENABLE_CENTER_MESSAGE_ANIMATION
#define ENABLE_CENTER_MESSAGE_ANIMATION 1
#undef ENABLE_RPM_ANIMATION
#define ENABLE_RPM_ANIMATION 1
#undef ENABLE_SPEED_ANIMATION
#define ENABLE_SPEED_ANIMATION 1
#undef ENABLE_FUEL_ANIMATION
#define ENABLE_FUEL_ANIMATION 1
#undef ENABLE_GEAR_ANIMATION
#define ENABLE_GEAR_ANIMATION 1
#undef ENABLE_ODO_ANIMATION
#define ENABLE_ODO_ANIMATION 1
#undef ENABLE_TIME_ANIMATION
#define ENABLE_TIME_ANIMATION 1
#endif

extern bool left_arrow_blinking;
extern bool right_arrow_blinking;

void Build_UI(void);
void update_rpm_inst(int rpm);
void update_gear(const char *gear);
void update_fuel(int percent);
void update_odo(int miles);
void set_animation_text_active(bool active);
void start_left_arrow_blink(void);
void start_right_arrow_blink(void);
void arrow_blink_task(void);
void set_center_message(const char *msg);
void set_center_message_color(lv_color_t color);

#define RPM_UPDATE_THRESHOLD   1     // Timer ticks before next target change
#define RPM_INCREMENT          500   // Step size for target RPM
#define RPM_MAX                8000  // Upper limit for the simulated engine
#define RPM_DECREMENT_RATE     2000  // Rate at which the displayed RPM falls

static int rpm_anim   = 0;      // Counts timer‑interrupt ticks
static int rpm_target = 0;      // Desired RPM – changes every RPM_UPDATE_THRESHOLD
static int rpm_display = 0;     // RPM value that is actually shown (animated)
volatile uint32_t ms_tick = 0;  // Global ms tick for timekeeping
static int last_rpm = 0;        // For ODO animation
static int odo_value = 0;       // ODO value


static void anim_gear(int rpm)
{
    char gear[2];
    if      (rpm < 1000) gear[0] = 'N';
    else if (rpm < 2000) gear[0] = '1';
    else if (rpm < 3000) gear[0] = '2';
    else if (rpm < 4500) gear[0] = '3';
    else if (rpm < 6000) gear[0] = '4';
    else                 gear[0] = '5';
    gear[1] = '\0';
    update_gear(gear);
}

static void anim_speed(int rpm_main) {
    // Map rpm 0-8000 to speed 0-99
    int speed = (rpm_main * 99) / 8000;
    update_speed(speed);
}

int main(void)
{
    SYSCFG_DL_init();
    NVIC_EnableIRQ(TIMER_0_INST_INT_IRQN);
    NVIC_EnableIRQ(SPI_0_INST_INT_IRQN);
    lv_init();
    lv_port_disp_init();
    lv_port_indev_init();
    DL_TimerG_startCounter(TIMER_0_INST);
    Build_UI();

    int fuel = 0;
    int fuel_dir = 1; // 1: up, -1: down
    uint32_t last_fuel_tick = 0;

    while (1) {
        // update LVGL task
        lv_task_handler();

#if ENABLE_ARROW_BLINK_ANIMATION
        arrow_blink_task();
#endif
#if ENABLE_TIME_ANIMATION
        update_time_label(ms_tick);
#endif

#if ENABLE_CENTER_MESSAGE_ANIMATION
        static uint32_t last_test_blink = 0;
        static bool center_msg_blinking = false;
#if !ENABLE_ARROW_BLINK_ANIMATION
        static uint32_t center_msg_blink_start = 0;
#endif
        if ((ms_tick - last_test_blink) > 10000) {
            last_test_blink = ms_tick;
#if !ENABLE_ARROW_BLINK_ANIMATION
            center_msg_blink_start = ms_tick;
#endif
            set_center_message("Calling...");
            set_center_message_color(lv_color_make(255, 215, 0));
            set_animation_text_active(true);
            center_msg_blinking = true;
#if ENABLE_ARROW_BLINK_ANIMATION
            start_left_arrow_blink();
            start_right_arrow_blink();
#endif
        }
        // Restore message after blinking ends (for center message only)
        if (center_msg_blinking) {
#if ENABLE_ARROW_BLINK_ANIMATION
            if (!left_arrow_blinking && !right_arrow_blinking) {
                set_center_message("Welcome!");
                set_center_message_color(lv_color_make(0,255,0));
                set_animation_text_active(false);
                center_msg_blinking = false;
            }
#else
            if ((ms_tick - center_msg_blink_start) >= 5000) {
                set_center_message("Welcome!");
                set_center_message_color(lv_color_make(0,255,0));
                set_animation_text_active(false);
                center_msg_blinking = false;
            }
#endif
        }
#endif

#if ENABLE_ARROW_BLINK_ANIMATION && !ENABLE_CENTER_MESSAGE_ANIMATION
        static uint32_t last_arrow_blink_test = 0;
        if ((ms_tick - last_arrow_blink_test) > 10000) {
            last_arrow_blink_test = ms_tick;
            start_left_arrow_blink();
            start_right_arrow_blink();
        }
#endif

        // Update the simulated engine RPM every RPM_UPDATE_THRESHOLD timer ticks.
        if (rpm_anim > RPM_UPDATE_THRESHOLD) {
            rpm_target += RPM_INCREMENT;
            if (rpm_target > RPM_MAX) {
                rpm_target = 0;
            }
            rpm_anim = 0;
        }

        // Smoothly animate rpm_display towards rpm_target.
#if ENABLE_RPM_ANIMATION
        if (rpm_display < rpm_target) {
            rpm_display += RPM_INCREMENT;
            if (rpm_display > rpm_target) {
                rpm_display = rpm_target;
            }
        } else if (rpm_display > rpm_target) {
            rpm_display -= RPM_DECREMENT_RATE;
            if (rpm_display < 0) {
                rpm_display = 0;
            }
        }
#endif

        // Present the animated values on the UI.
#if ENABLE_RPM_ANIMATION
        update_rpm_inst(rpm_display);
#endif
#if ENABLE_GEAR_ANIMATION
        anim_gear(rpm_display);
#endif
#if ENABLE_SPEED_ANIMATION
        anim_speed(rpm_display);
#endif
#if ENABLE_ODO_ANIMATION
        if (last_rpm != rpm_display && rpm_display >= 7600) {
            odo_value++;
            update_odo(odo_value);
        }
        last_rpm = rpm_display;
#endif

        // Fuel update every 100ms using ms_tick as 1ms tick
#if ENABLE_FUEL_ANIMATION
        if ((ms_tick - last_fuel_tick) >= 100) {
            last_fuel_tick = ms_tick;
            fuel += 5 * fuel_dir;
            if (fuel >= 100) {
                fuel = 100;
                fuel_dir = -1;
            } else if (fuel <= 0) {
                fuel = 0;
                fuel_dir = 1;
            }
            update_fuel(fuel);
        }
#endif
    }
    return 0;
}

void TIMER_0_INST_IRQHandler(void)
{
    switch (DL_TimerG_getPendingInterrupt(TIMER_0_INST)) {
        case DL_TIMER_IIDX_ZERO:
            lv_tick_inc(1);  //LVGL Heart Beat
            ++rpm_anim;
            ++ms_tick;
            break;
        default:
            break;
    }
}

#if !ENABLE_ODO_ANIMATION
__attribute__((weak)) void update_odo(int miles) {
    // ODO animation disabled: do nothing
}
#endif