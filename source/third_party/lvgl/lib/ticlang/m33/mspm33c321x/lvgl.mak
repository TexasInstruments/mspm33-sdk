SDK_INSTALL_DIR ?= $(abspath ../../../../../../..)

include $(SDK_INSTALL_DIR)/imports.mak

CC = "$(TICLANG_ARMCOMPILER)/bin/tiarmclang"
AR = "$(TICLANG_ARMCOMPILER)/bin/tiarmar"

NAME = lvgl

OBJ_DIR = ../../../../obj/ticlang/m33/mspm33c321x/lvgl
SRC_DIR = ../../../../src

CFLAGS = -I$(TICLANG_ARMCOMPILER)/include "-I$(SDK_INSTALL_DIR)/source" -mcpu=cortex-m33 -march=armv8-m.main -mthumb -D__MSPM33C321A__ -mfloat-abi=hard -mfpu=fpv5-sp-d16 -mlittle-endian -O2 -c
AFLAGS = rc
ASMFLAGS = -I$(TICLANG_ARMCOMPILER)/include 

OBJECTS = $(OBJ_DIR)/lv_disp.o $(OBJ_DIR)/lv_event.o $(OBJ_DIR)/lv_group.o $(OBJ_DIR)/lv_indev.o $(OBJ_DIR)/lv_indev_scroll.o $(OBJ_DIR)/lv_obj.o $(OBJ_DIR)/lv_obj_class.o $(OBJ_DIR)/lv_obj_draw.o $(OBJ_DIR)/lv_obj_pos.o $(OBJ_DIR)/lv_obj_scroll.o $(OBJ_DIR)/lv_obj_style.o $(OBJ_DIR)/lv_obj_style_gen.o $(OBJ_DIR)/lv_obj_tree.o $(OBJ_DIR)/lv_refr.o $(OBJ_DIR)/lv_theme.o $(OBJ_DIR)/lv_gpu_arm2d.o $(OBJ_DIR)/lv_draw.o $(OBJ_DIR)/lv_draw_arc.o $(OBJ_DIR)/lv_draw_img.o $(OBJ_DIR)/lv_draw_label.o $(OBJ_DIR)/lv_draw_layer.o $(OBJ_DIR)/lv_draw_line.o $(OBJ_DIR)/lv_draw_mask.o $(OBJ_DIR)/lv_draw_rect.o $(OBJ_DIR)/lv_draw_transform.o $(OBJ_DIR)/lv_draw_triangle.o $(OBJ_DIR)/lv_img_buf.o $(OBJ_DIR)/lv_img_cache.o $(OBJ_DIR)/lv_img_decoder.o $(OBJ_DIR)/lv_draw_pxp.o $(OBJ_DIR)/lv_draw_pxp_blend.o $(OBJ_DIR)/lv_gpu_nxp_pxp.o $(OBJ_DIR)/lv_gpu_nxp_pxp_osa.o $(OBJ_DIR)/lv_draw_vglite.o $(OBJ_DIR)/lv_draw_vglite_arc.o $(OBJ_DIR)/lv_draw_vglite_blend.o $(OBJ_DIR)/lv_draw_vglite_line.o $(OBJ_DIR)/lv_draw_vglite_rect.o $(OBJ_DIR)/lv_vglite_buf.o $(OBJ_DIR)/lv_vglite_utils.o $(OBJ_DIR)/lv_gpu_d2_draw_label.o $(OBJ_DIR)/lv_gpu_d2_ra6m3.o $(OBJ_DIR)/lv_draw_sdl.o $(OBJ_DIR)/lv_draw_sdl_arc.o $(OBJ_DIR)/lv_draw_sdl_bg.o $(OBJ_DIR)/lv_draw_sdl_composite.o $(OBJ_DIR)/lv_draw_sdl_img.o $(OBJ_DIR)/lv_draw_sdl_label.o $(OBJ_DIR)/lv_draw_sdl_layer.o $(OBJ_DIR)/lv_draw_sdl_line.o $(OBJ_DIR)/lv_draw_sdl_mask.o $(OBJ_DIR)/lv_draw_sdl_polygon.o $(OBJ_DIR)/lv_draw_sdl_rect.o $(OBJ_DIR)/lv_draw_sdl_stack_blur.o $(OBJ_DIR)/lv_draw_sdl_texture_cache.o $(OBJ_DIR)/lv_draw_sdl_utils.o $(OBJ_DIR)/lv_gpu_stm32_dma2d.o $(OBJ_DIR)/lv_draw_sw.o $(OBJ_DIR)/lv_draw_sw_arc.o $(OBJ_DIR)/lv_draw_sw_blend.o $(OBJ_DIR)/lv_draw_sw_dither.o $(OBJ_DIR)/lv_draw_sw_gradient.o $(OBJ_DIR)/lv_draw_sw_img.o $(OBJ_DIR)/lv_draw_sw_layer.o $(OBJ_DIR)/lv_draw_sw_letter.o $(OBJ_DIR)/lv_draw_sw_line.o $(OBJ_DIR)/lv_draw_sw_polygon.o $(OBJ_DIR)/lv_draw_sw_rect.o $(OBJ_DIR)/lv_draw_sw_transform.o $(OBJ_DIR)/lv_gpu_swm341_dma2d.o $(OBJ_DIR)/lv_flex.o $(OBJ_DIR)/lv_grid.o $(OBJ_DIR)/lv_bmp.o $(OBJ_DIR)/lv_ffmpeg.o $(OBJ_DIR)/lv_freetype.o $(OBJ_DIR)/lv_fs_fatfs.o $(OBJ_DIR)/lv_fs_littlefs.o $(OBJ_DIR)/lv_fs_posix.o $(OBJ_DIR)/lv_fs_stdio.o $(OBJ_DIR)/lv_fs_win32.o $(OBJ_DIR)/gifdec.o $(OBJ_DIR)/lv_gif.o $(OBJ_DIR)/lodepng.o $(OBJ_DIR)/lv_png.o $(OBJ_DIR)/lv_qrcode.o $(OBJ_DIR)/qrcodegen.o $(OBJ_DIR)/lv_rlottie.o $(OBJ_DIR)/lv_sjpg.o $(OBJ_DIR)/tjpgd.o $(OBJ_DIR)/lv_tiny_ttf.o $(OBJ_DIR)/lv_extra.o $(OBJ_DIR)/lv_fragment.o $(OBJ_DIR)/lv_fragment_manager.o $(OBJ_DIR)/lv_gridnav.o $(OBJ_DIR)/lv_ime_pinyin.o $(OBJ_DIR)/lv_imgfont.o $(OBJ_DIR)/lv_monkey.o $(OBJ_DIR)/lv_msg.o $(OBJ_DIR)/lv_snapshot.o $(OBJ_DIR)/lv_theme_basic.o $(OBJ_DIR)/lv_theme_default.o $(OBJ_DIR)/lv_theme_mono.o $(OBJ_DIR)/lv_animimg.o $(OBJ_DIR)/lv_calendar.o $(OBJ_DIR)/lv_calendar_header_arrow.o $(OBJ_DIR)/lv_calendar_header_dropdown.o $(OBJ_DIR)/lv_chart.o $(OBJ_DIR)/lv_colorwheel.o $(OBJ_DIR)/lv_imgbtn.o $(OBJ_DIR)/lv_keyboard.o $(OBJ_DIR)/lv_led.o $(OBJ_DIR)/lv_list.o $(OBJ_DIR)/lv_menu.o $(OBJ_DIR)/lv_meter.o $(OBJ_DIR)/lv_msgbox.o $(OBJ_DIR)/lv_span.o $(OBJ_DIR)/lv_spinbox.o $(OBJ_DIR)/lv_spinner.o $(OBJ_DIR)/lv_tabview.o $(OBJ_DIR)/lv_tileview.o $(OBJ_DIR)/lv_win.o $(OBJ_DIR)/lv_font.o $(OBJ_DIR)/lv_font_dejavu_16_persian_hebrew.o $(OBJ_DIR)/lv_font_fmt_txt.o $(OBJ_DIR)/lv_font_loader.o $(OBJ_DIR)/lv_font_montserrat_10.o $(OBJ_DIR)/lv_font_montserrat_12.o $(OBJ_DIR)/lv_font_montserrat_12_subpx.o $(OBJ_DIR)/lv_font_montserrat_14.o $(OBJ_DIR)/lv_font_montserrat_16.o $(OBJ_DIR)/lv_font_montserrat_18.o $(OBJ_DIR)/lv_font_montserrat_20.o $(OBJ_DIR)/lv_font_montserrat_22.o $(OBJ_DIR)/lv_font_montserrat_24.o $(OBJ_DIR)/lv_font_montserrat_26.o $(OBJ_DIR)/lv_font_montserrat_28.o $(OBJ_DIR)/lv_font_montserrat_28_compressed.o $(OBJ_DIR)/lv_font_montserrat_30.o $(OBJ_DIR)/lv_font_montserrat_32.o $(OBJ_DIR)/lv_font_montserrat_34.o $(OBJ_DIR)/lv_font_montserrat_36.o $(OBJ_DIR)/lv_font_montserrat_38.o $(OBJ_DIR)/lv_font_montserrat_40.o $(OBJ_DIR)/lv_font_montserrat_42.o $(OBJ_DIR)/lv_font_montserrat_44.o $(OBJ_DIR)/lv_font_montserrat_46.o $(OBJ_DIR)/lv_font_montserrat_48.o $(OBJ_DIR)/lv_font_montserrat_8.o $(OBJ_DIR)/lv_font_simsun_16_cjk.o $(OBJ_DIR)/lv_font_unscii_16.o $(OBJ_DIR)/lv_font_unscii_8.o $(OBJ_DIR)/lv_hal_disp.o $(OBJ_DIR)/lv_hal_indev.o $(OBJ_DIR)/lv_hal_tick.o $(OBJ_DIR)/lv_anim.o $(OBJ_DIR)/lv_anim_timeline.o $(OBJ_DIR)/lv_area.o $(OBJ_DIR)/lv_async.o $(OBJ_DIR)/lv_bidi.o $(OBJ_DIR)/lv_color.o $(OBJ_DIR)/lv_fs.o $(OBJ_DIR)/lv_gc.o $(OBJ_DIR)/lv_ll.o $(OBJ_DIR)/lv_log.o $(OBJ_DIR)/lv_lru.o $(OBJ_DIR)/lv_math.o $(OBJ_DIR)/lv_mem.o $(OBJ_DIR)/lv_printf.o $(OBJ_DIR)/lv_style.o $(OBJ_DIR)/lv_style_gen.o $(OBJ_DIR)/lv_templ.o $(OBJ_DIR)/lv_timer.o $(OBJ_DIR)/lv_tlsf.o $(OBJ_DIR)/lv_txt.o $(OBJ_DIR)/lv_txt_ap.o $(OBJ_DIR)/lv_utils.o $(OBJ_DIR)/lv_arc.o $(OBJ_DIR)/lv_bar.o $(OBJ_DIR)/lv_btn.o $(OBJ_DIR)/lv_btnmatrix.o $(OBJ_DIR)/lv_canvas.o $(OBJ_DIR)/lv_checkbox.o $(OBJ_DIR)/lv_dropdown.o $(OBJ_DIR)/lv_img.o $(OBJ_DIR)/lv_label.o $(OBJ_DIR)/lv_line.o $(OBJ_DIR)/lv_objx_templ.o $(OBJ_DIR)/lv_roller.o $(OBJ_DIR)/lv_slider.o $(OBJ_DIR)/lv_switch.o $(OBJ_DIR)/lv_table.o $(OBJ_DIR)/lv_textarea.o

