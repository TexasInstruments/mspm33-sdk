MSPM33_SDK_INSTALL_DIR ?= $(abspath ../../../../../..)

include $(MSPM33_SDK_INSTALL_DIR)/imports.mak

CC = "$(TICLANG_ARMCOMPILER)/bin/tiarmclang"
LNK = "$(TICLANG_ARMCOMPILER)/bin/tiarmclang"

# Enable verbose output by setting VERBOSE=1
V := @
ifeq ($(VERBOSE), 1)
  V :=
endif

OBJECTS = main.obj lcd_lcd.obj demos_cluster_Speedo_demo.obj demos_cluster_icon_gas-pump.obj demos_cluster_icon_high_beam.obj demos_cluster_icon_racing_helmet.obj demos_cluster_icon_TXN_logo.obj demos_widgets_lv_demo_widgets.obj demos_widgets_assets_img_lvgl_logo.obj demos_widgets_assets_img_demo_widgets_avatar.obj demos_widgets_assets_img_clothes.obj demos_stress_lv_demo_stress.obj demos_music_lv_demo_music_main.obj demos_music_lv_demo_music_list.obj demos_music_lv_demo_music.obj demos_music_assets_img_lv_demo_music_wave_top_large.obj demos_music_assets_img_lv_demo_music_wave_top.obj demos_music_assets_img_lv_demo_music_wave_bottom_large.obj demos_music_assets_img_lv_demo_music_wave_bottom.obj demos_music_assets_img_lv_demo_music_slider_knob_large.obj demos_music_assets_img_lv_demo_music_slider_knob.obj demos_music_assets_img_lv_demo_music_logo.obj demos_music_assets_img_lv_demo_music_list_border_large.obj demos_music_assets_img_lv_demo_music_list_border.obj demos_music_assets_img_lv_demo_music_icon_4_large.obj demos_music_assets_img_lv_demo_music_icon_4.obj demos_music_assets_img_lv_demo_music_icon_3_large.obj demos_music_assets_img_lv_demo_music_icon_3.obj demos_music_assets_img_lv_demo_music_icon_2_large.obj demos_music_assets_img_lv_demo_music_icon_2.obj demos_music_assets_img_lv_demo_music_icon_1_large.obj demos_music_assets_img_lv_demo_music_icon_1.obj demos_music_assets_img_lv_demo_music_cover_3_large.obj demos_music_assets_img_lv_demo_music_cover_3.obj demos_music_assets_img_lv_demo_music_cover_2_large.obj demos_music_assets_img_lv_demo_music_cover_2.obj demos_music_assets_img_lv_demo_music_cover_1_large.obj demos_music_assets_img_lv_demo_music_cover_1.obj demos_music_assets_img_lv_demo_music_corner_right_large.obj demos_music_assets_img_lv_demo_music_corner_right.obj demos_music_assets_img_lv_demo_music_corner_left_large.obj demos_music_assets_img_lv_demo_music_corner_left.obj demos_music_assets_img_lv_demo_music_btn_rnd_large.obj demos_music_assets_img_lv_demo_music_btn_rnd.obj demos_music_assets_img_lv_demo_music_btn_prev_large.obj demos_music_assets_img_lv_demo_music_btn_prev.obj demos_music_assets_img_lv_demo_music_btn_play_large.obj demos_music_assets_img_lv_demo_music_btn_play.obj demos_music_assets_img_lv_demo_music_btn_pause_large.obj demos_music_assets_img_lv_demo_music_btn_pause.obj demos_music_assets_img_lv_demo_music_btn_next_large.obj demos_music_assets_img_lv_demo_music_btn_next.obj demos_music_assets_img_lv_demo_music_btn_loop_large.obj demos_music_assets_img_lv_demo_music_btn_loop.obj demos_music_assets_img_lv_demo_music_btn_list_play_large.obj demos_music_assets_img_lv_demo_music_btn_list_play.obj demos_music_assets_img_lv_demo_music_btn_list_pause_large.obj demos_music_assets_img_lv_demo_music_btn_list_pause.obj demos_music_assets_img_lv_demo_music_btn_corner_large.obj demos_keypad_encoder_lv_demo_keypad_encoder.obj demos_benchmark_lv_demo_benchmark.obj demos_benchmark_assets_lv_font_bechmark_montserrat_28_compr_az.c.obj demos_benchmark_assets_lv_font_bechmark_montserrat_16_compr_az.c.obj demos_benchmark_assets_lv_font_bechmark_montserrat_12_compr_az.c.obj demos_benchmark_assets_img_benchmark_cogwheel_rgb565a8.obj demos_benchmark_assets_img_benchmark_cogwheel_rgb.obj demos_benchmark_assets_img_benchmark_cogwheel_indexed16.obj demos_benchmark_assets_img_benchmark_cogwheel_chroma_keyed.obj demos_benchmark_assets_img_benchmark_cogwheel_argb.obj demos_benchmark_assets_img_benchmark_cogwheel_alpha16.obj spi_spi.obj spi_delay.obj touch_touch.obj ti_msp_dl_config.obj porting_lv_port_fs.obj porting_lv_port_indev.obj porting_lv_port_disp.obj third_party_lvgl_src_core_lv_disp.obj third_party_lvgl_src_core_lv_event.obj third_party_lvgl_src_core_lv_group.obj third_party_lvgl_src_core_lv_indev.obj third_party_lvgl_src_core_lv_indev_scroll.obj third_party_lvgl_src_core_lv_obj.obj third_party_lvgl_src_core_lv_obj_class.obj third_party_lvgl_src_core_lv_obj_draw.obj third_party_lvgl_src_core_lv_obj_pos.obj third_party_lvgl_src_core_lv_obj_scroll.obj third_party_lvgl_src_core_lv_obj_style.obj third_party_lvgl_src_core_lv_obj_style_gen.obj third_party_lvgl_src_core_lv_obj_tree.obj third_party_lvgl_src_core_lv_refr.obj third_party_lvgl_src_core_lv_theme.obj third_party_lvgl_src_draw_arm2d_lv_gpu_arm2d.obj third_party_lvgl_src_draw_lv_draw.obj third_party_lvgl_src_draw_lv_draw_arc.obj third_party_lvgl_src_draw_lv_draw_img.obj third_party_lvgl_src_draw_lv_draw_label.obj third_party_lvgl_src_draw_lv_draw_layer.obj third_party_lvgl_src_draw_lv_draw_line.obj third_party_lvgl_src_draw_lv_draw_mask.obj third_party_lvgl_src_draw_lv_draw_rect.obj third_party_lvgl_src_draw_lv_draw_transform.obj third_party_lvgl_src_draw_lv_draw_triangle.obj third_party_lvgl_src_draw_lv_img_buf.obj third_party_lvgl_src_draw_lv_img_cache.obj third_party_lvgl_src_draw_lv_img_decoder.obj third_party_lvgl_src_draw_nxp_pxp_lv_draw_pxp.obj third_party_lvgl_src_draw_nxp_pxp_lv_draw_pxp_blend.obj third_party_lvgl_src_draw_nxp_pxp_lv_gpu_nxp_pxp.obj third_party_lvgl_src_draw_nxp_pxp_lv_gpu_nxp_pxp_osa.obj third_party_lvgl_src_draw_nxp_vglite_lv_draw_vglite.obj third_party_lvgl_src_draw_nxp_vglite_lv_draw_vglite_arc.obj third_party_lvgl_src_draw_nxp_vglite_lv_draw_vglite_blend.obj third_party_lvgl_src_draw_nxp_vglite_lv_draw_vglite_line.obj third_party_lvgl_src_draw_nxp_vglite_lv_draw_vglite_rect.obj third_party_lvgl_src_draw_nxp_vglite_lv_vglite_buf.obj third_party_lvgl_src_draw_nxp_vglite_lv_vglite_utils.obj third_party_lvgl_src_draw_renesas_lv_gpu_d2_draw_label.obj third_party_lvgl_src_draw_renesas_lv_gpu_d2_ra6m3.obj third_party_lvgl_src_draw_sdl_lv_draw_sdl.obj third_party_lvgl_src_draw_sdl_lv_draw_sdl_arc.obj third_party_lvgl_src_draw_sdl_lv_draw_sdl_bg.obj third_party_lvgl_src_draw_sdl_lv_draw_sdl_composite.obj third_party_lvgl_src_draw_sdl_lv_draw_sdl_img.obj third_party_lvgl_src_draw_sdl_lv_draw_sdl_label.obj third_party_lvgl_src_draw_sdl_lv_draw_sdl_layer.obj third_party_lvgl_src_draw_sdl_lv_draw_sdl_line.obj third_party_lvgl_src_draw_sdl_lv_draw_sdl_mask.obj third_party_lvgl_src_draw_sdl_lv_draw_sdl_polygon.obj third_party_lvgl_src_draw_sdl_lv_draw_sdl_rect.obj third_party_lvgl_src_draw_sdl_lv_draw_sdl_stack_blur.obj third_party_lvgl_src_draw_sdl_lv_draw_sdl_texture_cache.obj third_party_lvgl_src_draw_sdl_lv_draw_sdl_utils.obj third_party_lvgl_src_draw_stm32_dma2d_lv_gpu_stm32_dma2d.obj third_party_lvgl_src_draw_sw_lv_draw_sw.obj third_party_lvgl_src_draw_sw_lv_draw_sw_arc.obj third_party_lvgl_src_draw_sw_lv_draw_sw_blend.obj third_party_lvgl_src_draw_sw_lv_draw_sw_dither.obj third_party_lvgl_src_draw_sw_lv_draw_sw_gradient.obj third_party_lvgl_src_draw_sw_lv_draw_sw_img.obj third_party_lvgl_src_draw_sw_lv_draw_sw_layer.obj third_party_lvgl_src_draw_sw_lv_draw_sw_letter.obj third_party_lvgl_src_draw_sw_lv_draw_sw_line.obj third_party_lvgl_src_draw_sw_lv_draw_sw_polygon.obj third_party_lvgl_src_draw_sw_lv_draw_sw_rect.obj third_party_lvgl_src_draw_sw_lv_draw_sw_transform.obj third_party_lvgl_src_draw_swm341_dma2d_lv_gpu_swm341_dma2d.obj third_party_lvgl_src_extra_layouts_flex_lv_flex.obj third_party_lvgl_src_extra_layouts_grid_lv_grid.obj third_party_lvgl_src_extra_libs_bmp_lv_bmp.obj third_party_lvgl_src_extra_libs_ffmpeg_lv_ffmpeg.obj third_party_lvgl_src_extra_libs_freetype_lv_freetype.obj third_party_lvgl_src_extra_libs_fsdrv_lv_fs_fatfs.obj third_party_lvgl_src_extra_libs_fsdrv_lv_fs_littlefs.obj third_party_lvgl_src_extra_libs_fsdrv_lv_fs_posix.obj third_party_lvgl_src_extra_libs_fsdrv_lv_fs_stdio.obj third_party_lvgl_src_extra_libs_fsdrv_lv_fs_win32.obj third_party_lvgl_src_extra_libs_gif_gifdec.obj third_party_lvgl_src_extra_libs_gif_lv_gif.obj third_party_lvgl_src_extra_libs_png_lodepng.obj third_party_lvgl_src_extra_libs_png_lv_png.obj third_party_lvgl_src_extra_libs_qrcode_lv_qrcode.obj third_party_lvgl_src_extra_libs_qrcode_qrcodegen.obj third_party_lvgl_src_extra_libs_rlottie_lv_rlottie.obj third_party_lvgl_src_extra_libs_sjpg_lv_sjpg.obj third_party_lvgl_src_extra_libs_sjpg_tjpgd.obj third_party_lvgl_src_extra_libs_tiny_ttf_lv_tiny_ttf.obj third_party_lvgl_src_extra_lv_extra.obj third_party_lvgl_src_extra_others_fragment_lv_fragment.obj third_party_lvgl_src_extra_others_fragment_lv_fragment_manager.obj third_party_lvgl_src_extra_others_gridnav_lv_gridnav.obj third_party_lvgl_src_extra_others_ime_lv_ime_pinyin.obj third_party_lvgl_src_extra_others_imgfont_lv_imgfont.obj third_party_lvgl_src_extra_others_monkey_lv_monkey.obj third_party_lvgl_src_extra_others_msg_lv_msg.obj third_party_lvgl_src_extra_others_snapshot_lv_snapshot.obj third_party_lvgl_src_extra_themes_basic_lv_theme_basic.obj third_party_lvgl_src_extra_themes_default_lv_theme_default.obj third_party_lvgl_src_extra_themes_mono_lv_theme_mono.obj third_party_lvgl_src_extra_widgets_animimg_lv_animimg.obj third_party_lvgl_src_extra_widgets_calendar_lv_calendar.obj third_party_lvgl_src_extra_widgets_calendar_lv_calendar_header_arrow.obj third_party_lvgl_src_extra_widgets_calendar_lv_calendar_header_dropdown.obj third_party_lvgl_src_extra_widgets_chart_lv_chart.obj third_party_lvgl_src_extra_widgets_colorwheel_lv_colorwheel.obj third_party_lvgl_src_extra_widgets_imgbtn_lv_imgbtn.obj third_party_lvgl_src_extra_widgets_keyboard_lv_keyboard.obj third_party_lvgl_src_extra_widgets_led_lv_led.obj third_party_lvgl_src_extra_widgets_list_lv_list.obj third_party_lvgl_src_extra_widgets_menu_lv_menu.obj third_party_lvgl_src_extra_widgets_meter_lv_meter.obj third_party_lvgl_src_extra_widgets_msgbox_lv_msgbox.obj third_party_lvgl_src_extra_widgets_span_lv_span.obj third_party_lvgl_src_extra_widgets_spinbox_lv_spinbox.obj third_party_lvgl_src_extra_widgets_spinner_lv_spinner.obj third_party_lvgl_src_extra_widgets_tabview_lv_tabview.obj third_party_lvgl_src_extra_widgets_tileview_lv_tileview.obj third_party_lvgl_src_extra_widgets_win_lv_win.obj third_party_lvgl_src_font_lv_font.obj third_party_lvgl_src_font_lv_font_dejavu_16_persian_hebrew.obj third_party_lvgl_src_font_lv_font_fmt_txt.obj third_party_lvgl_src_font_lv_font_loader.obj third_party_lvgl_src_font_lv_font_montserrat_10.obj third_party_lvgl_src_font_lv_font_montserrat_12.obj third_party_lvgl_src_font_lv_font_montserrat_12_subpx.obj third_party_lvgl_src_font_lv_font_montserrat_14.obj third_party_lvgl_src_font_lv_font_montserrat_16.obj third_party_lvgl_src_font_lv_font_montserrat_18.obj third_party_lvgl_src_font_lv_font_montserrat_20.obj third_party_lvgl_src_font_lv_font_montserrat_22.obj third_party_lvgl_src_font_lv_font_montserrat_24.obj third_party_lvgl_src_font_lv_font_montserrat_26.obj third_party_lvgl_src_font_lv_font_montserrat_28.obj third_party_lvgl_src_font_lv_font_montserrat_28_compressed.obj third_party_lvgl_src_font_lv_font_montserrat_30.obj third_party_lvgl_src_font_lv_font_montserrat_32.obj third_party_lvgl_src_font_lv_font_montserrat_34.obj third_party_lvgl_src_font_lv_font_montserrat_36.obj third_party_lvgl_src_font_lv_font_montserrat_38.obj third_party_lvgl_src_font_lv_font_montserrat_40.obj third_party_lvgl_src_font_lv_font_montserrat_42.obj third_party_lvgl_src_font_lv_font_montserrat_44.obj third_party_lvgl_src_font_lv_font_montserrat_46.obj third_party_lvgl_src_font_lv_font_montserrat_48.obj third_party_lvgl_src_font_lv_font_montserrat_8.obj third_party_lvgl_src_font_lv_font_simsun_16_cjk.obj third_party_lvgl_src_font_lv_font_unscii_16.obj third_party_lvgl_src_font_lv_font_unscii_8.obj third_party_lvgl_src_hal_lv_hal_disp.obj third_party_lvgl_src_hal_lv_hal_indev.obj third_party_lvgl_src_hal_lv_hal_tick.obj third_party_lvgl_src_misc_lv_anim.obj third_party_lvgl_src_misc_lv_anim_timeline.obj third_party_lvgl_src_misc_lv_area.obj third_party_lvgl_src_misc_lv_async.obj third_party_lvgl_src_misc_lv_bidi.obj third_party_lvgl_src_misc_lv_color.obj third_party_lvgl_src_misc_lv_fs.obj third_party_lvgl_src_misc_lv_gc.obj third_party_lvgl_src_misc_lv_ll.obj third_party_lvgl_src_misc_lv_log.obj third_party_lvgl_src_misc_lv_lru.obj third_party_lvgl_src_misc_lv_math.obj third_party_lvgl_src_misc_lv_mem.obj third_party_lvgl_src_misc_lv_printf.obj third_party_lvgl_src_misc_lv_style.obj third_party_lvgl_src_misc_lv_style_gen.obj third_party_lvgl_src_misc_lv_templ.obj third_party_lvgl_src_misc_lv_timer.obj third_party_lvgl_src_misc_lv_tlsf.obj third_party_lvgl_src_misc_lv_txt.obj third_party_lvgl_src_misc_lv_txt_ap.obj third_party_lvgl_src_misc_lv_utils.obj third_party_lvgl_src_widgets_lv_arc.obj third_party_lvgl_src_widgets_lv_bar.obj third_party_lvgl_src_widgets_lv_btn.obj third_party_lvgl_src_widgets_lv_btnmatrix.obj third_party_lvgl_src_widgets_lv_canvas.obj third_party_lvgl_src_widgets_lv_checkbox.obj third_party_lvgl_src_widgets_lv_dropdown.obj third_party_lvgl_src_widgets_lv_img.obj third_party_lvgl_src_widgets_lv_label.obj third_party_lvgl_src_widgets_lv_line.obj third_party_lvgl_src_widgets_lv_objx_templ.obj third_party_lvgl_src_widgets_lv_roller.obj third_party_lvgl_src_widgets_lv_slider.obj third_party_lvgl_src_widgets_lv_switch.obj third_party_lvgl_src_widgets_lv_table.obj third_party_lvgl_src_widgets_lv_textarea.obj ticlang_startup_mspm33c321x_ticlang.obj

