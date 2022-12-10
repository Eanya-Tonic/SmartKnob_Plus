#pragma once

#include <lvgl.h>
#include <TFT_eSPI.h>

#include "guider_fonts/guider_fonts.h"

typedef struct
{
	lv_obj_t *screen;
	lv_obj_t *screen_bar_1;
	lv_obj_t *screen_label_1;
	lv_obj_t *volume;
	lv_obj_t *volume_lmeter_1;
	lv_obj_t *volume_label_1;
	lv_obj_t *volume_label_2;
	lv_obj_t *menu;
	lv_obj_t *menu_roller_1;
	lv_obj_t *menu_label_1;
	lv_obj_t *slide;
	lv_obj_t *slide_arc_1;
	lv_obj_t *slide_label_1;
	lv_obj_t *media;
	lv_obj_t *media_img_2;
	lv_obj_t *media_img_3;
	lv_obj_t *media_label_1;
	lv_obj_t *media_img_4;
	lv_obj_t *setup1;
	lv_obj_t *setup1_sw_1;
	lv_obj_t *setup1_label_1;
	lv_obj_t *setup1_label_2;
	lv_obj_t *setup1_label_3;
	lv_obj_t *setup2;
	lv_obj_t *setup2_roller_1;
	lv_obj_t *setup2_label_1;
	lv_obj_t *setup2_img_1;
	lv_obj_t *setup2_label_2;
	lv_obj_t *logo;
	lv_obj_t *logo_img_1;
} lv_ui;

void getValue(int32_t value);
void getconf_gui(int32_t value);
int32_t return_val(void);
void muteAction(void);
void nextSetup(void);
void startcompatibleMode(void);

void task_cb(lv_task_t task);
void task_menu(lv_task_t *task);
void task_slide(lv_task_t *task);

void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);

void setup_scr_screen(lv_ui *ui);
void setup_scr_volume(lv_ui *ui);
void setup_scr_menu(lv_ui *ui);
void setup_scr_slide(lv_ui *ui);
void setup_scr_setup1(lv_ui *ui);
void setup_scr_setup2(lv_ui *ui);
void setup_scr_logo(lv_ui *ui);

void setup_ui(lv_ui *ui);
void ui_loop(lv_ui *ui);

void gui_init(void);
void gui_loop(void);

LV_IMG_DECLARE(_skip_next_FILL0_wght400_GRAD0_opsz48_alpha_50x50);
LV_IMG_DECLARE(_play_circle_FILL0_wght400_GRAD0_opsz48_alpha_50x50);
LV_IMG_DECLARE(_skip_previous_FILL0_wght400_GRAD0_opsz48_alpha_50x50);
LV_IMG_DECLARE(_arrow_circle_right_FILL1_wght400_GRAD200_opsz48_alpha_35x35);
LV_IMG_DECLARE(_logo_alpha_200x200);