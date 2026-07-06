#ifndef ENABLE_ODO_ANIMATION
#define ENABLE_ODO_ANIMATION 1
#endif

#include "Speedo_demo.h"
#include "../../lvgl.h"
#include <stdio.h>
#include <math.h>
#include "fonts/montserrat_bold_28.h"
#include "fonts/montserrat_bold_80.h"
#include "fonts/montserrat_bold_120.h"

extern volatile uint32_t ms_tick;

//==============================================================================
// FUNCTION DECLARATIONS
//==============================================================================

void update_odo(int miles);
void position_markers(lv_obj_t *marker, int position);
void create_left_arrow(void);
void create_right_arrow(void);
void start_left_arrow_blink(void);
void start_right_arrow_blink(void);
void arrow_blink_task(void);
void make_time_area(void);
void update_time_label(uint32_t ms_tick);


extern const lv_img_dsc_t TXN_logo;

//==============================================================================
// GLOBAL UI COMPONENTS
//==============================================================================

lv_obj_t *scr;                  // Screen background
lv_obj_t *rpm_arc;              // Moving arc
lv_obj_t *rpm_label;            // Digital RPM
lv_obj_t *mph_label;            // Digital MPH
lv_obj_t *gear_label;           // Digital gear
lv_obj_t *gear_canvas;          // Gear canvas
lv_obj_t *odo_title_label;      // ODO title label
lv_obj_t *odo_value_label;      // ODO value label
lv_obj_t *odo_km_label;         // ODO km label
lv_obj_t *gas_pump_img;         // Gas pump icon
lv_obj_t *high_beam_img;        // High beam icon
lv_obj_t *TXN_logo_img;         // Logo icon
lv_obj_t *racing_helmet_img;    // Racing helmet icon
lv_obj_t *fuel_arc;             // Fuel gauge arc
lv_obj_t *left_arrow = NULL;    // Left arrow
lv_obj_t *right_arrow = NULL;   // Right arrow
lv_obj_t *center_message_label = NULL; // Center message label
lv_obj_t *time_box = NULL;
lv_obj_t *time_label = NULL;

static lv_obj_t *fuel_ticks[21] = {NULL};
static lv_obj_t *fuel_labels[5] = {NULL};

//==============================================================================
// GLOBAL STYLES
//==============================================================================

static lv_style_t style_unit_text;

//==============================================================================
// COLOR PALETTE
//==============================================================================

lv_color_t palette_amber = LV_COLOR_MAKE(250, 140, 0);
lv_color_t palette_black = LV_COLOR_MAKE(0, 0, 0);
lv_color_t palette_red = LV_COLOR_MAKE(255, 0, 0);
lv_color_t palette_white = LV_COLOR_MAKE(255, 255, 255);

//==============================================================================
// CONFIGURATION CONSTANTS
//==============================================================================

const int dimension = 280;
const int rpm_max = 8000;
const int rpm_redline = 6600;
const int rpm_arc_width = 24;
const int rpm_line_width = 4;
const int rpm_arc_size = dimension - (rpm_line_width * 12);
const int arc_start = 120;
const int arc_end = 344;
const int rpm_freq = 200;
const float marker_gap = ((float)arc_end - arc_start) / (rpm_max / 1000);

// --- Arrow and animation blink interval (ms) ---
static const uint32_t blink_interval = 750;

//==============================================================================
// GLOBAL VARIABLES
//==============================================================================

int rpm = 0;
int rpm_text_update = 0;

// Add a flag to track if animation text is active
static bool animation_text_active = false;
static uint32_t animation_text_last_toggle = 0;
static bool animation_text_visible = true;

#ifndef ENABLE_ODO_ANIMATION
#define ENABLE_ODO_ANIMATION 1
#endif

//==============================================================================
// STYLE INITIALIZATION
//==============================================================================

/**
 * @brief Initialize custom styles for UI elements
 */
void make_styles(void) {
    lv_style_init(&style_unit_text);
    lv_style_set_text_font(&style_unit_text, &montserrat_bold_28);
    lv_style_set_text_color(&style_unit_text, palette_amber);
}

//==============================================================================
// ANIMATION CALLBACK FUNCTIONS
//==============================================================================

/**
 * @brief Animation callback for RPM arc updates
 * @param arc Pointer to the arc object
 * @param value New value to set for the arc
 */
void arc_anim_cb(void *arc, int32_t value) {
    lv_arc_set_value((lv_obj_t *)arc, value);
}

//==============================================================================
// UPDATE FUNCTIONS - SPEED
//==============================================================================

/**
 * @brief Update the speed display
 * @param new_speed New speed value to display
 */
void update_speed(int new_speed) {
    static int speed_text_update = 0;
    static char mph_text[4];
    speed_text_update++;
    if(speed_text_update > 1) {
        speed_text_update = 0;
        snprintf(mph_text, sizeof(mph_text), "%d", new_speed);
        lv_label_set_text(mph_label, mph_text);
    }
}

//==============================================================================
// UPDATE FUNCTIONS - GEAR
//==============================================================================

/**
 * @brief Update the gear display with color coding
 * @param new_gear New gear string to display ('N' shows in green, others in amber)
 */
//void update_gear(char new_gear[2]) {
void update_gear(const char *gear) {
    lv_label_set_text(gear_label, gear);

    if (gear[0] == 'N') {
        lv_obj_set_style_text_color(gear_label, lv_color_make(0, 255, 0), 0);
    } else {
        lv_obj_set_style_text_color(gear_label, palette_amber, 0);
    }
}

//==============================================================================
// UPDATE FUNCTIONS - RPM
//==============================================================================

/**
 * @brief Update RPM with smooth animation
 * @param new_rpm New RPM value to animate to
 */