NAME = lvgl


DFLAGS += -gdwarf-3

CFLAGS += -I.. \
    -D__MSPM33C321A__ \
    -O2 \
    -mfloat-abi=hard \
    -mfpu=fpv5-sp-d16 \
    "-I$(MSPM33_SDK_INSTALL_DIR)/source/third_party/CMSIS/Core/Include" \
    "-I$(MSPM33_SDK_INSTALL_DIR)/source" \
    -mcpu=cortex-m33 \
    -Wall


ifneq ($(CONFIG),RELEASE)
# Add debug flags by default (excluded only when CONFIG=RELEASE)
CFLAGS += $(DFLAGS)
endif

LFLAGS += "-l$(MSPM33_SDK_INSTALL_DIR)/source/ti/driverlib/lib/ticlang/m33/mspm33c321x/driverlib.a" \
    "-L$(MSPM33_SDK_INSTALL_DIR)/source" \
    -L.. \
    ../ticlang/mspm33c321a.cmd \
    "-Wl,-m,$(NAME).map" \
    -Wl,--rom_model \
    -Wl,--warn_sections \
    "-L$(TICLANG_ARMCOMPILER)/lib" \
    -llibc.a

all: $(NAME).out

main.obj: ../main.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

lcd_lcd.obj: ../lcd/lcd.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_cluster_Speedo_demo.obj: ../demos/cluster/Speedo_demo.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_cluster_icon_gas-pump.obj: ../demos/cluster/icon/gas-pump.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_cluster_icon_high_beam.obj: ../demos/cluster/icon/high_beam.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_cluster_icon_racing_helmet.obj: ../demos/cluster/icon/racing_helmet.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_cluster_icon_TXN_logo.obj: ../demos/cluster/icon/TXN_logo.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_widgets_lv_demo_widgets.obj: ../demos/widgets/lv_demo_widgets.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_widgets_assets_img_lvgl_logo.obj: ../demos/widgets/assets/img_lvgl_logo.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_widgets_assets_img_demo_widgets_avatar.obj: ../demos/widgets/assets/img_demo_widgets_avatar.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_widgets_assets_img_clothes.obj: ../demos/widgets/assets/img_clothes.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_stress_lv_demo_stress.obj: ../demos/stress/lv_demo_stress.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_music_lv_demo_music_main.obj: ../demos/music/lv_demo_music_main.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_music_lv_demo_music_list.obj: ../demos/music/lv_demo_music_list.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_music_lv_demo_music.obj: ../demos/music/lv_demo_music.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_music_assets_img_lv_demo_music_wave_top_large.obj: ../demos/music/assets/img_lv_demo_music_wave_top_large.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_music_assets_img_lv_demo_music_wave_top.obj: ../demos/music/assets/img_lv_demo_music_wave_top.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_music_assets_img_lv_demo_music_wave_bottom_large.obj: ../demos/music/assets/img_lv_demo_music_wave_bottom_large.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_music_assets_img_lv_demo_music_wave_bottom.obj: ../demos/music/assets/img_lv_demo_music_wave_bottom.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_music_assets_img_lv_demo_music_slider_knob_large.obj: ../demos/music/assets/img_lv_demo_music_slider_knob_large.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_music_assets_img_lv_demo_music_slider_knob.obj: ../demos/music/assets/img_lv_demo_music_slider_knob.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_music_assets_img_lv_demo_music_logo.obj: ../demos/music/assets/img_lv_demo_music_logo.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_music_assets_img_lv_demo_music_list_border_large.obj: ../demos/music/assets/img_lv_demo_music_list_border_large.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_music_assets_img_lv_demo_music_list_border.obj: ../demos/music/assets/img_lv_demo_music_list_border.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_music_assets_img_lv_demo_music_icon_4_large.obj: ../demos/music/assets/img_lv_demo_music_icon_4_large.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_music_assets_img_lv_demo_music_icon_4.obj: ../demos/music/assets/img_lv_demo_music_icon_4.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_music_assets_img_lv_demo_music_icon_3_large.obj: ../demos/music/assets/img_lv_demo_music_icon_3_large.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_music_assets_img_lv_demo_music_icon_3.obj: ../demos/music/assets/img_lv_demo_music_icon_3.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_music_assets_img_lv_demo_music_icon_2_large.obj: ../demos/music/assets/img_lv_demo_music_icon_2_large.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_music_assets_img_lv_demo_music_icon_2.obj: ../demos/music/assets/img_lv_demo_music_icon_2.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_music_assets_img_lv_demo_music_icon_1_large.obj: ../demos/music/assets/img_lv_demo_music_icon_1_large.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_music_assets_img_lv_demo_music_icon_1.obj: ../demos/music/assets/img_lv_demo_music_icon_1.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_music_assets_img_lv_demo_music_cover_3_large.obj: ../demos/music/assets/img_lv_demo_music_cover_3_large.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_music_assets_img_lv_demo_music_cover_3.obj: ../demos/music/assets/img_lv_demo_music_cover_3.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_music_assets_img_lv_demo_music_cover_2_large.obj: ../demos/music/assets/img_lv_demo_music_cover_2_large.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_music_assets_img_lv_demo_music_cover_2.obj: ../demos/music/assets/img_lv_demo_music_cover_2.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_music_assets_img_lv_demo_music_cover_1_large.obj: ../demos/music/assets/img_lv_demo_music_cover_1_large.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_music_assets_img_lv_demo_music_cover_1.obj: ../demos/music/assets/img_lv_demo_music_cover_1.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_music_assets_img_lv_demo_music_corner_right_large.obj: ../demos/music/assets/img_lv_demo_music_corner_right_large.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_music_assets_img_lv_demo_music_corner_right.obj: ../demos/music/assets/img_lv_demo_music_corner_right.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_music_assets_img_lv_demo_music_corner_left_large.obj: ../demos/music/assets/img_lv_demo_music_corner_left_large.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_music_assets_img_lv_demo_music_corner_left.obj: ../demos/music/assets/img_lv_demo_music_corner_left.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_music_assets_img_lv_demo_music_btn_rnd_large.obj: ../demos/music/assets/img_lv_demo_music_btn_rnd_large.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_music_assets_img_lv_demo_music_btn_rnd.obj: ../demos/music/assets/img_lv_demo_music_btn_rnd.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_music_assets_img_lv_demo_music_btn_prev_large.obj: ../demos/music/assets/img_lv_demo_music_btn_prev_large.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_music_assets_img_lv_demo_music_btn_prev.obj: ../demos/music/assets/img_lv_demo_music_btn_prev.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_music_assets_img_lv_demo_music_btn_play_large.obj: ../demos/music/assets/img_lv_demo_music_btn_play_large.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_music_assets_img_lv_demo_music_btn_play.obj: ../demos/music/assets/img_lv_demo_music_btn_play.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_music_assets_img_lv_demo_music_btn_pause_large.obj: ../demos/music/assets/img_lv_demo_music_btn_pause_large.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_music_assets_img_lv_demo_music_btn_pause.obj: ../demos/music/assets/img_lv_demo_music_btn_pause.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_music_assets_img_lv_demo_music_btn_next_large.obj: ../demos/music/assets/img_lv_demo_music_btn_next_large.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_music_assets_img_lv_demo_music_btn_next.obj: ../demos/music/assets/img_lv_demo_music_btn_next.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_music_assets_img_lv_demo_music_btn_loop_large.obj: ../demos/music/assets/img_lv_demo_music_btn_loop_large.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_music_assets_img_lv_demo_music_btn_loop.obj: ../demos/music/assets/img_lv_demo_music_btn_loop.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_music_assets_img_lv_demo_music_btn_list_play_large.obj: ../demos/music/assets/img_lv_demo_music_btn_list_play_large.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_music_assets_img_lv_demo_music_btn_list_play.obj: ../demos/music/assets/img_lv_demo_music_btn_list_play.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_music_assets_img_lv_demo_music_btn_list_pause_large.obj: ../demos/music/assets/img_lv_demo_music_btn_list_pause_large.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_music_assets_img_lv_demo_music_btn_list_pause.obj: ../demos/music/assets/img_lv_demo_music_btn_list_pause.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_music_assets_img_lv_demo_music_btn_corner_large.obj: ../demos/music/assets/img_lv_demo_music_btn_corner_large.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_keypad_encoder_lv_demo_keypad_encoder.obj: ../demos/keypad_encoder/lv_demo_keypad_encoder.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_benchmark_lv_demo_benchmark.obj: ../demos/benchmark/lv_demo_benchmark.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_benchmark_assets_lv_font_bechmark_montserrat_28_compr_az.c.obj: ../demos/benchmark/assets/lv_font_bechmark_montserrat_28_compr_az.c.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_benchmark_assets_lv_font_bechmark_montserrat_16_compr_az.c.obj: ../demos/benchmark/assets/lv_font_bechmark_montserrat_16_compr_az.c.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_benchmark_assets_lv_font_bechmark_montserrat_12_compr_az.c.obj: ../demos/benchmark/assets/lv_font_bechmark_montserrat_12_compr_az.c.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_benchmark_assets_img_benchmark_cogwheel_rgb565a8.obj: ../demos/benchmark/assets/img_benchmark_cogwheel_rgb565a8.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_benchmark_assets_img_benchmark_cogwheel_rgb.obj: ../demos/benchmark/assets/img_benchmark_cogwheel_rgb.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_benchmark_assets_img_benchmark_cogwheel_indexed16.obj: ../demos/benchmark/assets/img_benchmark_cogwheel_indexed16.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_benchmark_assets_img_benchmark_cogwheel_chroma_keyed.obj: ../demos/benchmark/assets/img_benchmark_cogwheel_chroma_keyed.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_benchmark_assets_img_benchmark_cogwheel_argb.obj: ../demos/benchmark/assets/img_benchmark_cogwheel_argb.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

