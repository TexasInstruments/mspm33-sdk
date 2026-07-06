#pragma once

#include "lvgl.h"

#define EXAMPLE1_LVGL_TICK_PERIOD_MS  1000


void Build_UI(void);
void update_rpm(int new_rpm);
void update_rpm_inst(int new_rpm);
void update_gear(const char *gear);
void update_speed(int new_speed);
void update_time_label(uint32_t ms_tick);