void update_rpm(int new_rpm) {
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, rpm_arc);
    lv_anim_set_values(&a, lv_arc_get_value(rpm_arc), new_rpm);
    lv_anim_set_time(&a, rpm_freq);
    lv_anim_set_exec_cb(&a, arc_anim_cb);
    lv_anim_set_path_cb(&a, lv_anim_path_linear);
    lv_anim_start(&a);
    if (new_rpm >= rpm_redline) {
        lv_obj_set_style_arc_color(rpm_arc, palette_red, LV_PART_INDICATOR);
    } else {
        lv_obj_set_style_arc_color(rpm_arc, palette_amber, LV_PART_INDICATOR);
    }
    rpm_text_update++;
    static char rpm_text[8];
    if(rpm_text_update>4) {
        rpm_text_update=0;
        snprintf(rpm_text, sizeof(rpm_text), "%d", new_rpm);
        lv_label_set_text(rpm_label, rpm_text);
    }
    rpm = new_rpm;
}

/**
 * @brief Update RPM instantly without animation
 * @param new_rpm New RPM value to set immediately
 */
void update_rpm_inst(int new_rpm) {
    arc_anim_cb(rpm_arc, new_rpm);
    static char rpm_text[8];
    rpm_text_update++;
    if(rpm_text_update>4) {
        rpm_text_update=0;
        snprintf(rpm_text, sizeof(rpm_text), "%d", new_rpm);
        lv_label_set_text(rpm_label, rpm_text);
    }
    rpm = new_rpm;
}

//==============================================================================
// UPDATE FUNCTIONS - ODOMETER
//==============================================================================

#if ENABLE_ODO_ANIMATION
/**
 * @brief Update odometer display with 6-digit format
 * @param miles New mileage value to display
 */
void update_odo(int miles) {
    static char odo_text[8];
    snprintf(odo_text, sizeof(odo_text), "%06d", miles);
    lv_label_set_text(odo_value_label, odo_text);
}
#endif

//==============================================================================
// FUEL GAUGE FUNCTIONS
//==============================================================================

/**
 * @brief Create vertical fuel gauge bar (200x20 px) on left of LCD
 */
void make_fuel_gauge(void) {
    // Arc geometry
    int center_x = 80;
    int center_y = 120;
    int radius = 40; // updated radius
    int arc_width = 10;
    int arc_start = 150; // updated start angle
    int arc_end = 300; // keep as before (user did not specify change)
    int major_len = 18;
    static const char *labels[] = {" E", "1/4", "1/2", "3/4", "F"};
    static const int label_vals[] = {0, 25, 50, 75, 100};
    int label_count = 5;

    // Only create the arc and ticks/labels if not already created
    if (!fuel_arc) {
        fuel_arc = lv_arc_create(scr);
        lv_obj_set_size(fuel_arc, radius * 2, radius * 2);
        lv_obj_set_style_arc_width(fuel_arc, arc_width, LV_PART_MAIN);
        lv_obj_set_style_arc_width(fuel_arc, arc_width, LV_PART_INDICATOR);
        lv_obj_set_style_arc_color(fuel_arc, lv_color_make(40,40,40), LV_PART_MAIN);
        // lv_obj_set_style_arc_color(fuel_arc, palette_amber, LV_PART_INDICATOR);
        lv_obj_set_style_arc_color(fuel_arc, lv_color_make(200, 160, 200), LV_PART_INDICATOR); // Light green
        lv_obj_set_style_arc_rounded(fuel_arc, false, LV_PART_MAIN); // No rounded edge
        lv_obj_set_style_arc_rounded(fuel_arc, false, LV_PART_INDICATOR); // No rounded edge
        lv_arc_set_bg_angles(fuel_arc, arc_start, arc_end);
        lv_arc_set_range(fuel_arc, 0, 100);
        lv_arc_set_value(fuel_arc, 100);
        lv_obj_set_style_shadow_width(fuel_arc, 0, LV_PART_MAIN);
        lv_obj_set_style_shadow_width(fuel_arc, 0, LV_PART_INDICATOR);
        lv_obj_remove_style(fuel_arc, NULL, LV_PART_KNOB);
        lv_obj_align(fuel_arc, LV_ALIGN_TOP_LEFT, center_x - radius, center_y - radius + 20);
        lv_obj_add_flag(fuel_arc, LV_OBJ_FLAG_IGNORE_LAYOUT);
    }
    // Draw only major ticks as arc segments (like RPM arc markers)
    for (int i = 0; i < label_count; ++i) {
        if (!fuel_ticks[i]) {
            float angle = arc_start + ((arc_end - arc_start) * (label_vals[i] / 100.0f));
            float next_angle = angle + 2.0f; // 1.5 deg wide, similar to RPM marker
            int marker_size = (radius * 2) + 12; // slightly larger than arc
            int tick_width = arc_width - 4;
            if (i == 0 || i == label_count - 1) {
                tick_width = arc_width + 4; // Thicker for 0% and 100%
                next_angle = angle + 4.0f;
            }
            fuel_ticks[i] = lv_arc_create(scr);
            lv_obj_set_size(fuel_ticks[i], marker_size, marker_size);
            lv_obj_align(fuel_ticks[i], LV_ALIGN_TOP_LEFT, center_x - marker_size/2, center_y - marker_size/2 + 20);
            lv_arc_set_bg_angles(fuel_ticks[i], angle, next_angle);
            lv_obj_set_style_arc_width(fuel_ticks[i], tick_width, LV_PART_MAIN);
            lv_obj_set_style_arc_rounded(fuel_ticks[i], false, LV_PART_MAIN); // No rounded edge
            lv_obj_set_style_arc_color(fuel_ticks[i], lv_color_make(0, 200, 255), LV_PART_MAIN);
            lv_obj_set_style_shadow_width(fuel_ticks[i], 0, LV_PART_MAIN);
            lv_obj_remove_style(fuel_ticks[i], NULL, LV_PART_KNOB);
            lv_obj_remove_style(fuel_ticks[i], NULL, LV_PART_INDICATOR);
            lv_obj_add_flag(fuel_ticks[i], LV_OBJ_FLAG_IGNORE_LAYOUT);
        }
    }
    // Draw labels
    for (int i = 0; i < label_count; ++i) {
        if (!fuel_labels[i]) {
            float angle = arc_start + ((arc_end - arc_start) * (label_vals[i] / 100.0f));
            float rad = angle * 3.1415926f / 180.0f;
            // Calculate the outer end of the tick marker
            int tick_outer_x = center_x + (int)((radius + major_len + arc_width/2 + 2) * cosf(rad));
            int tick_outer_y = center_y + (int)((radius + major_len + arc_width/2 + 2) * sinf(rad));
            // Place label to the left of the tick marker (relative to the tick's direction)
            int label_x = tick_outer_x - 4; // 24 px to the left
            int label_y = tick_outer_y - 4 + 20;  // vertically centered
            fuel_labels[i] = lv_label_create(scr);
            lv_label_set_text(fuel_labels[i], labels[i]);
            lv_obj_set_style_text_color(fuel_labels[i], palette_amber, 0);
            lv_obj_set_style_text_font(fuel_labels[i], &lv_font_montserrat_12, 0);
            lv_obj_set_pos(fuel_labels[i], label_x, label_y);
        }
    }
}