demos_benchmark_assets_img_benchmark_cogwheel_alpha16.obj: ../demos/benchmark/assets/img_benchmark_cogwheel_alpha16.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

spi_spi.obj: ../spi/spi.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

spi_delay.obj: ../spi/delay.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

touch_touch.obj: ../touch/touch.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

ti_msp_dl_config.obj: ../ti_msp_dl_config.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

porting_lv_port_fs.obj: ../porting/lv_port_fs.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

porting_lv_port_indev.obj: ../porting/lv_port_indev.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

porting_lv_port_disp.obj: ../porting/lv_port_disp.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_core_lv_disp.obj: ../third_party/lvgl/src/core/lv_disp.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_core_lv_event.obj: ../third_party/lvgl/src/core/lv_event.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_core_lv_group.obj: ../third_party/lvgl/src/core/lv_group.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_core_lv_indev.obj: ../third_party/lvgl/src/core/lv_indev.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_core_lv_indev_scroll.obj: ../third_party/lvgl/src/core/lv_indev_scroll.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_core_lv_obj.obj: ../third_party/lvgl/src/core/lv_obj.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_core_lv_obj_class.obj: ../third_party/lvgl/src/core/lv_obj_class.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_core_lv_obj_draw.obj: ../third_party/lvgl/src/core/lv_obj_draw.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_core_lv_obj_pos.obj: ../third_party/lvgl/src/core/lv_obj_pos.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_core_lv_obj_scroll.obj: ../third_party/lvgl/src/core/lv_obj_scroll.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_core_lv_obj_style.obj: ../third_party/lvgl/src/core/lv_obj_style.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_core_lv_obj_style_gen.obj: ../third_party/lvgl/src/core/lv_obj_style_gen.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_core_lv_obj_tree.obj: ../third_party/lvgl/src/core/lv_obj_tree.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_core_lv_refr.obj: ../third_party/lvgl/src/core/lv_refr.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_core_lv_theme.obj: ../third_party/lvgl/src/core/lv_theme.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_draw_arm2d_lv_gpu_arm2d.obj: ../third_party/lvgl/src/draw/arm2d/lv_gpu_arm2d.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_draw_lv_draw.obj: ../third_party/lvgl/src/draw/lv_draw.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_draw_lv_draw_arc.obj: ../third_party/lvgl/src/draw/lv_draw_arc.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_draw_lv_draw_img.obj: ../third_party/lvgl/src/draw/lv_draw_img.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_draw_lv_draw_label.obj: ../third_party/lvgl/src/draw/lv_draw_label.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_draw_lv_draw_layer.obj: ../third_party/lvgl/src/draw/lv_draw_layer.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_draw_lv_draw_line.obj: ../third_party/lvgl/src/draw/lv_draw_line.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_draw_lv_draw_mask.obj: ../third_party/lvgl/src/draw/lv_draw_mask.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_draw_lv_draw_rect.obj: ../third_party/lvgl/src/draw/lv_draw_rect.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_draw_lv_draw_transform.obj: ../third_party/lvgl/src/draw/lv_draw_transform.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_draw_lv_draw_triangle.obj: ../third_party/lvgl/src/draw/lv_draw_triangle.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_draw_lv_img_buf.obj: ../third_party/lvgl/src/draw/lv_img_buf.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_draw_lv_img_cache.obj: ../third_party/lvgl/src/draw/lv_img_cache.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_draw_lv_img_decoder.obj: ../third_party/lvgl/src/draw/lv_img_decoder.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_draw_nxp_pxp_lv_draw_pxp.obj: ../third_party/lvgl/src/draw/nxp/pxp/lv_draw_pxp.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_draw_nxp_pxp_lv_draw_pxp_blend.obj: ../third_party/lvgl/src/draw/nxp/pxp/lv_draw_pxp_blend.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_draw_nxp_pxp_lv_gpu_nxp_pxp.obj: ../third_party/lvgl/src/draw/nxp/pxp/lv_gpu_nxp_pxp.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_draw_nxp_pxp_lv_gpu_nxp_pxp_osa.obj: ../third_party/lvgl/src/draw/nxp/pxp/lv_gpu_nxp_pxp_osa.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_draw_nxp_vglite_lv_draw_vglite.obj: ../third_party/lvgl/src/draw/nxp/vglite/lv_draw_vglite.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_draw_nxp_vglite_lv_draw_vglite_arc.obj: ../third_party/lvgl/src/draw/nxp/vglite/lv_draw_vglite_arc.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_draw_nxp_vglite_lv_draw_vglite_blend.obj: ../third_party/lvgl/src/draw/nxp/vglite/lv_draw_vglite_blend.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_draw_nxp_vglite_lv_draw_vglite_line.obj: ../third_party/lvgl/src/draw/nxp/vglite/lv_draw_vglite_line.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_draw_nxp_vglite_lv_draw_vglite_rect.obj: ../third_party/lvgl/src/draw/nxp/vglite/lv_draw_vglite_rect.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_draw_nxp_vglite_lv_vglite_buf.obj: ../third_party/lvgl/src/draw/nxp/vglite/lv_vglite_buf.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_draw_nxp_vglite_lv_vglite_utils.obj: ../third_party/lvgl/src/draw/nxp/vglite/lv_vglite_utils.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_draw_renesas_lv_gpu_d2_draw_label.obj: ../third_party/lvgl/src/draw/renesas/lv_gpu_d2_draw_label.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_draw_renesas_lv_gpu_d2_ra6m3.obj: ../third_party/lvgl/src/draw/renesas/lv_gpu_d2_ra6m3.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_draw_sdl_lv_draw_sdl.obj: ../third_party/lvgl/src/draw/sdl/lv_draw_sdl.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_draw_sdl_lv_draw_sdl_arc.obj: ../third_party/lvgl/src/draw/sdl/lv_draw_sdl_arc.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_draw_sdl_lv_draw_sdl_bg.obj: ../third_party/lvgl/src/draw/sdl/lv_draw_sdl_bg.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_draw_sdl_lv_draw_sdl_composite.obj: ../third_party/lvgl/src/draw/sdl/lv_draw_sdl_composite.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_draw_sdl_lv_draw_sdl_img.obj: ../third_party/lvgl/src/draw/sdl/lv_draw_sdl_img.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_draw_sdl_lv_draw_sdl_label.obj: ../third_party/lvgl/src/draw/sdl/lv_draw_sdl_label.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_draw_sdl_lv_draw_sdl_layer.obj: ../third_party/lvgl/src/draw/sdl/lv_draw_sdl_layer.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_draw_sdl_lv_draw_sdl_line.obj: ../third_party/lvgl/src/draw/sdl/lv_draw_sdl_line.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_draw_sdl_lv_draw_sdl_mask.obj: ../third_party/lvgl/src/draw/sdl/lv_draw_sdl_mask.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_draw_sdl_lv_draw_sdl_polygon.obj: ../third_party/lvgl/src/draw/sdl/lv_draw_sdl_polygon.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_draw_sdl_lv_draw_sdl_rect.obj: ../third_party/lvgl/src/draw/sdl/lv_draw_sdl_rect.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_draw_sdl_lv_draw_sdl_stack_blur.obj: ../third_party/lvgl/src/draw/sdl/lv_draw_sdl_stack_blur.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_draw_sdl_lv_draw_sdl_texture_cache.obj: ../third_party/lvgl/src/draw/sdl/lv_draw_sdl_texture_cache.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_draw_sdl_lv_draw_sdl_utils.obj: ../third_party/lvgl/src/draw/sdl/lv_draw_sdl_utils.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_draw_stm32_dma2d_lv_gpu_stm32_dma2d.obj: ../third_party/lvgl/src/draw/stm32_dma2d/lv_gpu_stm32_dma2d.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_draw_sw_lv_draw_sw.obj: ../third_party/lvgl/src/draw/sw/lv_draw_sw.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_draw_sw_lv_draw_sw_arc.obj: ../third_party/lvgl/src/draw/sw/lv_draw_sw_arc.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_draw_sw_lv_draw_sw_blend.obj: ../third_party/lvgl/src/draw/sw/lv_draw_sw_blend.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_draw_sw_lv_draw_sw_dither.obj: ../third_party/lvgl/src/draw/sw/lv_draw_sw_dither.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_draw_sw_lv_draw_sw_gradient.obj: ../third_party/lvgl/src/draw/sw/lv_draw_sw_gradient.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_draw_sw_lv_draw_sw_img.obj: ../third_party/lvgl/src/draw/sw/lv_draw_sw_img.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_draw_sw_lv_draw_sw_layer.obj: ../third_party/lvgl/src/draw/sw/lv_draw_sw_layer.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_draw_sw_lv_draw_sw_letter.obj: ../third_party/lvgl/src/draw/sw/lv_draw_sw_letter.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_draw_sw_lv_draw_sw_line.obj: ../third_party/lvgl/src/draw/sw/lv_draw_sw_line.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_draw_sw_lv_draw_sw_polygon.obj: ../third_party/lvgl/src/draw/sw/lv_draw_sw_polygon.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_draw_sw_lv_draw_sw_rect.obj: ../third_party/lvgl/src/draw/sw/lv_draw_sw_rect.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_draw_sw_lv_draw_sw_transform.obj: ../third_party/lvgl/src/draw/sw/lv_draw_sw_transform.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_draw_swm341_dma2d_lv_gpu_swm341_dma2d.obj: ../third_party/lvgl/src/draw/swm341_dma2d/lv_gpu_swm341_dma2d.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_extra_layouts_flex_lv_flex.obj: ../third_party/lvgl/src/extra/layouts/flex/lv_flex.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_extra_layouts_grid_lv_grid.obj: ../third_party/lvgl/src/extra/layouts/grid/lv_grid.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_extra_libs_bmp_lv_bmp.obj: ../third_party/lvgl/src/extra/libs/bmp/lv_bmp.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_extra_libs_ffmpeg_lv_ffmpeg.obj: ../third_party/lvgl/src/extra/libs/ffmpeg/lv_ffmpeg.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_extra_libs_freetype_lv_freetype.obj: ../third_party/lvgl/src/extra/libs/freetype/lv_freetype.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_extra_libs_fsdrv_lv_fs_fatfs.obj: ../third_party/lvgl/src/extra/libs/fsdrv/lv_fs_fatfs.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_extra_libs_fsdrv_lv_fs_littlefs.obj: ../third_party/lvgl/src/extra/libs/fsdrv/lv_fs_littlefs.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_extra_libs_fsdrv_lv_fs_posix.obj: ../third_party/lvgl/src/extra/libs/fsdrv/lv_fs_posix.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_extra_libs_fsdrv_lv_fs_stdio.obj: ../third_party/lvgl/src/extra/libs/fsdrv/lv_fs_stdio.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_extra_libs_fsdrv_lv_fs_win32.obj: ../third_party/lvgl/src/extra/libs/fsdrv/lv_fs_win32.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_extra_libs_gif_gifdec.obj: ../third_party/lvgl/src/extra/libs/gif/gifdec.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_extra_libs_gif_lv_gif.obj: ../third_party/lvgl/src/extra/libs/gif/lv_gif.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_extra_libs_png_lodepng.obj: ../third_party/lvgl/src/extra/libs/png/lodepng.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_extra_libs_png_lv_png.obj: ../third_party/lvgl/src/extra/libs/png/lv_png.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_extra_libs_qrcode_lv_qrcode.obj: ../third_party/lvgl/src/extra/libs/qrcode/lv_qrcode.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_extra_libs_qrcode_qrcodegen.obj: ../third_party/lvgl/src/extra/libs/qrcode/qrcodegen.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_extra_libs_rlottie_lv_rlottie.obj: ../third_party/lvgl/src/extra/libs/rlottie/lv_rlottie.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_extra_libs_sjpg_lv_sjpg.obj: ../third_party/lvgl/src/extra/libs/sjpg/lv_sjpg.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_extra_libs_sjpg_tjpgd.obj: ../third_party/lvgl/src/extra/libs/sjpg/tjpgd.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_extra_libs_tiny_ttf_lv_tiny_ttf.obj: ../third_party/lvgl/src/extra/libs/tiny_ttf/lv_tiny_ttf.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_extra_lv_extra.obj: ../third_party/lvgl/src/extra/lv_extra.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_extra_others_fragment_lv_fragment.obj: ../third_party/lvgl/src/extra/others/fragment/lv_fragment.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_extra_others_fragment_lv_fragment_manager.obj: ../third_party/lvgl/src/extra/others/fragment/lv_fragment_manager.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_extra_others_gridnav_lv_gridnav.obj: ../third_party/lvgl/src/extra/others/gridnav/lv_gridnav.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_extra_others_ime_lv_ime_pinyin.obj: ../third_party/lvgl/src/extra/others/ime/lv_ime_pinyin.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_extra_others_imgfont_lv_imgfont.obj: ../third_party/lvgl/src/extra/others/imgfont/lv_imgfont.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_extra_others_monkey_lv_monkey.obj: ../third_party/lvgl/src/extra/others/monkey/lv_monkey.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_extra_others_msg_lv_msg.obj: ../third_party/lvgl/src/extra/others/msg/lv_msg.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_extra_others_snapshot_lv_snapshot.obj: ../third_party/lvgl/src/extra/others/snapshot/lv_snapshot.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_extra_themes_basic_lv_theme_basic.obj: ../third_party/lvgl/src/extra/themes/basic/lv_theme_basic.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_extra_themes_default_lv_theme_default.obj: ../third_party/lvgl/src/extra/themes/default/lv_theme_default.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_extra_themes_mono_lv_theme_mono.obj: ../third_party/lvgl/src/extra/themes/mono/lv_theme_mono.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_extra_widgets_animimg_lv_animimg.obj: ../third_party/lvgl/src/extra/widgets/animimg/lv_animimg.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_extra_widgets_calendar_lv_calendar.obj: ../third_party/lvgl/src/extra/widgets/calendar/lv_calendar.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_extra_widgets_calendar_lv_calendar_header_arrow.obj: ../third_party/lvgl/src/extra/widgets/calendar/lv_calendar_header_arrow.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_extra_widgets_calendar_lv_calendar_header_dropdown.obj: ../third_party/lvgl/src/extra/widgets/calendar/lv_calendar_header_dropdown.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_extra_widgets_chart_lv_chart.obj: ../third_party/lvgl/src/extra/widgets/chart/lv_chart.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_extra_widgets_colorwheel_lv_colorwheel.obj: ../third_party/lvgl/src/extra/widgets/colorwheel/lv_colorwheel.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_extra_widgets_imgbtn_lv_imgbtn.obj: ../third_party/lvgl/src/extra/widgets/imgbtn/lv_imgbtn.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_extra_widgets_keyboard_lv_keyboard.obj: ../third_party/lvgl/src/extra/widgets/keyboard/lv_keyboard.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_extra_widgets_led_lv_led.obj: ../third_party/lvgl/src/extra/widgets/led/lv_led.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_extra_widgets_list_lv_list.obj: ../third_party/lvgl/src/extra/widgets/list/lv_list.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_extra_widgets_menu_lv_menu.obj: ../third_party/lvgl/src/extra/widgets/menu/lv_menu.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_extra_widgets_meter_lv_meter.obj: ../third_party/lvgl/src/extra/widgets/meter/lv_meter.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_extra_widgets_msgbox_lv_msgbox.obj: ../third_party/lvgl/src/extra/widgets/msgbox/lv_msgbox.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_extra_widgets_span_lv_span.obj: ../third_party/lvgl/src/extra/widgets/span/lv_span.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_extra_widgets_spinbox_lv_spinbox.obj: ../third_party/lvgl/src/extra/widgets/spinbox/lv_spinbox.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_extra_widgets_spinner_lv_spinner.obj: ../third_party/lvgl/src/extra/widgets/spinner/lv_spinner.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_extra_widgets_tabview_lv_tabview.obj: ../third_party/lvgl/src/extra/widgets/tabview/lv_tabview.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_extra_widgets_tileview_lv_tileview.obj: ../third_party/lvgl/src/extra/widgets/tileview/lv_tileview.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_extra_widgets_win_lv_win.obj: ../third_party/lvgl/src/extra/widgets/win/lv_win.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_font_lv_font.obj: ../third_party/lvgl/src/font/lv_font.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_font_lv_font_dejavu_16_persian_hebrew.obj: ../third_party/lvgl/src/font/lv_font_dejavu_16_persian_hebrew.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_font_lv_font_fmt_txt.obj: ../third_party/lvgl/src/font/lv_font_fmt_txt.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_font_lv_font_loader.obj: ../third_party/lvgl/src/font/lv_font_loader.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_font_lv_font_montserrat_10.obj: ../third_party/lvgl/src/font/lv_font_montserrat_10.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_font_lv_font_montserrat_12.obj: ../third_party/lvgl/src/font/lv_font_montserrat_12.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_font_lv_font_montserrat_12_subpx.obj: ../third_party/lvgl/src/font/lv_font_montserrat_12_subpx.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_font_lv_font_montserrat_14.obj: ../third_party/lvgl/src/font/lv_font_montserrat_14.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_font_lv_font_montserrat_16.obj: ../third_party/lvgl/src/font/lv_font_montserrat_16.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_font_lv_font_montserrat_18.obj: ../third_party/lvgl/src/font/lv_font_montserrat_18.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_font_lv_font_montserrat_20.obj: ../third_party/lvgl/src/font/lv_font_montserrat_20.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_font_lv_font_montserrat_22.obj: ../third_party/lvgl/src/font/lv_font_montserrat_22.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_font_lv_font_montserrat_24.obj: ../third_party/lvgl/src/font/lv_font_montserrat_24.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_font_lv_font_montserrat_26.obj: ../third_party/lvgl/src/font/lv_font_montserrat_26.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_font_lv_font_montserrat_28.obj: ../third_party/lvgl/src/font/lv_font_montserrat_28.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_font_lv_font_montserrat_28_compressed.obj: ../third_party/lvgl/src/font/lv_font_montserrat_28_compressed.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_font_lv_font_montserrat_30.obj: ../third_party/lvgl/src/font/lv_font_montserrat_30.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_font_lv_font_montserrat_32.obj: ../third_party/lvgl/src/font/lv_font_montserrat_32.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_font_lv_font_montserrat_34.obj: ../third_party/lvgl/src/font/lv_font_montserrat_34.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_font_lv_font_montserrat_36.obj: ../third_party/lvgl/src/font/lv_font_montserrat_36.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_font_lv_font_montserrat_38.obj: ../third_party/lvgl/src/font/lv_font_montserrat_38.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_font_lv_font_montserrat_40.obj: ../third_party/lvgl/src/font/lv_font_montserrat_40.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_font_lv_font_montserrat_42.obj: ../third_party/lvgl/src/font/lv_font_montserrat_42.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_font_lv_font_montserrat_44.obj: ../third_party/lvgl/src/font/lv_font_montserrat_44.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_font_lv_font_montserrat_46.obj: ../third_party/lvgl/src/font/lv_font_montserrat_46.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_font_lv_font_montserrat_48.obj: ../third_party/lvgl/src/font/lv_font_montserrat_48.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_font_lv_font_montserrat_8.obj: ../third_party/lvgl/src/font/lv_font_montserrat_8.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_font_lv_font_simsun_16_cjk.obj: ../third_party/lvgl/src/font/lv_font_simsun_16_cjk.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_font_lv_font_unscii_16.obj: ../third_party/lvgl/src/font/lv_font_unscii_16.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_font_lv_font_unscii_8.obj: ../third_party/lvgl/src/font/lv_font_unscii_8.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_hal_lv_hal_disp.obj: ../third_party/lvgl/src/hal/lv_hal_disp.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_hal_lv_hal_indev.obj: ../third_party/lvgl/src/hal/lv_hal_indev.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_hal_lv_hal_tick.obj: ../third_party/lvgl/src/hal/lv_hal_tick.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_misc_lv_anim.obj: ../third_party/lvgl/src/misc/lv_anim.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_misc_lv_anim_timeline.obj: ../third_party/lvgl/src/misc/lv_anim_timeline.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_misc_lv_area.obj: ../third_party/lvgl/src/misc/lv_area.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_misc_lv_async.obj: ../third_party/lvgl/src/misc/lv_async.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_misc_lv_bidi.obj: ../third_party/lvgl/src/misc/lv_bidi.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_misc_lv_color.obj: ../third_party/lvgl/src/misc/lv_color.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_misc_lv_fs.obj: ../third_party/lvgl/src/misc/lv_fs.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_misc_lv_gc.obj: ../third_party/lvgl/src/misc/lv_gc.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_misc_lv_ll.obj: ../third_party/lvgl/src/misc/lv_ll.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_misc_lv_log.obj: ../third_party/lvgl/src/misc/lv_log.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_misc_lv_lru.obj: ../third_party/lvgl/src/misc/lv_lru.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_misc_lv_math.obj: ../third_party/lvgl/src/misc/lv_math.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_misc_lv_mem.obj: ../third_party/lvgl/src/misc/lv_mem.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_misc_lv_printf.obj: ../third_party/lvgl/src/misc/lv_printf.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_misc_lv_style.obj: ../third_party/lvgl/src/misc/lv_style.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_misc_lv_style_gen.obj: ../third_party/lvgl/src/misc/lv_style_gen.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_misc_lv_templ.obj: ../third_party/lvgl/src/misc/lv_templ.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_misc_lv_timer.obj: ../third_party/lvgl/src/misc/lv_timer.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_misc_lv_tlsf.obj: ../third_party/lvgl/src/misc/lv_tlsf.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_misc_lv_txt.obj: ../third_party/lvgl/src/misc/lv_txt.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_misc_lv_txt_ap.obj: ../third_party/lvgl/src/misc/lv_txt_ap.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_misc_lv_utils.obj: ../third_party/lvgl/src/misc/lv_utils.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_widgets_lv_arc.obj: ../third_party/lvgl/src/widgets/lv_arc.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_widgets_lv_bar.obj: ../third_party/lvgl/src/widgets/lv_bar.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_widgets_lv_btn.obj: ../third_party/lvgl/src/widgets/lv_btn.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_widgets_lv_btnmatrix.obj: ../third_party/lvgl/src/widgets/lv_btnmatrix.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_widgets_lv_canvas.obj: ../third_party/lvgl/src/widgets/lv_canvas.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_widgets_lv_checkbox.obj: ../third_party/lvgl/src/widgets/lv_checkbox.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_widgets_lv_dropdown.obj: ../third_party/lvgl/src/widgets/lv_dropdown.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_widgets_lv_img.obj: ../third_party/lvgl/src/widgets/lv_img.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_widgets_lv_label.obj: ../third_party/lvgl/src/widgets/lv_label.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_widgets_lv_line.obj: ../third_party/lvgl/src/widgets/lv_line.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_widgets_lv_objx_templ.obj: ../third_party/lvgl/src/widgets/lv_objx_templ.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_widgets_lv_roller.obj: ../third_party/lvgl/src/widgets/lv_roller.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_widgets_lv_slider.obj: ../third_party/lvgl/src/widgets/lv_slider.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_widgets_lv_switch.obj: ../third_party/lvgl/src/widgets/lv_switch.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_widgets_lv_table.obj: ../third_party/lvgl/src/widgets/lv_table.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

third_party_lvgl_src_widgets_lv_textarea.obj: ../third_party/lvgl/src/widgets/lv_textarea.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

ticlang_startup_mspm33c321x_ticlang.obj: ../ticlang/startup_mspm33c321x_ticlang.c
	@ echo Building $@
	$(V) $(CC) $(CFLAGS) -c $< -o $@

$(NAME).out: $(OBJECTS)
	@ echo linking $@
	$(V) $(LNK) -Wl,-u,_c_int00 $(OBJECTS)  $(LFLAGS) -o $(NAME).out

clean:
	@ echo Cleaning...
	$(V) $(RM) $(OBJECTS) > $(DEVNULL) 2>&1
	$(V) $(RM) $(NAME).out > $(DEVNULL) 2>&1
	$(V) $(RM) $(NAME).map > $(DEVNULL) 2>&1