all: $(NAME).a

$(NAME).a: $(OBJECTS)
	@ echo Archiving $@
	@ $(AR) $(AFLAGS) $@ $^

$(OBJ_DIR)/lv_disp.o: $(SRC_DIR)/core/lv_disp.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_event.o: $(SRC_DIR)/core/lv_event.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_group.o: $(SRC_DIR)/core/lv_group.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_indev.o: $(SRC_DIR)/core/lv_indev.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_indev_scroll.o: $(SRC_DIR)/core/lv_indev_scroll.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_obj.o: $(SRC_DIR)/core/lv_obj.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_obj_class.o: $(SRC_DIR)/core/lv_obj_class.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_obj_draw.o: $(SRC_DIR)/core/lv_obj_draw.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_obj_pos.o: $(SRC_DIR)/core/lv_obj_pos.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_obj_scroll.o: $(SRC_DIR)/core/lv_obj_scroll.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_obj_style.o: $(SRC_DIR)/core/lv_obj_style.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_obj_style_gen.o: $(SRC_DIR)/core/lv_obj_style_gen.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_obj_tree.o: $(SRC_DIR)/core/lv_obj_tree.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_refr.o: $(SRC_DIR)/core/lv_refr.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_theme.o: $(SRC_DIR)/core/lv_theme.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_gpu_arm2d.o: $(SRC_DIR)/draw/arm2d/lv_gpu_arm2d.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_draw.o: $(SRC_DIR)/draw/lv_draw.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_draw_arc.o: $(SRC_DIR)/draw/lv_draw_arc.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_draw_img.o: $(SRC_DIR)/draw/lv_draw_img.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_draw_label.o: $(SRC_DIR)/draw/lv_draw_label.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_draw_layer.o: $(SRC_DIR)/draw/lv_draw_layer.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_draw_line.o: $(SRC_DIR)/draw/lv_draw_line.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_draw_mask.o: $(SRC_DIR)/draw/lv_draw_mask.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_draw_rect.o: $(SRC_DIR)/draw/lv_draw_rect.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_draw_transform.o: $(SRC_DIR)/draw/lv_draw_transform.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_draw_triangle.o: $(SRC_DIR)/draw/lv_draw_triangle.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_img_buf.o: $(SRC_DIR)/draw/lv_img_buf.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_img_cache.o: $(SRC_DIR)/draw/lv_img_cache.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_img_decoder.o: $(SRC_DIR)/draw/lv_img_decoder.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_draw_pxp.o: $(SRC_DIR)/draw/nxp/pxp/lv_draw_pxp.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_draw_pxp_blend.o: $(SRC_DIR)/draw/nxp/pxp/lv_draw_pxp_blend.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_gpu_nxp_pxp.o: $(SRC_DIR)/draw/nxp/pxp/lv_gpu_nxp_pxp.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_gpu_nxp_pxp_osa.o: $(SRC_DIR)/draw/nxp/pxp/lv_gpu_nxp_pxp_osa.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_draw_vglite.o: $(SRC_DIR)/draw/nxp/vglite/lv_draw_vglite.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_draw_vglite_arc.o: $(SRC_DIR)/draw/nxp/vglite/lv_draw_vglite_arc.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_draw_vglite_blend.o: $(SRC_DIR)/draw/nxp/vglite/lv_draw_vglite_blend.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_draw_vglite_line.o: $(SRC_DIR)/draw/nxp/vglite/lv_draw_vglite_line.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_draw_vglite_rect.o: $(SRC_DIR)/draw/nxp/vglite/lv_draw_vglite_rect.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_vglite_buf.o: $(SRC_DIR)/draw/nxp/vglite/lv_vglite_buf.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_vglite_utils.o: $(SRC_DIR)/draw/nxp/vglite/lv_vglite_utils.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_gpu_d2_draw_label.o: $(SRC_DIR)/draw/renesas/lv_gpu_d2_draw_label.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_gpu_d2_ra6m3.o: $(SRC_DIR)/draw/renesas/lv_gpu_d2_ra6m3.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_draw_sdl.o: $(SRC_DIR)/draw/sdl/lv_draw_sdl.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_draw_sdl_arc.o: $(SRC_DIR)/draw/sdl/lv_draw_sdl_arc.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_draw_sdl_bg.o: $(SRC_DIR)/draw/sdl/lv_draw_sdl_bg.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_draw_sdl_composite.o: $(SRC_DIR)/draw/sdl/lv_draw_sdl_composite.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_draw_sdl_img.o: $(SRC_DIR)/draw/sdl/lv_draw_sdl_img.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_draw_sdl_label.o: $(SRC_DIR)/draw/sdl/lv_draw_sdl_label.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_draw_sdl_layer.o: $(SRC_DIR)/draw/sdl/lv_draw_sdl_layer.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_draw_sdl_line.o: $(SRC_DIR)/draw/sdl/lv_draw_sdl_line.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_draw_sdl_mask.o: $(SRC_DIR)/draw/sdl/lv_draw_sdl_mask.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_draw_sdl_polygon.o: $(SRC_DIR)/draw/sdl/lv_draw_sdl_polygon.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_draw_sdl_rect.o: $(SRC_DIR)/draw/sdl/lv_draw_sdl_rect.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_draw_sdl_stack_blur.o: $(SRC_DIR)/draw/sdl/lv_draw_sdl_stack_blur.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_draw_sdl_texture_cache.o: $(SRC_DIR)/draw/sdl/lv_draw_sdl_texture_cache.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_draw_sdl_utils.o: $(SRC_DIR)/draw/sdl/lv_draw_sdl_utils.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_gpu_stm32_dma2d.o: $(SRC_DIR)/draw/stm32_dma2d/lv_gpu_stm32_dma2d.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_draw_sw.o: $(SRC_DIR)/draw/sw/lv_draw_sw.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_draw_sw_arc.o: $(SRC_DIR)/draw/sw/lv_draw_sw_arc.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_draw_sw_blend.o: $(SRC_DIR)/draw/sw/lv_draw_sw_blend.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_draw_sw_dither.o: $(SRC_DIR)/draw/sw/lv_draw_sw_dither.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_draw_sw_gradient.o: $(SRC_DIR)/draw/sw/lv_draw_sw_gradient.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_draw_sw_img.o: $(SRC_DIR)/draw/sw/lv_draw_sw_img.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_draw_sw_layer.o: $(SRC_DIR)/draw/sw/lv_draw_sw_layer.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_draw_sw_letter.o: $(SRC_DIR)/draw/sw/lv_draw_sw_letter.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_draw_sw_line.o: $(SRC_DIR)/draw/sw/lv_draw_sw_line.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_draw_sw_polygon.o: $(SRC_DIR)/draw/sw/lv_draw_sw_polygon.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_draw_sw_rect.o: $(SRC_DIR)/draw/sw/lv_draw_sw_rect.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_draw_sw_transform.o: $(SRC_DIR)/draw/sw/lv_draw_sw_transform.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_gpu_swm341_dma2d.o: $(SRC_DIR)/draw/swm341_dma2d/lv_gpu_swm341_dma2d.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_flex.o: $(SRC_DIR)/extra/layouts/flex/lv_flex.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_grid.o: $(SRC_DIR)/extra/layouts/grid/lv_grid.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_bmp.o: $(SRC_DIR)/extra/libs/bmp/lv_bmp.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_ffmpeg.o: $(SRC_DIR)/extra/libs/ffmpeg/lv_ffmpeg.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_freetype.o: $(SRC_DIR)/extra/libs/freetype/lv_freetype.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_fs_fatfs.o: $(SRC_DIR)/extra/libs/fsdrv/lv_fs_fatfs.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_fs_littlefs.o: $(SRC_DIR)/extra/libs/fsdrv/lv_fs_littlefs.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_fs_posix.o: $(SRC_DIR)/extra/libs/fsdrv/lv_fs_posix.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_fs_stdio.o: $(SRC_DIR)/extra/libs/fsdrv/lv_fs_stdio.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_fs_win32.o: $(SRC_DIR)/extra/libs/fsdrv/lv_fs_win32.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/gifdec.o: $(SRC_DIR)/extra/libs/gif/gifdec.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_gif.o: $(SRC_DIR)/extra/libs/gif/lv_gif.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lodepng.o: $(SRC_DIR)/extra/libs/png/lodepng.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_png.o: $(SRC_DIR)/extra/libs/png/lv_png.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_qrcode.o: $(SRC_DIR)/extra/libs/qrcode/lv_qrcode.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/qrcodegen.o: $(SRC_DIR)/extra/libs/qrcode/qrcodegen.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_rlottie.o: $(SRC_DIR)/extra/libs/rlottie/lv_rlottie.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_sjpg.o: $(SRC_DIR)/extra/libs/sjpg/lv_sjpg.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/tjpgd.o: $(SRC_DIR)/extra/libs/sjpg/tjpgd.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_tiny_ttf.o: $(SRC_DIR)/extra/libs/tiny_ttf/lv_tiny_ttf.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_extra.o: $(SRC_DIR)/extra/lv_extra.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_fragment.o: $(SRC_DIR)/extra/others/fragment/lv_fragment.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_fragment_manager.o: $(SRC_DIR)/extra/others/fragment/lv_fragment_manager.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_gridnav.o: $(SRC_DIR)/extra/others/gridnav/lv_gridnav.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_ime_pinyin.o: $(SRC_DIR)/extra/others/ime/lv_ime_pinyin.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_imgfont.o: $(SRC_DIR)/extra/others/imgfont/lv_imgfont.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_monkey.o: $(SRC_DIR)/extra/others/monkey/lv_monkey.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_msg.o: $(SRC_DIR)/extra/others/msg/lv_msg.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_snapshot.o: $(SRC_DIR)/extra/others/snapshot/lv_snapshot.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_theme_basic.o: $(SRC_DIR)/extra/themes/basic/lv_theme_basic.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_theme_default.o: $(SRC_DIR)/extra/themes/default/lv_theme_default.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_theme_mono.o: $(SRC_DIR)/extra/themes/mono/lv_theme_mono.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_animimg.o: $(SRC_DIR)/extra/widgets/animimg/lv_animimg.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_calendar.o: $(SRC_DIR)/extra/widgets/calendar/lv_calendar.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_calendar_header_arrow.o: $(SRC_DIR)/extra/widgets/calendar/lv_calendar_header_arrow.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_calendar_header_dropdown.o: $(SRC_DIR)/extra/widgets/calendar/lv_calendar_header_dropdown.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_chart.o: $(SRC_DIR)/extra/widgets/chart/lv_chart.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_colorwheel.o: $(SRC_DIR)/extra/widgets/colorwheel/lv_colorwheel.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_imgbtn.o: $(SRC_DIR)/extra/widgets/imgbtn/lv_imgbtn.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_keyboard.o: $(SRC_DIR)/extra/widgets/keyboard/lv_keyboard.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_led.o: $(SRC_DIR)/extra/widgets/led/lv_led.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_list.o: $(SRC_DIR)/extra/widgets/list/lv_list.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_menu.o: $(SRC_DIR)/extra/widgets/menu/lv_menu.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_meter.o: $(SRC_DIR)/extra/widgets/meter/lv_meter.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_msgbox.o: $(SRC_DIR)/extra/widgets/msgbox/lv_msgbox.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_span.o: $(SRC_DIR)/extra/widgets/span/lv_span.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_spinbox.o: $(SRC_DIR)/extra/widgets/spinbox/lv_spinbox.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_spinner.o: $(SRC_DIR)/extra/widgets/spinner/lv_spinner.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_tabview.o: $(SRC_DIR)/extra/widgets/tabview/lv_tabview.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_tileview.o: $(SRC_DIR)/extra/widgets/tileview/lv_tileview.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_win.o: $(SRC_DIR)/extra/widgets/win/lv_win.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_font.o: $(SRC_DIR)/font/lv_font.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_font_dejavu_16_persian_hebrew.o: $(SRC_DIR)/font/lv_font_dejavu_16_persian_hebrew.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_font_fmt_txt.o: $(SRC_DIR)/font/lv_font_fmt_txt.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_font_loader.o: $(SRC_DIR)/font/lv_font_loader.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_font_montserrat_10.o: $(SRC_DIR)/font/lv_font_montserrat_10.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_font_montserrat_12.o: $(SRC_DIR)/font/lv_font_montserrat_12.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_font_montserrat_12_subpx.o: $(SRC_DIR)/font/lv_font_montserrat_12_subpx.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_font_montserrat_14.o: $(SRC_DIR)/font/lv_font_montserrat_14.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_font_montserrat_16.o: $(SRC_DIR)/font/lv_font_montserrat_16.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_font_montserrat_18.o: $(SRC_DIR)/font/lv_font_montserrat_18.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_font_montserrat_20.o: $(SRC_DIR)/font/lv_font_montserrat_20.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_font_montserrat_22.o: $(SRC_DIR)/font/lv_font_montserrat_22.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_font_montserrat_24.o: $(SRC_DIR)/font/lv_font_montserrat_24.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_font_montserrat_26.o: $(SRC_DIR)/font/lv_font_montserrat_26.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_font_montserrat_28.o: $(SRC_DIR)/font/lv_font_montserrat_28.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_font_montserrat_28_compressed.o: $(SRC_DIR)/font/lv_font_montserrat_28_compressed.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_font_montserrat_30.o: $(SRC_DIR)/font/lv_font_montserrat_30.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_font_montserrat_32.o: $(SRC_DIR)/font/lv_font_montserrat_32.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_font_montserrat_34.o: $(SRC_DIR)/font/lv_font_montserrat_34.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_font_montserrat_36.o: $(SRC_DIR)/font/lv_font_montserrat_36.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_font_montserrat_38.o: $(SRC_DIR)/font/lv_font_montserrat_38.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_font_montserrat_40.o: $(SRC_DIR)/font/lv_font_montserrat_40.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_font_montserrat_42.o: $(SRC_DIR)/font/lv_font_montserrat_42.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_font_montserrat_44.o: $(SRC_DIR)/font/lv_font_montserrat_44.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_font_montserrat_46.o: $(SRC_DIR)/font/lv_font_montserrat_46.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_font_montserrat_48.o: $(SRC_DIR)/font/lv_font_montserrat_48.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_font_montserrat_8.o: $(SRC_DIR)/font/lv_font_montserrat_8.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_font_simsun_16_cjk.o: $(SRC_DIR)/font/lv_font_simsun_16_cjk.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_font_unscii_16.o: $(SRC_DIR)/font/lv_font_unscii_16.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_font_unscii_8.o: $(SRC_DIR)/font/lv_font_unscii_8.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_hal_disp.o: $(SRC_DIR)/hal/lv_hal_disp.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_hal_indev.o: $(SRC_DIR)/hal/lv_hal_indev.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_hal_tick.o: $(SRC_DIR)/hal/lv_hal_tick.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_anim.o: $(SRC_DIR)/misc/lv_anim.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_anim_timeline.o: $(SRC_DIR)/misc/lv_anim_timeline.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_area.o: $(SRC_DIR)/misc/lv_area.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_async.o: $(SRC_DIR)/misc/lv_async.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_bidi.o: $(SRC_DIR)/misc/lv_bidi.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_color.o: $(SRC_DIR)/misc/lv_color.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_fs.o: $(SRC_DIR)/misc/lv_fs.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_gc.o: $(SRC_DIR)/misc/lv_gc.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_ll.o: $(SRC_DIR)/misc/lv_ll.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_log.o: $(SRC_DIR)/misc/lv_log.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_lru.o: $(SRC_DIR)/misc/lv_lru.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_math.o: $(SRC_DIR)/misc/lv_math.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_mem.o: $(SRC_DIR)/misc/lv_mem.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_printf.o: $(SRC_DIR)/misc/lv_printf.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_style.o: $(SRC_DIR)/misc/lv_style.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_style_gen.o: $(SRC_DIR)/misc/lv_style_gen.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_templ.o: $(SRC_DIR)/misc/lv_templ.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_timer.o: $(SRC_DIR)/misc/lv_timer.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_tlsf.o: $(SRC_DIR)/misc/lv_tlsf.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_txt.o: $(SRC_DIR)/misc/lv_txt.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_txt_ap.o: $(SRC_DIR)/misc/lv_txt_ap.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_utils.o: $(SRC_DIR)/misc/lv_utils.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_arc.o: $(SRC_DIR)/widgets/lv_arc.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_bar.o: $(SRC_DIR)/widgets/lv_bar.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_btn.o: $(SRC_DIR)/widgets/lv_btn.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_btnmatrix.o: $(SRC_DIR)/widgets/lv_btnmatrix.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_canvas.o: $(SRC_DIR)/widgets/lv_canvas.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_checkbox.o: $(SRC_DIR)/widgets/lv_checkbox.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_dropdown.o: $(SRC_DIR)/widgets/lv_dropdown.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_img.o: $(SRC_DIR)/widgets/lv_img.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_label.o: $(SRC_DIR)/widgets/lv_label.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_line.o: $(SRC_DIR)/widgets/lv_line.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_objx_templ.o: $(SRC_DIR)/widgets/lv_objx_templ.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_roller.o: $(SRC_DIR)/widgets/lv_roller.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_slider.o: $(SRC_DIR)/widgets/lv_slider.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_switch.o: $(SRC_DIR)/widgets/lv_switch.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_table.o: $(SRC_DIR)/widgets/lv_table.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/lv_textarea.o: $(SRC_DIR)/widgets/lv_textarea.c
	@ echo Building $@
	@ mkdir -p $(dir $@)
	@ $(CC) $(CFLAGS) $< -o $@

clean:
	@ echo Cleaning...
	@ $(RM) $(OBJECTS) > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).a > $(DEVNULL) 2>&1
