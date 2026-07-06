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

#ifndef LV_ATTRIBUTE_IMG_RACING_HELMET
#define LV_ATTRIBUTE_IMG_RACING_HELMET
#endif

const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMG_RACING_HELMET uint8_t racing_helmet_map[] = {
  0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 
  0x01, 0xe0, 0x00, 
  0x07, 0xfc, 0x00, 
  0x0f, 0xf8, 0x00, 
  0x1f, 0xe0, 0x00, 
  0x3f, 0x87, 0x00, 
  0x3f, 0x9f, 0x80, 
  0x3f, 0x3f, 0xc0, 
  0x3f, 0x9f, 0xc0, 
  0x3f, 0x80, 0x00, 
  0x3f, 0xe0, 0x40, 
  0x1f, 0xff, 0xc0, 
  0x0f, 0xff, 0xc0, 
  0x07, 0xff, 0xc0, 
  0x01, 0xff, 0x80, 
  0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 
};

const lv_img_dsc_t racing_helmet = {
    .header.always_zero = 0,
    .header.w = 20,    // Replace with actual width
    .header.h = 20,   // Replace with actual height
    .data_size = sizeof(racing_helmet_map),
    .header.cf = LV_IMG_CF_ALPHA_1BIT,  // Common format
    .data = racing_helmet_map,
};