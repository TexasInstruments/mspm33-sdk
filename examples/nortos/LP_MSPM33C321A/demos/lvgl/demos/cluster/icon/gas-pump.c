#ifdef __has_include
    #if __has_include("lvgl.h")
        #ifndef LV_LVGL_H_INCLUDE_SIMPLE
            #define LV_LVGL_H_INCLUDE_SIMPLE
        #endif
    #endif
#endif

#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
    #include "lvgl.h"
#else
    #include "lvgl/lvgl.h"
#endif


#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif

#ifndef LV_ATTRIBUTE_IMG_GAS_PUMP_SOLID
#define LV_ATTRIBUTE_IMG_GAS_PUMP_SOLID
#endif

const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMG_GAS_PUMP_SOLID uint8_t gas_pump_solid_map[] = {
  0x1f, 0xe0, 0x00, 
  0x3f, 0xf1, 0x00, 
  0x30, 0x31, 0x80, 
  0x70, 0x30, 0xc0, 
  0x70, 0x30, 0xe0, 
  0x70, 0x30, 0xf0, 
  0x70, 0x30, 0xf0, 
  0x70, 0x30, 0x70, 
  0x7f, 0xf0, 0x30, 
  0x7f, 0xf0, 0x30, 
  0x7f, 0xfc, 0x30, 
  0x7f, 0xfe, 0x30, 
  0x7f, 0xf3, 0x30, 
  0x7f, 0xf3, 0x30, 
  0x7f, 0xf3, 0x30, 
  0x7f, 0xf1, 0xf0, 
  0x00, 0x00, 0xe0, 
  0x00, 0x00, 0x00, 
  0xff, 0xfc, 0x00, 
  0xff, 0xf8, 0x00, 
};

const lv_img_dsc_t gas_pump_solid = {
    .header.always_zero = 0,
    .header.w = 20,    // Replace with actual width
    .header.h = 20,   // Replace with actual height
    .data_size = sizeof(gas_pump_solid_map),
    .header.cf = LV_IMG_CF_ALPHA_1BIT,  // Common format
    .data = gas_pump_solid_map,
};