/**
 * @brief Create low fuel red arc above the fuel arc between 0-25%
 */
void make_fuel_low_arc(void) {
    // Geometry must match make_fuel_gauge
    int center_x = 80;
    int center_y = 120;
    int radius = 45;
    int arc_width = 3;
    int arc_start = 154;
    int arc_end = 290;
    int low_start = arc_start;
    int low_end = arc_start + (arc_end - arc_start) * 0.25f; // 0-25%
    lv_obj_t *fuel_low_arc = lv_arc_create(scr);
    lv_obj_set_size(fuel_low_arc, radius * 2, radius * 2);
    lv_obj_align(fuel_low_arc, LV_ALIGN_TOP_LEFT, center_x - radius, center_y - radius + 20);
    lv_arc_set_bg_angles(fuel_low_arc, low_start, low_end);
    lv_obj_set_style_arc_width(fuel_low_arc, arc_width + 2, LV_PART_MAIN);
    lv_obj_set_style_arc_color(fuel_low_arc, palette_red, LV_PART_MAIN);
    lv_obj_set_style_arc_rounded(fuel_low_arc, false, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(fuel_low_arc, 0, LV_PART_MAIN);
    lv_obj_remove_style(fuel_low_arc, NULL, LV_PART_KNOB);
    lv_obj_remove_style(fuel_low_arc, NULL, LV_PART_INDICATOR);
    lv_obj_add_flag(fuel_low_arc, LV_OBJ_FLAG_IGNORE_LAYOUT);
}

void update_fuel(int percent) {
    if (!fuel_arc) return;
    if(percent < 0) percent = 0;
    if(percent > 100) percent = 100;
    if(percent >= 75)
    {
        lv_obj_set_style_img_recolor(gas_pump_img, lv_color_make(0, 255, 0), 0);
    }
    else if(percent<=25)
    {
        lv_obj_set_style_img_recolor(gas_pump_img, lv_color_make(255, 95, 31), 0);
    }
    else
    {
        lv_obj_set_style_img_recolor(gas_pump_img, lv_color_make(255, 174, 66), 0);
    }   
    lv_arc_set_value(fuel_arc, percent);
}

//==============================================================================
// UI CREATION FUNCTIONS - RPM ARC
//==============================================================================

/**
 * @brief Create the main RPM arc indicator
 */
void make_rpm_arc(void) {
    rpm_arc = lv_arc_create(scr);
    lv_obj_set_size(rpm_arc, rpm_arc_size, rpm_arc_size);
    lv_obj_align(rpm_arc, LV_ALIGN_CENTER, 0, 20);
    lv_arc_set_bg_angles(rpm_arc, arc_start + 2, arc_end - 1);
    
    lv_obj_set_style_arc_color(rpm_arc, lv_color_make(20, 20, 30), LV_PART_MAIN);
    lv_obj_set_style_arc_color(rpm_arc, palette_amber, LV_PART_INDICATOR);
    lv_obj_set_style_arc_rounded(rpm_arc, true, LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(rpm_arc, rpm_arc_width, LV_PART_INDICATOR);
    lv_obj_set_style_shadow_width(rpm_arc, 0, LV_PART_INDICATOR);
    
    lv_obj_remove_style(rpm_arc, NULL, LV_PART_KNOB);
    lv_arc_set_range(rpm_arc, 0, rpm_max);
    lv_arc_set_value(rpm_arc, rpm_max);
}

/**
 * @brief Create RPM border and markers
 */
void make_rpm_border(void) {
    lv_obj_t *rpm_arc_border = lv_arc_create(scr);
    lv_obj_set_size(rpm_arc_border, rpm_arc_size - (rpm_arc_width * 2) - (rpm_line_width * 2), 
                    rpm_arc_size - (rpm_arc_width * 2) - (rpm_line_width * 2));
    lv_obj_align(rpm_arc_border, LV_ALIGN_CENTER, 0, 20);
    lv_arc_set_bg_angles(rpm_arc_border, arc_start, arc_end);
    lv_obj_set_style_arc_width(rpm_arc_border, rpm_line_width, LV_PART_MAIN);
    lv_obj_set_style_arc_rounded(rpm_arc_border, false, LV_PART_MAIN);
    lv_obj_set_style_arc_color(rpm_arc_border, lv_color_make(100, 150, 255), LV_PART_MAIN);
    lv_obj_set_style_shadow_width(rpm_arc_border, 0, LV_PART_MAIN);
    
    lv_obj_remove_style(rpm_arc_border, NULL, LV_PART_KNOB);
    lv_obj_remove_style(rpm_arc_border, NULL, LV_PART_INDICATOR);
    
    int rpm_marker_count = (rpm_max / 1000) + 1;
    lv_obj_t *rpm_markers[9]; /* rpm_max/1000 + 1 */
    for (int i = 0; i < rpm_marker_count; i++) {
        rpm_markers[i] = lv_arc_create(scr);
        position_markers(rpm_markers[i], i);
    }
}

/**
 * @brief Create RPM redline indicator
 */
void make_rpm_redline(void) {
    lv_obj_t *rpm_redline_marker = lv_arc_create(scr);
    lv_obj_set_size(rpm_redline_marker, dimension, dimension);
    lv_obj_align(rpm_redline_marker, LV_ALIGN_CENTER, 0, 20);
    lv_arc_set_bg_angles(rpm_redline_marker, 
                        arc_start + ((arc_end - arc_start) * ((float)rpm_redline / rpm_max)), arc_end);
    
    lv_obj_set_style_arc_color(rpm_redline_marker, palette_red, LV_PART_MAIN);
    lv_obj_set_style_arc_rounded(rpm_redline_marker, true, LV_PART_MAIN);
    lv_obj_set_style_arc_width(rpm_redline_marker, rpm_line_width * 2, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(rpm_redline_marker, 0, LV_PART_MAIN);
    
    lv_obj_remove_style(rpm_redline_marker, NULL, LV_PART_KNOB);
    lv_obj_remove_style(rpm_redline_marker, NULL, LV_PART_INDICATOR);
}

/**
 * @brief Create RPM green zone indicator
 */
void make_rpm_greenzone(void) {
    const int green_start_rpm = 0;
    const int green_end_rpm = 1500;
    
    lv_obj_t *rpm_greenzone_marker = lv_arc_create(scr);
    lv_obj_set_size(rpm_greenzone_marker, dimension, dimension);
    lv_obj_align(rpm_greenzone_marker, LV_ALIGN_CENTER, 0, 20);
    lv_arc_set_bg_angles(rpm_greenzone_marker,
                        arc_start + ((arc_end - arc_start) * ((float)green_start_rpm / rpm_max)),
                        arc_start + ((arc_end - arc_start) * ((float)green_end_rpm / rpm_max)));
    
    lv_obj_set_style_arc_color(rpm_greenzone_marker, lv_color_make(0, 255, 0), LV_PART_MAIN);
    lv_obj_set_style_arc_rounded(rpm_greenzone_marker, true, LV_PART_MAIN);
    lv_obj_set_style_arc_width(rpm_greenzone_marker, rpm_line_width * 2, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(rpm_greenzone_marker, 0, LV_PART_MAIN);
    
    lv_obj_remove_style(rpm_greenzone_marker, NULL, LV_PART_KNOB);
    lv_obj_remove_style(rpm_greenzone_marker, NULL, LV_PART_INDICATOR);
}

/**
 * @brief Position RPM markers at specified positions
 * @param marker Marker object to position
 * @param position Position index for the marker
 */
void position_markers(lv_obj_t *marker, int position) {
    lv_obj_set_size(marker, rpm_arc_size + (rpm_line_width * 8), rpm_arc_size + (rpm_line_width * 8));
    lv_obj_align(marker, LV_ALIGN_CENTER, 0, 20);
    
    lv_obj_set_style_arc_width(marker, rpm_line_width + 2, LV_PART_MAIN);
    lv_obj_set_style_arc_rounded(marker, false, LV_PART_MAIN);
    lv_obj_set_style_arc_color(marker, lv_color_make(0, 200, 255), LV_PART_MAIN);
    
    lv_arc_set_bg_angles(marker, arc_start + (position * marker_gap), arc_start + (position * marker_gap) + 1);
    lv_obj_remove_style(marker, NULL, LV_PART_KNOB);
    lv_obj_remove_style(marker, NULL, LV_PART_INDICATOR);
}

/**
 * @brief Create digital RPM display
 */
void make_rpm_digital(void) {
    static lv_style_t style_rpm_text;
    lv_style_init(&style_rpm_text);
    lv_style_set_text_font(&style_rpm_text, &lv_font_montserrat_30);
    lv_style_set_text_color(&style_rpm_text, palette_white);
    
    rpm_label = lv_label_create(scr);
    lv_label_set_text(rpm_label, "2500");
    lv_obj_add_style(rpm_label, &style_rpm_text, 0);
    lv_obj_align(rpm_label, LV_ALIGN_LEFT_MID, 360, 10);
    
    lv_obj_t *rpm_unit_label = lv_label_create(scr);
    lv_label_set_text(rpm_unit_label, "rpm");
    lv_obj_add_style(rpm_unit_label, &style_unit_text, 0);
    lv_obj_align(rpm_unit_label, LV_ALIGN_LEFT_MID, 360, 40);
}

//==============================================================================
// UI CREATION FUNCTIONS - SPEED
//==============================================================================

/**
 * @brief Create digital speed display
 */
void make_speed_digital(void) {
    static lv_style_t style_speed_text;
    lv_style_init(&style_speed_text);
    lv_style_set_text_font(&style_speed_text, &lv_font_montserrat_48);
    lv_style_set_text_color(&style_speed_text, palette_white);
    
    mph_label = lv_label_create(scr);
    lv_label_set_text(mph_label, "85");
    lv_obj_add_style(mph_label, &style_speed_text, 0);
    lv_obj_align(mph_label, LV_ALIGN_CENTER, 0, 20);
    
    lv_obj_t *mph_unit_label = lv_label_create(scr);
    lv_label_set_text(mph_unit_label, "kph");
    lv_obj_add_style(mph_unit_label, &style_unit_text, 0);
    lv_obj_align(mph_unit_label, LV_ALIGN_CENTER, 40, 60);
}

//==============================================================================
// UI CREATION FUNCTIONS - GEAR
//==============================================================================

/**
 * @brief Create gear display with canvas background
 */
void make_gear(void) {
    // Calculate minimal canvas size and buffer for trapezoid
    // Polygon points: top width=30, bottom width=60, height=35
    // Minimal bounding box: x range 10-70, y range 20-55
    #define GEAR_CANVAS_WIDTH 60
    #define GEAR_CANVAS_HEIGHT 35
    static lv_color_t canvas_buf[LV_CANVAS_BUF_SIZE_TRUE_COLOR(GEAR_CANVAS_WIDTH, GEAR_CANVAS_HEIGHT)];
    gear_canvas = lv_canvas_create(scr);
    lv_canvas_set_buffer(gear_canvas, canvas_buf, GEAR_CANVAS_WIDTH, GEAR_CANVAS_HEIGHT, LV_IMG_CF_TRUE_COLOR);
    lv_obj_set_size(gear_canvas, GEAR_CANVAS_WIDTH, GEAR_CANVAS_HEIGHT);
    lv_obj_align(gear_canvas, LV_ALIGN_CENTER, 0, 120);
    lv_obj_set_style_border_width(gear_canvas, 0, 0);
    lv_obj_set_style_bg_opa(gear_canvas, LV_OPA_0, 0);
    
    lv_point_t points[] = {
        {25-10, 20-20}, // shift to fit minimal canvas (origin at 10,20)
        {55-10, 20-20},
        {70-10, 55-20},
        {10-10, 55-20},
    };
    
    lv_draw_rect_dsc_t gear_bg_style;
    lv_draw_rect_dsc_init(&gear_bg_style);
    gear_bg_style.bg_color = lv_color_make(20, 40, 60);
    gear_bg_style.bg_opa = LV_OPA_COVER;
    gear_bg_style.border_width = 2;
    gear_bg_style.border_color = lv_color_make(0, 200, 255);
    gear_bg_style.border_opa = LV_OPA_COVER;
    lv_canvas_draw_polygon(gear_canvas, points, 4, &gear_bg_style);
    
    gear_label = lv_label_create(scr);
    static lv_style_t style_gear_text;
    lv_style_init(&style_gear_text);
    lv_style_set_text_font(&style_gear_text, &lv_font_montserrat_30);
    lv_style_set_text_color(&style_gear_text, palette_amber);
    lv_label_set_text(gear_label, "N");
    lv_obj_add_style(gear_label, &style_gear_text, 0);
    lv_obj_align(gear_label, LV_ALIGN_CENTER, 0, 120);
}

//==============================================================================
// UI CREATION FUNCTIONS - ODOMETER
//==============================================================================

/**
 * @brief Create ODO display with title above the box
 */
void make_odo(void) {
    static lv_style_t style_odo_title;
    lv_style_init(&style_odo_title);
    lv_style_set_text_font(&style_odo_title, &lv_font_montserrat_12);
    lv_style_set_text_color(&style_odo_title, palette_white);
    
    lv_obj_t *odo_box = lv_obj_create(scr);
    lv_obj_set_size(odo_box, 110, 40); // Fixed size to prevent resizing
    lv_obj_set_style_bg_color(odo_box, lv_color_make(15, 15, 25), 0);
    lv_obj_set_style_bg_opa(odo_box, LV_OPA_80, 0);
    lv_obj_set_style_border_color(odo_box, lv_color_make(0, 200, 255), 0);
    lv_obj_set_style_border_width(odo_box, 2, 0);
    lv_obj_set_style_border_opa(odo_box, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(odo_box, 10, 0);
    lv_obj_set_style_pad_left(odo_box, 2, 0);
    lv_obj_set_style_pad_right(odo_box, 2, 0);
    lv_obj_set_style_pad_top(odo_box, 0, 0);
    lv_obj_set_style_pad_bottom(odo_box, 0, 0);
    lv_obj_set_style_shadow_width(odo_box, 15, 0);
    lv_obj_set_style_shadow_color(odo_box, lv_color_make(0, 200, 255), 0);
    lv_obj_set_style_shadow_opa(odo_box, LV_OPA_50, 0);
    lv_obj_set_style_shadow_spread(odo_box, 2, 0);
    lv_obj_align_to(odo_box, gear_canvas, LV_ALIGN_OUT_RIGHT_MID, 15, -3);
    
    odo_title_label = lv_label_create(scr);
    lv_label_set_text(odo_title_label, "ODO");
    lv_obj_add_style(odo_title_label, &style_odo_title, 0);
    lv_obj_align_to(odo_title_label, odo_box, LV_ALIGN_OUT_TOP_LEFT, 0, 0);
    
    static lv_style_t style_odo_value;
    lv_style_init(&style_odo_value);
    lv_style_set_text_font(&style_odo_value, &lv_font_montserrat_20);
    lv_style_set_text_color(&style_odo_value, lv_color_make(0, 255, 200));
    lv_style_set_text_letter_space(&style_odo_value, 3);
    
    odo_value_label = lv_label_create(odo_box);
    lv_label_set_text(odo_value_label, "000001");
    lv_obj_add_style(odo_value_label, &style_odo_value, 0);
    lv_obj_center(odo_value_label);
    lv_label_set_long_mode(odo_value_label, LV_LABEL_LONG_CLIP); // Prevent shifting
    lv_obj_set_width(odo_value_label, 90); // Increased width for text alignment and to avoid digit clipping
    
    static lv_style_t style_km_text;
    lv_style_init(&style_km_text);
    lv_style_set_text_font(&style_km_text, &lv_font_montserrat_12);
    lv_style_set_text_color(&style_km_text, lv_color_make(0, 200, 255));
    
    odo_km_label = lv_label_create(scr);
    lv_label_set_text(odo_km_label, "km");
    lv_obj_add_style(odo_km_label, &style_km_text, 0);
    lv_obj_align_to(odo_km_label, odo_box, LV_ALIGN_OUT_RIGHT_MID, 8, 10);
}

//==============================================================================
// UI CREATION FUNCTIONS - ICONS
//==============================================================================

/**
 * @brief Create gas pump icon
 */
void make_gas_pump_icon(void) {
    LV_IMG_DECLARE(gas_pump_solid);
    // Center of fuel gauge arc
    int center_x = 80;
    int center_y = 120;
    gas_pump_img = lv_img_create(scr);
    lv_img_set_src(gas_pump_img, &gas_pump_solid);
    lv_obj_align(gas_pump_img, LV_ALIGN_TOP_LEFT, center_x - (gas_pump_solid.header.w)/2, center_y - (gas_pump_solid.header.h)/2 + 20);
    lv_obj_set_style_img_recolor(gas_pump_img, lv_color_make(0, 255, 0), 0);
    lv_obj_set_style_img_recolor_opa(gas_pump_img, LV_OPA_COVER, 0);
}

/**
 * @brief Create high beam icon
 */
void make_high_beam_icon(void) {
    LV_IMG_DECLARE(high_beam);
    
    high_beam_img = lv_img_create(scr);
    lv_img_set_src(high_beam_img, &high_beam);
    lv_obj_align_to(high_beam_img, mph_label, LV_ALIGN_OUT_TOP_MID, 0, -10);
    
    lv_obj_set_style_img_recolor(high_beam_img, lv_color_make(0, 100, 255), 0);
    lv_obj_set_style_img_recolor_opa(high_beam_img, LV_OPA_COVER, 0);
}

/**
 * @brief Create racing helmet icon
 */
void make_racing_helmet_icon(void) {
    LV_IMG_DECLARE(racing_helmet);
    
    racing_helmet_img = lv_img_create(scr);
    lv_img_set_src(racing_helmet_img, &racing_helmet);
    lv_obj_align_to(racing_helmet_img, mph_label, LV_ALIGN_OUT_RIGHT_MID, 15, 0);
    
    lv_obj_set_style_img_recolor(racing_helmet_img, lv_color_make(255, 215, 0), 0);
    lv_obj_set_style_img_recolor_opa(racing_helmet_img, LV_OPA_COVER, 0);
}

//==============================================================================
// UI CREATION FUNCTIONS - BORDER FRAME
//==============================================================================

/**
 * @brief Create double border frame on all sides of LCD, avoiding title area
 */
void create_lcd_border_frame(void) {
    // === OUTER BORDER ELEMENTS (flush to LCD edges) ===
    lv_obj_t *outer_top = lv_obj_create(scr);
    lv_obj_set_size(outer_top, LV_HOR_RES, 3);
    lv_obj_align(outer_top, LV_ALIGN_TOP_LEFT, 0, 0); // Top edge
    lv_obj_set_style_bg_color(outer_top, lv_color_make(100, 150, 255), 0);
    lv_obj_set_style_bg_opa(outer_top, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(outer_top, 0, 0);

    lv_obj_t *outer_left = lv_obj_create(scr);
    lv_obj_set_size(outer_left, 3, LV_VER_RES);
    lv_obj_align(outer_left, LV_ALIGN_TOP_LEFT, 0, 0); // Left edge
    lv_obj_set_style_bg_color(outer_left, lv_color_make(100, 150, 255), 0);
    lv_obj_set_style_bg_opa(outer_left, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(outer_left, 0, 0);

    lv_obj_t *outer_right = lv_obj_create(scr);
    lv_obj_set_size(outer_right, 3, LV_VER_RES);
    lv_obj_align(outer_right, LV_ALIGN_TOP_RIGHT, 0, 0); // Right edge
    lv_obj_set_style_bg_color(outer_right, lv_color_make(100, 150, 255), 0);
    lv_obj_set_style_bg_opa(outer_right, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(outer_right, 0, 0);

    lv_obj_t *outer_bottom = lv_obj_create(scr);
    lv_obj_set_size(outer_bottom, LV_HOR_RES, 3);
    lv_obj_align(outer_bottom, LV_ALIGN_BOTTOM_LEFT, 0, 0); // Bottom edge
    lv_obj_set_style_bg_color(outer_bottom, lv_color_make(100, 150, 255), 0);
    lv_obj_set_style_bg_opa(outer_bottom, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(outer_bottom, 0, 0);

    // === CONTRAST BOX (inset 6px from border, 30px height) ===
    lv_obj_t *contrast_box = lv_obj_create(scr);
    lv_obj_set_size(contrast_box, LV_HOR_RES - 12, 30); // Inset 6px left/right
    lv_obj_align(contrast_box, LV_ALIGN_TOP_LEFT, 6, 6); // 6px inset from top/left
    lv_obj_set_style_bg_color(contrast_box, lv_color_make(30, 30, 30), 0); // Contrast color
    lv_obj_set_style_bg_opa(contrast_box, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(contrast_box, 0, 0);
    lv_obj_add_flag(contrast_box, LV_OBJ_FLAG_IGNORE_LAYOUT);
    lv_obj_move_to_index(contrast_box, 0);
    lv_obj_move_background(contrast_box);

    // --- Center message label inside contrast box ---
    center_message_label = lv_label_create(contrast_box);
    lv_label_set_text(center_message_label, "Welcome!");
    lv_obj_set_style_text_color(center_message_label, lv_color_make(0,255,0), 0);
    lv_obj_set_style_text_font(center_message_label, &lv_font_montserrat_20, 0);
    lv_obj_center(center_message_label);

    // === INNER BORDER ELEMENTS (6px inset from outer, below contrast box) ===
    lv_obj_t *inner_top = lv_obj_create(scr);
    lv_obj_set_size(inner_top, LV_HOR_RES - 12, 2);
    lv_obj_align(inner_top, LV_ALIGN_TOP_LEFT, 6, 36); // 6px inset, below contrast box
    lv_obj_set_style_bg_color(inner_top, lv_color_make(100, 150, 255), 0);
    lv_obj_set_style_bg_opa(inner_top, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(inner_top, 0, 0);

    lv_obj_t *inner_left = lv_obj_create(scr);
    lv_obj_set_size(inner_left, 2, LV_VER_RES - 42);
    lv_obj_align(inner_left, LV_ALIGN_TOP_LEFT, 6, 36);
    lv_obj_set_style_bg_color(inner_left, lv_color_make(100, 150, 255), 0);
    lv_obj_set_style_bg_opa(inner_left, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(inner_left, 0, 0);

    lv_obj_t *inner_right = lv_obj_create(scr);
    lv_obj_set_size(inner_right, 2, LV_VER_RES - 42);
    lv_obj_align(inner_right, LV_ALIGN_TOP_RIGHT, -6, 36);
    lv_obj_set_style_bg_color(inner_right, lv_color_make(100, 150, 255), 0);
    lv_obj_set_style_bg_opa(inner_right, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(inner_right, 0, 0);

    lv_obj_t *inner_bottom = lv_obj_create(scr);
    lv_obj_set_size(inner_bottom, LV_HOR_RES - 12, 2);
    lv_obj_align(inner_bottom, LV_ALIGN_BOTTOM_LEFT, 6, -6);
    lv_obj_set_style_bg_color(inner_bottom, lv_color_make(100, 150, 255), 0);
    lv_obj_set_style_bg_opa(inner_bottom, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(inner_bottom, 0, 0);

    lv_obj_t *borders[] = {outer_top, outer_left, outer_right, outer_bottom,
                          inner_top, inner_left, inner_right, inner_bottom};
    for(int i = 0; i < 8; i++) {
        lv_obj_add_flag(borders[i], LV_OBJ_FLAG_IGNORE_LAYOUT);
        lv_obj_move_to_index(borders[i], 0);
        lv_obj_move_background(borders[i]);
    }

    // Create green arrows
    create_right_arrow();
    create_left_arrow();
}

void create_left_arrow(void) {
    if (left_arrow) return; // Only create once
    static lv_color_t left_arrow_buf[LV_CANVAS_BUF_SIZE_TRUE_COLOR(60, 30)];
    left_arrow = lv_canvas_create(scr);
    lv_canvas_set_buffer(left_arrow, left_arrow_buf, 60, 30, LV_IMG_CF_TRUE_COLOR);
    lv_obj_set_size(left_arrow, 60, 30);
    lv_obj_align(left_arrow, LV_ALIGN_TOP_RIGHT, -6, 6);
    lv_obj_set_style_bg_opa(left_arrow, LV_OPA_0, 0);
    lv_canvas_fill_bg(left_arrow, lv_color_make(30, 30, 30), LV_OPA_COVER);
    lv_draw_rect_dsc_t rect_style;
    lv_draw_rect_dsc_init(&rect_style);
    rect_style.bg_color = lv_color_make(0, 255, 0);
    rect_style.bg_opa = LV_OPA_COVER;
    rect_style.border_width = 0;
    lv_canvas_draw_rect(left_arrow, 7, 10, 30, 10, &rect_style);
    lv_point_t tri[] = { {37, 0}, {59, 15}, {37, 30} };
    lv_draw_rect_dsc_t arrow_style;
    lv_draw_rect_dsc_init(&arrow_style);
    arrow_style.bg_color = lv_color_make(0, 255, 0);
    arrow_style.bg_opa = LV_OPA_COVER;
    arrow_style.border_width = 0;
    lv_canvas_draw_polygon(left_arrow, tri, 3, &arrow_style);
}

void create_right_arrow(void) {
    if (right_arrow) return;
    static lv_color_t right_arrow_buf[LV_CANVAS_BUF_SIZE_TRUE_COLOR(60, 30)];
    right_arrow = lv_canvas_create(scr);
    lv_canvas_set_buffer(right_arrow, right_arrow_buf, 60, 30, LV_IMG_CF_TRUE_COLOR);
    lv_obj_set_size(right_arrow, 60, 30);
    lv_obj_align(right_arrow, LV_ALIGN_TOP_LEFT, 6, 6);
    lv_obj_set_style_bg_opa(right_arrow, LV_OPA_0, 0);
    lv_canvas_fill_bg(right_arrow, lv_color_make(30, 30, 30), LV_OPA_COVER);
    lv_draw_rect_dsc_t rect_style;
    lv_draw_rect_dsc_init(&rect_style);
    rect_style.bg_color = lv_color_make(0, 255, 0);
    rect_style.bg_opa = LV_OPA_COVER;
    rect_style.border_width = 0;
    lv_canvas_draw_rect(right_arrow, 24, 10, 30, 10, &rect_style);
    lv_point_t tri[] = { {23, 0}, {1, 15}, {23, 30} };
    lv_draw_rect_dsc_t arrow_style;
    lv_draw_rect_dsc_init(&arrow_style);
    arrow_style.bg_color = lv_color_make(0, 255, 0);
    arrow_style.bg_opa = LV_OPA_COVER;
    arrow_style.border_width = 0;
    lv_canvas_draw_polygon(right_arrow, tri, 3, &arrow_style);
}

static uint32_t left_arrow_blink_start = 0;
static uint32_t right_arrow_blink_start = 0;
bool left_arrow_blinking = false;
bool right_arrow_blinking = false;
static bool left_arrow_visible = true;
static bool right_arrow_visible = true;

void start_left_arrow_blink(void) {
    left_arrow_blinking = true;
    left_arrow_blink_start = ms_tick;
    left_arrow_visible = true;
    lv_obj_clear_flag(left_arrow, LV_OBJ_FLAG_HIDDEN);
}

void start_right_arrow_blink(void) {
    right_arrow_blinking = true;
    right_arrow_blink_start = ms_tick;
    right_arrow_visible = true;
    lv_obj_clear_flag(right_arrow, LV_OBJ_FLAG_HIDDEN);
}

void arrow_blink_task(void) {
    // 750ms blink interval, 5 cycles on/off (10 total transitions)
    const uint32_t blink_cycles = 5; // 5 on, 5 off
    const uint32_t blink_total_time = blink_interval * blink_cycles * 2; // 7500ms
    if (left_arrow_blinking) {
        uint32_t elapsed = ms_tick - left_arrow_blink_start;
        if (elapsed >= blink_total_time) {
            left_arrow_blinking = false;
            lv_obj_clear_flag(left_arrow, LV_OBJ_FLAG_HIDDEN);
        } else if ((elapsed / blink_interval) % 2 == 0) {
            if (!left_arrow_visible) {
                lv_obj_clear_flag(left_arrow, LV_OBJ_FLAG_HIDDEN);
                left_arrow_visible = true;
            }
        } else {
            if (left_arrow_visible) {
                lv_obj_add_flag(left_arrow, LV_OBJ_FLAG_HIDDEN);
                left_arrow_visible = false;
            }
        }
    }
    if (right_arrow_blinking) {
        uint32_t elapsed = ms_tick - right_arrow_blink_start;
        if (elapsed >= blink_total_time) {
            right_arrow_blinking = false;
            lv_obj_clear_flag(right_arrow, LV_OBJ_FLAG_HIDDEN);
        } else if ((elapsed / blink_interval) % 2 == 0) {
            if (!right_arrow_visible) {
                lv_obj_clear_flag(right_arrow, LV_OBJ_FLAG_HIDDEN);
                right_arrow_visible = true;
            }
        } else {
            if (right_arrow_visible) {
                lv_obj_add_flag(right_arrow, LV_OBJ_FLAG_HIDDEN);
                right_arrow_visible = false;
            }
        }
    }
    // Animation text blinking logic
    if (animation_text_active) {
        uint32_t now = ms_tick;
        if ((now - animation_text_last_toggle) > blink_interval) { // Use same interval as arrows
            animation_text_last_toggle = now;
            animation_text_visible = !animation_text_visible;
            if (center_message_label) {
                lv_obj_set_style_opa(center_message_label, animation_text_visible ? LV_OPA_COVER : LV_OPA_TRANSP, 0);
            }
        }
    } else {
        // Always show text if not animating
        if (center_message_label) {
            lv_obj_set_style_opa(center_message_label, LV_OPA_COVER, 0);
        }
    }
}

/**
 * @brief Add a setter for animation_text_active so main.c can control blinking
 * @param active Boolean to enable or disable animation text blinking
 */
void set_animation_text_active(bool active) {
    animation_text_active = active;
    animation_text_last_toggle = ms_tick;
    animation_text_visible = true;
    if (center_message_label) {
        lv_obj_set_style_opa(center_message_label, LV_OPA_COVER, 0);
    }
}

/**
 * @brief Create TXN logo
 */
void create_txn_logo(void) {
    lv_obj_t *txn_logo_img = lv_img_create(scr);
    lv_img_set_src(txn_logo_img, &TXN_logo);
    lv_obj_align(txn_logo_img, LV_ALIGN_BOTTOM_LEFT, 15, -15);
    
    lv_obj_set_style_img_recolor(txn_logo_img, lv_color_make(255, 0, 0), 0);
    lv_obj_set_style_img_recolor_opa(txn_logo_img, LV_OPA_COVER, 0);
}

//==============================================================================
// MAIN UI BUILD FUNCTION
//==============================================================================
/**
 * @brief Build the complete speedometer UI
 */

void Build_UI(void) {
    scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, palette_black, 0);
    make_styles();
    create_lcd_border_frame();
    create_txn_logo();
    make_fuel_gauge();
    make_fuel_low_arc();
    make_rpm_arc();
    make_rpm_border();
    make_rpm_redline();
    make_rpm_greenzone();
    make_rpm_digital();
    make_speed_digital();
    make_high_beam_icon();
    make_gas_pump_icon();
    make_racing_helmet_icon();
    make_gear();
    make_odo();
    make_time_area();
}

void set_center_message(const char *msg) {
    if (center_message_label) {
        lv_label_set_text(center_message_label, msg);
    }
}

void set_center_message_color(lv_color_t color) {
    if (center_message_label) {
        lv_obj_set_style_text_color(center_message_label, color, 0);
    }
}

void make_time_area(void) {
    time_box = lv_obj_create(scr);
    lv_obj_set_size(time_box, 110, 40);
    lv_obj_set_style_bg_color(time_box, lv_color_make(15, 15, 25), 0);
    lv_obj_set_style_bg_opa(time_box, LV_OPA_80, 0);
    lv_obj_set_style_border_color(time_box, lv_color_make(0, 200, 255), 0);
    lv_obj_set_style_border_width(time_box, 2, 0);
    lv_obj_set_style_border_opa(time_box, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(time_box, 10, 0);
    lv_obj_set_style_pad_left(time_box, 2, 0);
    lv_obj_set_style_pad_right(time_box, 2, 0);
    lv_obj_set_style_pad_top(time_box, 0, 0);
    lv_obj_set_style_pad_bottom(time_box, 0, 0);
    lv_obj_set_style_shadow_width(time_box, 15, 0);
    lv_obj_set_style_shadow_color(time_box, lv_color_make(0, 200, 255), 0);
    lv_obj_set_style_shadow_opa(time_box, LV_OPA_50, 0);
    lv_obj_set_style_shadow_spread(time_box, 2, 0);
    lv_obj_align(time_box, LV_ALIGN_TOP_LEFT, 350, 55);

    static lv_style_t style_time_text;
    lv_style_init(&style_time_text);
    lv_style_set_text_font(&style_time_text, &lv_font_montserrat_20);
    lv_style_set_text_color(&style_time_text, palette_amber);
    time_label = lv_label_create(time_box);
    lv_label_set_text(time_label, "00:00:00");
    lv_obj_add_style(time_label, &style_time_text, 0);
    lv_obj_center(time_label);
    lv_label_set_long_mode(time_label, LV_LABEL_LONG_CLIP);
    lv_obj_set_width(time_label, 80);
}

void update_time_label(uint32_t ms_tick) {
    static uint32_t last_update = 0;
    static uint32_t seconds = 0;
    if (ms_tick - last_update >= 1000) {
        last_update = ms_tick;
        seconds++;
        uint32_t hrs = (seconds / 3600) % 24;
        uint32_t mins = (seconds / 60) % 60;
        uint32_t secs = seconds % 60;
        static char time_str[9];
        snprintf(time_str, sizeof(time_str), "%02lu:%02lu:%02lu", (unsigned long)hrs, (unsigned long)mins, (unsigned long)secs);
        if (time_label) {
            lv_label_set_text(time_label, time_str);
        }
    }
}