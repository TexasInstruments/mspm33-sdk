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

#ifndef LV_ATTRIBUTE_IMG_HIGH_BEAM
#define LV_ATTRIBUTE_IMG_HIGH_BEAM
#endif

const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMG_HIGH_BEAM uint8_t high_beam_map[] = {
  0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 
  0xfe, 0x7f, 0x00, 
  0xfc, 0xc1, 0x80, 
  0x00, 0x80, 0xc0, 
  0xfe, 0x80, 0x60, 
  0x01, 0x80, 0x30, 
  0x7d, 0x80, 0x10, 
  0xfd, 0x80, 0x10, 
  0x01, 0x80, 0x30, 
  0xfe, 0x80, 0x60, 
  0x00, 0x80, 0xc0, 
  0x00, 0xc1, 0x80, 
  0xfe, 0x7f, 0x00, 
  0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 
};

const lv_img_dsc_t high_beam = {
    .header.always_zero = 0,
    .header.w = 20,    // Replace with actual width
    .header.h = 20,   // Replace with actual height
    .data_size = sizeof(high_beam_map),
    .header.cf = LV_IMG_CF_ALPHA_1BIT,  // Common format
    .data = high_beam_map,
};