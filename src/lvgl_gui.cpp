#include "lvgl_gui.h"

// tft驱动
TFT_eSPI tft = TFT_eSPI();
// lvgl驱动
static lv_disp_buf_t disp_buf;
static lv_color_t buf[LV_HOR_RES_MAX * 10];
lv_ui guider_ui;

// 当前值
int32_t val = 0;
// 上一次值
int32_t prevous_val = 0;
// 配置信息
int32_t conf = 0;
// 上一次配置信息
int32_t prevous_conf = 0;

// 静音标志
bool muteFlag = true;

// 获取当前位置值
void getValue(int32_t value)
{
    val = value * 2;
}

// 获取配置信息
void getconf_gui(int32_t value)
{
    conf = value;
    /*
    config 0: 音量控制
    config 1: 移动控制
    config 2: 音乐控制
    config 3: 兼容模式确认页
    config 4: 兼容模式设置页
    config 5: 功能菜单
    config 6: 兼容模式无阻尼
    config 7: 兼容模式弱阻尼
    config 8: 兼容模式强阻尼
    */
}

// 返回当前值
int32_t return_val(void)
{
    return round(val / 2);
}

// 静音时改变音量条颜色
void muteAction(void)
{
    static lv_style_t style_volume_lmeter_1_main;
    if (muteFlag)
    {
        lv_style_set_line_color(&style_volume_lmeter_1_main, LV_STATE_DEFAULT, LV_COLOR_GREEN);     // 活跃刻度线起始颜色
        lv_style_set_scale_grad_color(&style_volume_lmeter_1_main, LV_STATE_DEFAULT, LV_COLOR_RED); // 活跃刻度线终止颜色
        muteFlag = !muteFlag;
    }
    else
    {
        lv_style_set_line_color(&style_volume_lmeter_1_main, LV_STATE_DEFAULT, LV_COLOR_MAKE(0xD2, 0xFF, 0xCE));       // 活跃刻度线起始颜色
        lv_style_set_scale_grad_color(&style_volume_lmeter_1_main, LV_STATE_DEFAULT, LV_COLOR_MAKE(0xFF, 0xCE, 0xCE)); // 活跃刻度线终止颜色
        muteFlag = !muteFlag;
    }
    lv_obj_add_style(guider_ui.volume_lmeter_1, LV_LINEMETER_PART_MAIN, &style_volume_lmeter_1_main);
}

// 进入兼容模式配置页
void nextSetup(void)
{
    lv_scr_load_anim(guider_ui.setup2, LV_SCR_LOAD_ANIM_OVER_LEFT, 500, 0, false);
}

// 切换至logo页
void startcompatibleMode(void)
{
    lv_scr_load_anim(guider_ui.logo, LV_SCR_LOAD_ANIM_OVER_LEFT, 500, 0, true);
}

// 设置音量
void task_cb(lv_task_t *task)
{
    if (conf != 0)
    {
        return;
    }
    char buf[5];
    lv_linemeter_set_value(guider_ui.volume_lmeter_1, val);
    sprintf(buf, "%d%%", val);
    lv_label_set_text(guider_ui.volume_label_2, buf);
    Serial.printf("VOLUME: %d\n", val);
    if (prevous_val != val && muteFlag == true)
    {
        muteAction();
    }
    prevous_val = val;
}

// 选择菜单项
void task_menu(lv_task_t *task)
{
    if (conf != 5)
    {
        return;
    }
    lv_roller_set_selected(guider_ui.menu_roller_1, round(val / 2), LV_ANIM_ON);
}

// 移动模式旋转
void task_slide(lv_task_t *task)
{
    if (conf != 1)
    {
        return;
    }
    uint16_t rotation = ((val * 5) % 360 + 360) % 360;
    lv_arc_set_angles(guider_ui.slide_arc_1, rotation, rotation + 60);
}

// 兼容模式确认页按钮控制
void task_switch(lv_task_t *task)
{
    if (conf != 3)
    {
        return;
    }
    if (val == 0)
    {
        lv_switch_off(guider_ui.setup1_sw_1, LV_ANIM_ON);
    }
    else
    {
        lv_switch_on(guider_ui.setup1_sw_1, LV_ANIM_ON);
    }
}

// 兼容模式设置页控制
void task_setup(lv_task_t *task)
{
    if (conf != 4)
    {
        return;
    }
    lv_roller_set_selected(guider_ui.setup2_roller_1, round(val / 2), LV_ANIM_ON);
}

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushColors(&color_p->full, w * h, true);
    tft.endWrite();

    lv_disp_flush_ready(disp);
}

// 初始化启动进度条
void setup_scr_screen(lv_ui *ui)
{
    // Write codes screen_bar_1
    ui->screen_bar_1 = lv_bar_create(ui->screen, NULL);

    // Write style LV_BAR_PART_BG for screen_bar_1
    static lv_style_t style_screen_bar_1_bg;
    lv_style_reset(&style_screen_bar_1_bg);

    // Write style state: LV_STATE_DEFAULT for style_screen_bar_1_bg
    lv_style_set_radius(&style_screen_bar_1_bg, LV_STATE_DEFAULT, 10);
    lv_style_set_bg_color(&style_screen_bar_1_bg, LV_STATE_DEFAULT, lv_color_make(0xd4, 0xd7, 0xd9));
    lv_style_set_bg_grad_color(&style_screen_bar_1_bg, LV_STATE_DEFAULT, lv_color_make(0xd4, 0xd7, 0xd9));
    lv_style_set_bg_grad_dir(&style_screen_bar_1_bg, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
    lv_style_set_bg_opa(&style_screen_bar_1_bg, LV_STATE_DEFAULT, 255);
    lv_style_set_pad_left(&style_screen_bar_1_bg, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_right(&style_screen_bar_1_bg, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_top(&style_screen_bar_1_bg, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_bottom(&style_screen_bar_1_bg, LV_STATE_DEFAULT, 0);
    lv_obj_add_style(ui->screen_bar_1, LV_BAR_PART_BG, &style_screen_bar_1_bg);

    // Write style LV_BAR_PART_INDIC for screen_bar_1
    static lv_style_t style_screen_bar_1_indic;
    lv_style_reset(&style_screen_bar_1_indic);

    // Write style state: LV_STATE_DEFAULT for style_screen_bar_1_indic
    lv_style_set_radius(&style_screen_bar_1_indic, LV_STATE_DEFAULT, 10);
    lv_style_set_bg_color(&style_screen_bar_1_indic, LV_STATE_DEFAULT, lv_color_make(0x01, 0xa2, 0xb1));
    lv_style_set_bg_grad_color(&style_screen_bar_1_indic, LV_STATE_DEFAULT, lv_color_make(0x01, 0xa2, 0xb1));
    lv_style_set_bg_grad_dir(&style_screen_bar_1_indic, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
    lv_style_set_bg_opa(&style_screen_bar_1_indic, LV_STATE_DEFAULT, 255);
    lv_obj_add_style(ui->screen_bar_1, LV_BAR_PART_INDIC, &style_screen_bar_1_indic);
    lv_obj_set_pos(ui->screen_bar_1, 48, 96);
    lv_obj_set_size(ui->screen_bar_1, 144, 41);
    lv_bar_set_anim_time(ui->screen_bar_1, 1000);
    lv_bar_set_value(ui->screen_bar_1, 100, LV_ANIM_ON);
    lv_bar_set_range(ui->screen_bar_1, 0, 100);

    // Write codes screen_label_1
    ui->screen_label_1 = lv_label_create(ui->screen, NULL);
    lv_label_set_text(ui->screen_label_1, "正在启动...");
    lv_label_set_long_mode(ui->screen_label_1, LV_LABEL_LONG_BREAK);
    lv_label_set_align(ui->screen_label_1, LV_LABEL_ALIGN_CENTER);

    // Write style LV_LABEL_PART_MAIN for screen_label_1
    static lv_style_t style_screen_label_1_main;
    lv_style_reset(&style_screen_label_1_main);

    // Write style state: LV_STATE_DEFAULT for style_screen_label_1_main
    lv_style_set_radius(&style_screen_label_1_main, LV_STATE_DEFAULT, 0);
    lv_style_set_bg_color(&style_screen_label_1_main, LV_STATE_DEFAULT, lv_color_make(0x5a, 0x61, 0x73));
    lv_style_set_bg_grad_color(&style_screen_label_1_main, LV_STATE_DEFAULT, lv_color_make(0x5a, 0x61, 0x73));
    lv_style_set_bg_grad_dir(&style_screen_label_1_main, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
    lv_style_set_bg_opa(&style_screen_label_1_main, LV_STATE_DEFAULT, 0);
    lv_style_set_text_color(&style_screen_label_1_main, LV_STATE_DEFAULT, lv_color_make(0x00, 0x00, 0x00));
    lv_style_set_text_font(&style_screen_label_1_main, LV_STATE_DEFAULT, &lv_font_HarmonyOS_Sans_SC_Regular_14);
    lv_style_set_text_letter_space(&style_screen_label_1_main, LV_STATE_DEFAULT, 2);
    lv_style_set_pad_left(&style_screen_label_1_main, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_right(&style_screen_label_1_main, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_top(&style_screen_label_1_main, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_bottom(&style_screen_label_1_main, LV_STATE_DEFAULT, 0);
    lv_obj_add_style(ui->screen_label_1, LV_LABEL_PART_MAIN, &style_screen_label_1_main);
    lv_obj_set_pos(ui->screen_label_1, 70, 153);
    lv_obj_set_size(ui->screen_label_1, 100, 0);
}

// 初始化音量界面
void setup_scr_volume(lv_ui *ui)
{
    // Write codes volume_lmeter_1
    ui->volume_lmeter_1 = lv_linemeter_create(ui->volume, NULL);

    // Write style LV_LINEMETER_PART_MAIN for volume_lmeter_1
    static lv_style_t style_volume_lmeter_1_main;
    lv_style_reset(&style_volume_lmeter_1_main);

    // Write style state: LV_STATE_DEFAULT for style_volume_lmeter_1_main
    lv_style_init(&style_volume_lmeter_1_main);
    lv_style_set_line_width(&style_volume_lmeter_1_main, LV_STATE_DEFAULT, 6);                                    // 活跃刻度线宽度
    lv_style_set_line_color(&style_volume_lmeter_1_main, LV_STATE_DEFAULT, LV_COLOR_GREEN);                       // 活跃刻度线起始颜色
    lv_style_set_scale_grad_color(&style_volume_lmeter_1_main, LV_STATE_DEFAULT, LV_COLOR_RED);                   // 活跃刻度线终止颜色
    lv_style_set_scale_end_color(&style_volume_lmeter_1_main, LV_STATE_DEFAULT, lv_color_make(0x8b, 0x89, 0x8b)); // 非活跃刻度线颜色
    lv_style_set_scale_end_line_width(&style_volume_lmeter_1_main, LV_STATE_DEFAULT, 6);                          // 非活跃刻度线宽度
    lv_style_set_scale_width(&style_volume_lmeter_1_main, LV_STATE_DEFAULT, 15);                                  // 设置刻度线长度

    lv_obj_add_style(ui->volume_lmeter_1, LV_LINEMETER_PART_MAIN, &style_volume_lmeter_1_main);
    lv_obj_set_pos(ui->volume_lmeter_1, 17, 15);
    lv_obj_set_size(ui->volume_lmeter_1, 206, 206);

    lv_linemeter_set_scale(ui->volume_lmeter_1, 300, 50);
    lv_linemeter_set_range(ui->volume_lmeter_1, 0, 100);
    lv_linemeter_set_value(ui->volume_lmeter_1, 0);
    lv_linemeter_set_angle_offset(ui->volume_lmeter_1, 0);

    // Write codes volume_label_1
    ui->volume_label_1 = lv_label_create(ui->volume, NULL);
    lv_label_set_text(ui->volume_label_1, "音量调节");
    lv_label_set_long_mode(ui->volume_label_1, LV_LABEL_LONG_BREAK);
    lv_label_set_align(ui->volume_label_1, LV_LABEL_ALIGN_CENTER);

    // Write style LV_LABEL_PART_MAIN for volume_label_1
    static lv_style_t style_volume_label_1_main;
    lv_style_reset(&style_volume_label_1_main);

    // Write style state: LV_STATE_DEFAULT for style_volume_label_1_main
    lv_style_set_radius(&style_volume_label_1_main, LV_STATE_DEFAULT, 0);
    lv_style_set_bg_color(&style_volume_label_1_main, LV_STATE_DEFAULT, lv_color_make(0x00, 0x00, 0x00));
    lv_style_set_bg_grad_color(&style_volume_label_1_main, LV_STATE_DEFAULT, lv_color_make(0x00, 0x00, 0x00));
    lv_style_set_bg_grad_dir(&style_volume_label_1_main, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
    lv_style_set_bg_opa(&style_volume_label_1_main, LV_STATE_DEFAULT, 0);
    lv_style_set_text_color(&style_volume_label_1_main, LV_STATE_DEFAULT, lv_color_make(0x00, 0x00, 0x00));
    lv_style_set_text_font(&style_volume_label_1_main, LV_STATE_DEFAULT, &lv_font_HarmonyOS_Sans_SC_Regular_14);
    lv_style_set_text_letter_space(&style_volume_label_1_main, LV_STATE_DEFAULT, 2);
    lv_style_set_pad_left(&style_volume_label_1_main, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_right(&style_volume_label_1_main, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_top(&style_volume_label_1_main, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_bottom(&style_volume_label_1_main, LV_STATE_DEFAULT, 0);
    lv_obj_add_style(ui->volume_label_1, LV_LABEL_PART_MAIN, &style_volume_label_1_main);
    lv_obj_set_pos(ui->volume_label_1, 72, 155);
    lv_obj_set_size(ui->volume_label_1, 100, 0);

    // Write codes volume_label_2
    ui->volume_label_2 = lv_label_create(ui->volume, NULL);
    lv_label_set_text(ui->volume_label_2, "0%");
    lv_label_set_long_mode(ui->volume_label_2, LV_LABEL_LONG_BREAK);
    lv_label_set_align(ui->volume_label_2, LV_LABEL_ALIGN_CENTER);
    lv_task_create(task_cb, 300, LV_TASK_PRIO_MID, NULL);

    // Write style LV_LABEL_PART_MAIN for volume_label_2
    static lv_style_t style_volume_label_2_main;
    lv_style_reset(&style_volume_label_2_main);

    // Write style state: LV_STATE_DEFAULT for style_volume_label_2_main
    lv_style_set_radius(&style_volume_label_2_main, LV_STATE_DEFAULT, 0);
    lv_style_set_bg_color(&style_volume_label_2_main, LV_STATE_DEFAULT, lv_color_make(0x00, 0x00, 0x00));
    lv_style_set_bg_grad_color(&style_volume_label_2_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_bg_grad_dir(&style_volume_label_2_main, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
    lv_style_set_bg_opa(&style_volume_label_2_main, LV_STATE_DEFAULT, 0);
    lv_style_set_text_color(&style_volume_label_2_main, LV_STATE_DEFAULT, lv_color_make(0x00, 0x00, 0x00));
    lv_style_set_text_font(&style_volume_label_2_main, LV_STATE_DEFAULT, &lv_font_HarmonyOS_Sans_SC_Regular_24);
    lv_style_set_text_letter_space(&style_volume_label_2_main, LV_STATE_DEFAULT, 2);
    lv_style_set_pad_left(&style_volume_label_2_main, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_right(&style_volume_label_2_main, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_top(&style_volume_label_2_main, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_bottom(&style_volume_label_2_main, LV_STATE_DEFAULT, 0);
    lv_obj_add_style(ui->volume_label_2, LV_LABEL_PART_MAIN, &style_volume_label_2_main);
    lv_obj_set_pos(ui->volume_label_2, 73, 104);
    lv_obj_set_size(ui->volume_label_2, 100, 0);
}

// 初始化菜单界面
void setup_scr_menu(lv_ui *ui)
{
    // Write codes menu_roller_1
    ui->menu_roller_1 = lv_roller_create(ui->menu, NULL);

    // Write style LV_ROLLER_PART_BG for menu_roller_1
    static lv_style_t style_menu_roller_1_bg;
    lv_style_reset(&style_menu_roller_1_bg);

    // Write style state: LV_STATE_DEFAULT for style_menu_roller_1_bg
    lv_style_set_radius(&style_menu_roller_1_bg, LV_STATE_DEFAULT, 5);
    lv_style_set_bg_color(&style_menu_roller_1_bg, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_bg_grad_color(&style_menu_roller_1_bg, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_bg_grad_dir(&style_menu_roller_1_bg, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
    lv_style_set_bg_opa(&style_menu_roller_1_bg, LV_STATE_DEFAULT, 255);
    lv_style_set_border_color(&style_menu_roller_1_bg, LV_STATE_DEFAULT, lv_color_make(0xdf, 0xe7, 0xed));
    lv_style_set_border_width(&style_menu_roller_1_bg, LV_STATE_DEFAULT, 1);
    lv_style_set_text_color(&style_menu_roller_1_bg, LV_STATE_DEFAULT, lv_color_make(0x33, 0x33, 0x33));
    lv_obj_add_style(ui->menu_roller_1, LV_ROLLER_PART_BG, &style_menu_roller_1_bg);

    // Write style LV_ROLLER_PART_SELECTED for menu_roller_1
    static lv_style_t style_menu_roller_1_selected;
    lv_style_reset(&style_menu_roller_1_selected);
    lv_task_create(task_menu, 300, LV_TASK_PRIO_MID, NULL);

    // Write style state: LV_STATE_DEFAULT for style_menu_roller_1_selected
    lv_style_set_bg_color(&style_menu_roller_1_selected, LV_STATE_DEFAULT, lv_color_make(0x01, 0xa2, 0xb1));
    lv_style_set_bg_grad_color(&style_menu_roller_1_selected, LV_STATE_DEFAULT, lv_color_make(0x01, 0xa2, 0xb1));
    lv_style_set_bg_grad_dir(&style_menu_roller_1_selected, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
    lv_style_set_bg_opa(&style_menu_roller_1_selected, LV_STATE_DEFAULT, 255);
    lv_style_set_text_color(&style_menu_roller_1_selected, LV_STATE_DEFAULT, lv_color_make(0xFF, 0xFF, 0xFF));
    lv_obj_add_style(ui->menu_roller_1, LV_ROLLER_PART_SELECTED, &style_menu_roller_1_selected);
    lv_obj_set_pos(ui->menu_roller_1, 83, 69);
    lv_roller_set_options(ui->menu_roller_1, "音量控制\n移动控制\n媒体控制\n兼容模式", LV_ROLLER_MODE_NORMAL);
    lv_obj_set_style_local_text_font(ui->menu_roller_1, LV_ROLLER_PART_BG, LV_STATE_DEFAULT, &lv_font_HarmonyOS_Sans_SC_Regular_14);
    lv_obj_set_style_local_text_font(ui->menu_roller_1, LV_ROLLER_PART_BG, LV_STATE_FOCUSED, &lv_font_HarmonyOS_Sans_SC_Regular_14);
    lv_obj_set_style_local_text_font(ui->menu_roller_1, LV_ROLLER_PART_SELECTED, LV_STATE_DEFAULT, &lv_font_HarmonyOS_Sans_SC_Regular_14);
    lv_roller_set_visible_row_count(ui->menu_roller_1, 4);

    // Write codes menu_label_1
    ui->menu_label_1 = lv_label_create(ui->menu, NULL);
    lv_label_set_text(ui->menu_label_1, "功能菜单");
    lv_label_set_long_mode(ui->menu_label_1, LV_LABEL_LONG_BREAK);
    lv_label_set_align(ui->menu_label_1, LV_LABEL_ALIGN_CENTER);

    // Write style LV_LABEL_PART_MAIN for menu_label_1
    static lv_style_t style_menu_label_1_main;
    lv_style_reset(&style_menu_label_1_main);

    // Write style state: LV_STATE_DEFAULT for style_menu_label_1_main
    lv_style_set_radius(&style_menu_label_1_main, LV_STATE_DEFAULT, 0);
    lv_style_set_bg_color(&style_menu_label_1_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_bg_grad_color(&style_menu_label_1_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_bg_grad_dir(&style_menu_label_1_main, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
    lv_style_set_bg_opa(&style_menu_label_1_main, LV_STATE_DEFAULT, 0);
    lv_style_set_text_color(&style_menu_label_1_main, LV_STATE_DEFAULT, lv_color_make(0x00, 0x00, 0x00));
    lv_style_set_text_font(&style_menu_label_1_main, LV_STATE_DEFAULT, &lv_font_HarmonyOS_Sans_SC_Regular_24);
    lv_style_set_text_letter_space(&style_menu_label_1_main, LV_STATE_DEFAULT, 2);
    lv_style_set_pad_left(&style_menu_label_1_main, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_right(&style_menu_label_1_main, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_top(&style_menu_label_1_main, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_bottom(&style_menu_label_1_main, LV_STATE_DEFAULT, 0);
    lv_obj_add_style(ui->menu_label_1, LV_LABEL_PART_MAIN, &style_menu_label_1_main);
    lv_obj_set_pos(ui->menu_label_1, 68, 35);
    lv_obj_set_size(ui->menu_label_1, 110, 0);
}

// 初始化移动控制
void setup_scr_slide(lv_ui *ui)
{
    // Write codes slide_arc_1
    ui->slide_arc_1 = lv_arc_create(ui->slide, NULL);

    // Write style LV_ARC_PART_BG for slide_arc_1
    static lv_style_t style_slide_arc_1_bg;
    lv_style_reset(&style_slide_arc_1_bg);
    lv_task_create(task_slide, 100, LV_TASK_PRIO_MID, NULL);

    // Write style state: LV_STATE_DEFAULT for style_slide_arc_1_bg
    lv_style_set_bg_color(&style_slide_arc_1_bg, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_bg_grad_color(&style_slide_arc_1_bg, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_bg_grad_dir(&style_slide_arc_1_bg, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
    lv_style_set_bg_opa(&style_slide_arc_1_bg, LV_STATE_DEFAULT, 0);
    lv_style_set_border_width(&style_slide_arc_1_bg, LV_STATE_DEFAULT, 0);
    lv_style_set_line_color(&style_slide_arc_1_bg, LV_STATE_DEFAULT, lv_color_make(0xd4, 0xd7, 0xd9));
    lv_style_set_line_width(&style_slide_arc_1_bg, LV_STATE_DEFAULT, 16);
    lv_obj_add_style(ui->slide_arc_1, LV_ARC_PART_BG, &style_slide_arc_1_bg);

    // Write style LV_ARC_PART_INDIC for slide_arc_1
    static lv_style_t style_slide_arc_1_indic;
    lv_style_reset(&style_slide_arc_1_indic);

    // Write style state: LV_STATE_DEFAULT for style_slide_arc_1_indic
    lv_style_set_line_color(&style_slide_arc_1_indic, LV_STATE_DEFAULT, lv_color_make(0x01, 0xa2, 0xb1));
    lv_style_set_line_width(&style_slide_arc_1_indic, LV_STATE_DEFAULT, 16);
    lv_obj_add_style(ui->slide_arc_1, LV_ARC_PART_INDIC, &style_slide_arc_1_indic);
    lv_obj_set_pos(ui->slide_arc_1, 38, 12);
    lv_obj_set_size(ui->slide_arc_1, 162, 170);
    lv_arc_set_bg_angles(ui->slide_arc_1, 0, 360);
    lv_arc_set_angles(ui->slide_arc_1, 90, 150);
    lv_arc_set_rotation(ui->slide_arc_1, 0);
    lv_obj_set_style_local_pad_top(ui->slide_arc_1, LV_ARC_PART_BG, LV_STATE_DEFAULT, 20);
    lv_obj_set_style_local_pad_bottom(ui->slide_arc_1, LV_ARC_PART_BG, LV_STATE_DEFAULT, 20);
    lv_obj_set_style_local_pad_left(ui->slide_arc_1, LV_ARC_PART_BG, LV_STATE_DEFAULT, 20);
    lv_obj_set_style_local_pad_right(ui->slide_arc_1, LV_ARC_PART_BG, LV_STATE_DEFAULT, 20);
    lv_obj_set_style_local_line_rounded(ui->slide_arc_1, LV_ARC_PART_INDIC, LV_STATE_DEFAULT, 0);
    lv_obj_set_style_local_line_rounded(ui->slide_arc_1, LV_ARC_PART_BG, LV_STATE_DEFAULT, 0);

    // Write codes slide_label_1
    ui->slide_label_1 = lv_label_create(ui->slide, NULL);
    lv_label_set_text(ui->slide_label_1, "移动控制");
    lv_label_set_long_mode(ui->slide_label_1, LV_LABEL_LONG_BREAK);
    lv_label_set_align(ui->slide_label_1, LV_LABEL_ALIGN_CENTER);

    // Write style LV_LABEL_PART_MAIN for slide_label_1
    static lv_style_t style_slide_label_1_main;
    lv_style_reset(&style_slide_label_1_main);

    // Write style state: LV_STATE_DEFAULT for style_slide_label_1_main
    lv_style_set_radius(&style_slide_label_1_main, LV_STATE_DEFAULT, 0);
    lv_style_set_bg_color(&style_slide_label_1_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_bg_grad_color(&style_slide_label_1_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_bg_grad_dir(&style_slide_label_1_main, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
    lv_style_set_bg_opa(&style_slide_label_1_main, LV_STATE_DEFAULT, 0);
    lv_style_set_text_color(&style_slide_label_1_main, LV_STATE_DEFAULT, lv_color_make(0x00, 0x00, 0x00));
    lv_style_set_text_font(&style_slide_label_1_main, LV_STATE_DEFAULT, &lv_font_HarmonyOS_Sans_SC_Regular_14);
    lv_style_set_text_letter_space(&style_slide_label_1_main, LV_STATE_DEFAULT, 2);
    lv_style_set_pad_left(&style_slide_label_1_main, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_right(&style_slide_label_1_main, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_top(&style_slide_label_1_main, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_bottom(&style_slide_label_1_main, LV_STATE_DEFAULT, 0);
    lv_obj_add_style(ui->slide_label_1, LV_LABEL_PART_MAIN, &style_slide_label_1_main);
    lv_obj_set_pos(ui->slide_label_1, 70, 173);
    lv_obj_set_size(ui->slide_label_1, 100, 0);
}

// 初始化媒体控制页面
void setup_scr_media(lv_ui *ui)
{
    // Write codes media_img_2
    ui->media_img_2 = lv_img_create(ui->media, NULL);

    // Write style LV_IMG_PART_MAIN for media_img_2
    static lv_style_t style_media_img_2_main;
    lv_style_reset(&style_media_img_2_main);

    // Write style state: LV_STATE_DEFAULT for style_media_img_2_main
    lv_style_set_image_recolor(&style_media_img_2_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_image_recolor_opa(&style_media_img_2_main, LV_STATE_DEFAULT, 0);
    lv_style_set_image_opa(&style_media_img_2_main, LV_STATE_DEFAULT, 255);
    lv_obj_add_style(ui->media_img_2, LV_IMG_PART_MAIN, &style_media_img_2_main);
    lv_obj_set_pos(ui->media_img_2, 36, 85);
    lv_obj_set_size(ui->media_img_2, 50, 50);
    lv_obj_set_click(ui->media_img_2, true);
    lv_img_set_src(ui->media_img_2, &_skip_previous_FILL0_wght400_GRAD0_opsz48_alpha_50x50);
    lv_img_set_pivot(ui->media_img_2, 0, 0);
    lv_img_set_angle(ui->media_img_2, 0);

    // Write codes media_img_3
    ui->media_img_3 = lv_img_create(ui->media, NULL);

    // Write style LV_IMG_PART_MAIN for media_img_3
    static lv_style_t style_media_img_3_main;
    lv_style_reset(&style_media_img_3_main);

    // Write style state: LV_STATE_DEFAULT for style_media_img_3_main
    lv_style_set_image_recolor(&style_media_img_3_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_image_recolor_opa(&style_media_img_3_main, LV_STATE_DEFAULT, 0);
    lv_style_set_image_opa(&style_media_img_3_main, LV_STATE_DEFAULT, 255);
    lv_obj_add_style(ui->media_img_3, LV_IMG_PART_MAIN, &style_media_img_3_main);
    lv_obj_set_pos(ui->media_img_3, 150, 85);
    lv_obj_set_size(ui->media_img_3, 50, 50);
    lv_obj_set_click(ui->media_img_3, true);
    lv_img_set_src(ui->media_img_3, &_skip_next_FILL0_wght400_GRAD0_opsz48_alpha_50x50);
    lv_img_set_pivot(ui->media_img_3, 0, 0);
    lv_img_set_angle(ui->media_img_3, 0);

    // Write codes media_label_1
    ui->media_label_1 = lv_label_create(ui->media, NULL);
    lv_label_set_text(ui->media_label_1, "媒体控制");
    lv_label_set_long_mode(ui->media_label_1, LV_LABEL_LONG_BREAK);
    lv_label_set_align(ui->media_label_1, LV_LABEL_ALIGN_CENTER);

    // Write style LV_LABEL_PART_MAIN for media_label_1
    static lv_style_t style_media_label_1_main;
    lv_style_reset(&style_media_label_1_main);

    // Write style state: LV_STATE_DEFAULT for style_media_label_1_main
    lv_style_set_radius(&style_media_label_1_main, LV_STATE_DEFAULT, 0);
    lv_style_set_bg_color(&style_media_label_1_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_bg_grad_color(&style_media_label_1_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_bg_grad_dir(&style_media_label_1_main, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
    lv_style_set_bg_opa(&style_media_label_1_main, LV_STATE_DEFAULT, 0);
    lv_style_set_text_color(&style_media_label_1_main, LV_STATE_DEFAULT, lv_color_make(0x00, 0x00, 0x00));
    lv_style_set_text_font(&style_media_label_1_main, LV_STATE_DEFAULT, &lv_font_HarmonyOS_Sans_SC_Regular_14);
    lv_style_set_text_letter_space(&style_media_label_1_main, LV_STATE_DEFAULT, 2);
    lv_style_set_pad_left(&style_media_label_1_main, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_right(&style_media_label_1_main, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_top(&style_media_label_1_main, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_bottom(&style_media_label_1_main, LV_STATE_DEFAULT, 0);
    lv_obj_add_style(ui->media_label_1, LV_LABEL_PART_MAIN, &style_media_label_1_main);
    lv_obj_set_pos(ui->media_label_1, 69, 166);
    lv_obj_set_size(ui->media_label_1, 100, 0);

    // Write codes media_img_4
    ui->media_img_4 = lv_img_create(ui->media, NULL);

    // Write style LV_IMG_PART_MAIN for media_img_4
    static lv_style_t style_media_img_4_main;
    lv_style_reset(&style_media_img_4_main);

    // Write style state: LV_STATE_DEFAULT for style_media_img_4_main
    lv_style_set_image_recolor(&style_media_img_4_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_image_recolor_opa(&style_media_img_4_main, LV_STATE_DEFAULT, 0);
    lv_style_set_image_opa(&style_media_img_4_main, LV_STATE_DEFAULT, 255);
    lv_obj_add_style(ui->media_img_4, LV_IMG_PART_MAIN, &style_media_img_4_main);
    lv_obj_set_pos(ui->media_img_4, 95, 85);
    lv_obj_set_size(ui->media_img_4, 50, 50);
    lv_obj_set_click(ui->media_img_4, true);
    lv_img_set_src(ui->media_img_4, &_play_circle_FILL0_wght400_GRAD0_opsz48_alpha_50x50);
    lv_img_set_pivot(ui->media_img_4, 0, 0);
    lv_img_set_angle(ui->media_img_4, 0);
}

// 兼容模式确认页面
void setup_scr_setup1(lv_ui *ui)
{

    // Write codes setup1_sw_1
    ui->setup1_sw_1 = lv_switch_create(ui->setup1, NULL);
    lv_task_create(task_switch, 300, LV_TASK_PRIO_MID, NULL);

    // Write style LV_SWITCH_PART_BG for setup1_sw_1
    static lv_style_t style_setup1_sw_1_bg;
    lv_style_reset(&style_setup1_sw_1_bg);

    // Write style state: LV_STATE_DEFAULT for style_setup1_sw_1_bg
    lv_style_set_radius(&style_setup1_sw_1_bg, LV_STATE_DEFAULT, 100);
    lv_style_set_bg_color(&style_setup1_sw_1_bg, LV_STATE_DEFAULT, lv_color_make(0xd4, 0xd7, 0xd9));
    lv_style_set_bg_grad_color(&style_setup1_sw_1_bg, LV_STATE_DEFAULT, lv_color_make(0xd4, 0xd7, 0xd9));
    lv_style_set_bg_grad_dir(&style_setup1_sw_1_bg, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
    lv_style_set_bg_opa(&style_setup1_sw_1_bg, LV_STATE_DEFAULT, 255);
    lv_obj_add_style(ui->setup1_sw_1, LV_SWITCH_PART_BG, &style_setup1_sw_1_bg);

    // Write style LV_SWITCH_PART_INDIC for setup1_sw_1
    static lv_style_t style_setup1_sw_1_indic;
    lv_style_reset(&style_setup1_sw_1_indic);

    // Write style state: LV_STATE_DEFAULT for style_setup1_sw_1_indic
    lv_style_set_radius(&style_setup1_sw_1_indic, LV_STATE_DEFAULT, 100);
    lv_style_set_bg_color(&style_setup1_sw_1_indic, LV_STATE_DEFAULT, lv_color_make(0x00, 0xa1, 0xb5));
    lv_style_set_bg_grad_color(&style_setup1_sw_1_indic, LV_STATE_DEFAULT, lv_color_make(0x00, 0xa1, 0xb5));
    lv_style_set_bg_grad_dir(&style_setup1_sw_1_indic, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
    lv_style_set_bg_opa(&style_setup1_sw_1_indic, LV_STATE_DEFAULT, 255);
    lv_obj_add_style(ui->setup1_sw_1, LV_SWITCH_PART_INDIC, &style_setup1_sw_1_indic);

    // Write style LV_SWITCH_PART_KNOB for setup1_sw_1
    static lv_style_t style_setup1_sw_1_knob;
    lv_style_reset(&style_setup1_sw_1_knob);

    // Write style state: LV_STATE_DEFAULT for style_setup1_sw_1_knob
    lv_style_set_radius(&style_setup1_sw_1_knob, LV_STATE_DEFAULT, 100);
    lv_style_set_bg_color(&style_setup1_sw_1_knob, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_bg_grad_color(&style_setup1_sw_1_knob, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_bg_grad_dir(&style_setup1_sw_1_knob, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
    lv_style_set_bg_opa(&style_setup1_sw_1_knob, LV_STATE_DEFAULT, 255);
    lv_obj_add_style(ui->setup1_sw_1, LV_SWITCH_PART_KNOB, &style_setup1_sw_1_knob);
    lv_obj_set_pos(ui->setup1_sw_1, 72, 61);
    lv_obj_set_size(ui->setup1_sw_1, 96, 44);
    lv_switch_set_anim_time(ui->setup1_sw_1, 300);

    // Write codes setup1_label_1
    ui->setup1_label_1 = lv_label_create(ui->setup1, NULL);
    lv_label_set_text(ui->setup1_label_1, "启动兼容模式后");
    lv_label_set_long_mode(ui->setup1_label_1, LV_LABEL_LONG_BREAK);
    lv_label_set_align(ui->setup1_label_1, LV_LABEL_ALIGN_CENTER);

    // Write style LV_LABEL_PART_MAIN for setup1_label_1
    static lv_style_t style_setup1_label_1_main;
    lv_style_reset(&style_setup1_label_1_main);

    // Write style state: LV_STATE_DEFAULT for style_setup1_label_1_main
    lv_style_set_radius(&style_setup1_label_1_main, LV_STATE_DEFAULT, 0);
    lv_style_set_bg_color(&style_setup1_label_1_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_bg_grad_color(&style_setup1_label_1_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_bg_grad_dir(&style_setup1_label_1_main, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
    lv_style_set_bg_opa(&style_setup1_label_1_main, LV_STATE_DEFAULT, 0);
    lv_style_set_text_color(&style_setup1_label_1_main, LV_STATE_DEFAULT, lv_color_make(0x00, 0x00, 0x00));
    lv_style_set_text_font(&style_setup1_label_1_main, LV_STATE_DEFAULT, &lv_font_HarmonyOS_Sans_SC_Regular_14);
    lv_style_set_text_letter_space(&style_setup1_label_1_main, LV_STATE_DEFAULT, 2);
    lv_style_set_pad_left(&style_setup1_label_1_main, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_right(&style_setup1_label_1_main, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_top(&style_setup1_label_1_main, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_bottom(&style_setup1_label_1_main, LV_STATE_DEFAULT, 0);
    lv_obj_add_style(ui->setup1_label_1, LV_LABEL_PART_MAIN, &style_setup1_label_1_main);
    lv_obj_set_pos(ui->setup1_label_1, 44.5, 122);
    lv_obj_set_size(ui->setup1_label_1, 151, 0);

    // Write codes setup1_label_2
    ui->setup1_label_2 = lv_label_create(ui->setup1, NULL);
    lv_label_set_text(ui->setup1_label_2, "需通过重启回到常规模式");
    lv_label_set_long_mode(ui->setup1_label_2, LV_LABEL_LONG_BREAK);
    lv_label_set_align(ui->setup1_label_2, LV_LABEL_ALIGN_CENTER);

    // Write style LV_LABEL_PART_MAIN for setup1_label_2
    static lv_style_t style_setup1_label_2_main;
    lv_style_reset(&style_setup1_label_2_main);

    // Write style state: LV_STATE_DEFAULT for style_setup1_label_2_main
    lv_style_set_radius(&style_setup1_label_2_main, LV_STATE_DEFAULT, 0);
    lv_style_set_bg_color(&style_setup1_label_2_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_bg_grad_color(&style_setup1_label_2_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_bg_grad_dir(&style_setup1_label_2_main, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
    lv_style_set_bg_opa(&style_setup1_label_2_main, LV_STATE_DEFAULT, 0);
    lv_style_set_text_color(&style_setup1_label_2_main, LV_STATE_DEFAULT, lv_color_make(0x00, 0x00, 0x00));
    lv_style_set_text_font(&style_setup1_label_2_main, LV_STATE_DEFAULT, &lv_font_HarmonyOS_Sans_SC_Regular_14);
    lv_style_set_text_letter_space(&style_setup1_label_2_main, LV_STATE_DEFAULT, 2);
    lv_style_set_pad_left(&style_setup1_label_2_main, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_right(&style_setup1_label_2_main, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_top(&style_setup1_label_2_main, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_bottom(&style_setup1_label_2_main, LV_STATE_DEFAULT, 0);
    lv_obj_add_style(ui->setup1_label_2, LV_LABEL_PART_MAIN, &style_setup1_label_2_main);
    lv_obj_set_pos(ui->setup1_label_2, 31, 144);
    lv_obj_set_size(ui->setup1_label_2, 178, 0);

    // Write codes setup1_label_3
    ui->setup1_label_3 = lv_label_create(ui->setup1, NULL);
    lv_label_set_text(ui->setup1_label_3, "请确认操作");
    lv_label_set_long_mode(ui->setup1_label_3, LV_LABEL_LONG_BREAK);
    lv_label_set_align(ui->setup1_label_3, LV_LABEL_ALIGN_CENTER);

    // Write style LV_LABEL_PART_MAIN for setup1_label_3
    static lv_style_t style_setup1_label_3_main;
    lv_style_reset(&style_setup1_label_3_main);

    // Write style state: LV_STATE_DEFAULT for style_setup1_label_3_main
    lv_style_set_radius(&style_setup1_label_3_main, LV_STATE_DEFAULT, 0);
    lv_style_set_bg_color(&style_setup1_label_3_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_bg_grad_color(&style_setup1_label_3_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_bg_grad_dir(&style_setup1_label_3_main, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
    lv_style_set_bg_opa(&style_setup1_label_3_main, LV_STATE_DEFAULT, 0);
    lv_style_set_text_color(&style_setup1_label_3_main, LV_STATE_DEFAULT, lv_color_make(0x00, 0x00, 0x00));
    lv_style_set_text_font(&style_setup1_label_3_main, LV_STATE_DEFAULT, &lv_font_HarmonyOS_Sans_SC_Regular_24);
    lv_style_set_text_letter_space(&style_setup1_label_3_main, LV_STATE_DEFAULT, 2);
    lv_style_set_pad_left(&style_setup1_label_3_main, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_right(&style_setup1_label_3_main, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_top(&style_setup1_label_3_main, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_bottom(&style_setup1_label_3_main, LV_STATE_DEFAULT, 0);
    lv_obj_add_style(ui->setup1_label_3, LV_LABEL_PART_MAIN, &style_setup1_label_3_main);
    lv_obj_set_pos(ui->setup1_label_3, 31, 170);
    lv_obj_set_size(ui->setup1_label_3, 178, 0);
}

// 兼容模式配置页面
void setup_scr_setup2(lv_ui *ui)
{

    // Write codes setup2_roller_1
    ui->setup2_roller_1 = lv_roller_create(ui->setup2, NULL);
    lv_task_create(task_setup, 300, LV_TASK_PRIO_MID, NULL);

    // Write style LV_ROLLER_PART_BG for setup2_roller_1
    static lv_style_t style_setup2_roller_1_bg;
    lv_style_reset(&style_setup2_roller_1_bg);

    // Write style state: LV_STATE_DEFAULT for style_setup2_roller_1_bg
    lv_style_set_radius(&style_setup2_roller_1_bg, LV_STATE_DEFAULT, 5);
    lv_style_set_bg_color(&style_setup2_roller_1_bg, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_bg_grad_color(&style_setup2_roller_1_bg, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_bg_grad_dir(&style_setup2_roller_1_bg, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
    lv_style_set_bg_opa(&style_setup2_roller_1_bg, LV_STATE_DEFAULT, 2);
    lv_style_set_border_color(&style_setup2_roller_1_bg, LV_STATE_DEFAULT, lv_color_make(0xdf, 0xe7, 0xed));
    lv_style_set_border_width(&style_setup2_roller_1_bg, LV_STATE_DEFAULT, 0);
    lv_style_set_text_color(&style_setup2_roller_1_bg, LV_STATE_DEFAULT, lv_color_make(0x33, 0x33, 0x33));
    lv_obj_add_style(ui->setup2_roller_1, LV_ROLLER_PART_BG, &style_setup2_roller_1_bg);

    // Write style LV_ROLLER_PART_SELECTED for setup2_roller_1
    static lv_style_t style_setup2_roller_1_selected;
    lv_style_reset(&style_setup2_roller_1_selected);

    // Write style state: LV_STATE_DEFAULT for style_setup2_roller_1_selected
    lv_style_set_bg_color(&style_setup2_roller_1_selected, LV_STATE_DEFAULT, lv_color_make(0x01, 0xa2, 0xb1));
    lv_style_set_bg_grad_color(&style_setup2_roller_1_selected, LV_STATE_DEFAULT, lv_color_make(0x01, 0xa2, 0xb1));
    lv_style_set_bg_grad_dir(&style_setup2_roller_1_selected, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
    lv_style_set_bg_opa(&style_setup2_roller_1_selected, LV_STATE_DEFAULT, 255);
    lv_style_set_text_color(&style_setup2_roller_1_selected, LV_STATE_DEFAULT, lv_color_make(0xFF, 0xFF, 0xFF));
    lv_obj_add_style(ui->setup2_roller_1, LV_ROLLER_PART_SELECTED, &style_setup2_roller_1_selected);
    lv_obj_set_pos(ui->setup2_roller_1, 89, 61);
    lv_roller_set_options(ui->setup2_roller_1, "无阻尼\n弱阻尼\n强阻尼", LV_ROLLER_MODE_NORMAL);
    lv_obj_set_style_local_text_font(ui->setup2_roller_1, LV_ROLLER_PART_BG, LV_STATE_DEFAULT, &lv_font_HarmonyOS_Sans_SC_Regular1_14);
    lv_obj_set_style_local_text_font(ui->setup2_roller_1, LV_ROLLER_PART_BG, LV_STATE_FOCUSED, &lv_font_HarmonyOS_Sans_SC_Regular1_14);
    lv_obj_set_style_local_text_font(ui->setup2_roller_1, LV_ROLLER_PART_SELECTED, LV_STATE_DEFAULT, &lv_font_HarmonyOS_Sans_SC_Regular1_14);
    lv_roller_set_visible_row_count(ui->setup2_roller_1, 3);

    // Write codes setup2_label_1
    ui->setup2_label_1 = lv_label_create(ui->setup2, NULL);
    lv_label_set_text(ui->setup2_label_1, "自定义阻尼模式");
    lv_label_set_long_mode(ui->setup2_label_1, LV_LABEL_LONG_BREAK);
    lv_label_set_align(ui->setup2_label_1, LV_LABEL_ALIGN_CENTER);

    // Write style LV_LABEL_PART_MAIN for setup2_label_1
    static lv_style_t style_setup2_label_1_main;
    lv_style_reset(&style_setup2_label_1_main);

    // Write style state: LV_STATE_DEFAULT for style_setup2_label_1_main
    lv_style_set_radius(&style_setup2_label_1_main, LV_STATE_DEFAULT, 0);
    lv_style_set_bg_color(&style_setup2_label_1_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_bg_grad_color(&style_setup2_label_1_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_bg_grad_dir(&style_setup2_label_1_main, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
    lv_style_set_bg_opa(&style_setup2_label_1_main, LV_STATE_DEFAULT, 0);
    lv_style_set_text_color(&style_setup2_label_1_main, LV_STATE_DEFAULT, lv_color_make(0x00, 0x00, 0x00));
    lv_style_set_text_font(&style_setup2_label_1_main, LV_STATE_DEFAULT, &lv_font_HarmonyOS_Sans_SC_Regular1_14);
    lv_style_set_text_letter_space(&style_setup2_label_1_main, LV_STATE_DEFAULT, 2);
    lv_style_set_pad_left(&style_setup2_label_1_main, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_right(&style_setup2_label_1_main, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_top(&style_setup2_label_1_main, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_bottom(&style_setup2_label_1_main, LV_STATE_DEFAULT, 0);
    lv_obj_add_style(ui->setup2_label_1, LV_LABEL_PART_MAIN, &style_setup2_label_1_main);
    lv_obj_set_pos(ui->setup2_label_1, 57, 40);
    lv_obj_set_size(ui->setup2_label_1, 126, 0);

    // Write codes setup2_img_1
    ui->setup2_img_1 = lv_img_create(ui->setup2, NULL);

    // Write style LV_IMG_PART_MAIN for setup2_img_1
    static lv_style_t style_setup2_img_1_main;
    lv_style_reset(&style_setup2_img_1_main);

    // Write style state: LV_STATE_DEFAULT for style_setup2_img_1_main
    lv_style_set_image_recolor(&style_setup2_img_1_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_image_recolor_opa(&style_setup2_img_1_main, LV_STATE_DEFAULT, 0);
    lv_style_set_image_opa(&style_setup2_img_1_main, LV_STATE_DEFAULT, 255);
    lv_obj_add_style(ui->setup2_img_1, LV_IMG_PART_MAIN, &style_setup2_img_1_main);
    lv_obj_set_pos(ui->setup2_img_1, 50, 170);
    lv_obj_set_size(ui->setup2_img_1, 35, 35);
    lv_obj_set_click(ui->setup2_img_1, true);
    lv_img_set_src(ui->setup2_img_1, &_arrow_circle_right_FILL1_wght400_GRAD200_opsz48_alpha_35x35);
    lv_img_set_pivot(ui->setup2_img_1, 0, 0);
    lv_img_set_angle(ui->setup2_img_1, 0);

    // Write codes setup2_label_2
    ui->setup2_label_2 = lv_label_create(ui->setup2, NULL);
    lv_label_set_text(ui->setup2_label_2, "启动兼容模式");
    lv_label_set_long_mode(ui->setup2_label_2, LV_LABEL_LONG_BREAK);
    lv_label_set_align(ui->setup2_label_2, LV_LABEL_ALIGN_CENTER);

    // Write style LV_LABEL_PART_MAIN for setup2_label_2
    static lv_style_t style_setup2_label_2_main;
    lv_style_reset(&style_setup2_label_2_main);

    // Write style state: LV_STATE_DEFAULT for style_setup2_label_2_main
    lv_style_set_radius(&style_setup2_label_2_main, LV_STATE_DEFAULT, 0);
    lv_style_set_bg_color(&style_setup2_label_2_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_bg_grad_color(&style_setup2_label_2_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_bg_grad_dir(&style_setup2_label_2_main, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
    lv_style_set_bg_opa(&style_setup2_label_2_main, LV_STATE_DEFAULT, 0);
    lv_style_set_text_color(&style_setup2_label_2_main, LV_STATE_DEFAULT, lv_color_make(0x00, 0x00, 0x00));
    lv_style_set_text_font(&style_setup2_label_2_main, LV_STATE_DEFAULT, &lv_font_HarmonyOS_Sans_SC_Regular1_14);
    lv_style_set_text_letter_space(&style_setup2_label_2_main, LV_STATE_DEFAULT, 2);
    lv_style_set_pad_left(&style_setup2_label_2_main, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_right(&style_setup2_label_2_main, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_top(&style_setup2_label_2_main, LV_STATE_DEFAULT, 0);
    lv_style_set_pad_bottom(&style_setup2_label_2_main, LV_STATE_DEFAULT, 0);
    lv_obj_add_style(ui->setup2_label_2, LV_LABEL_PART_MAIN, &style_setup2_label_2_main);
    lv_obj_set_pos(ui->setup2_label_2, 90, 180);
    lv_obj_set_size(ui->setup2_label_2, 100, 0);
}

// 初始化Logo页面
void setup_scr_logo(lv_ui *ui)
{
    // Write codes logo_img_1
    ui->logo_img_1 = lv_img_create(ui->logo, NULL);

    // Write style LV_IMG_PART_MAIN for logo_img_1
    static lv_style_t style_logo_img_1_main;
    lv_style_reset(&style_logo_img_1_main);

    // Write style state: LV_STATE_DEFAULT for style_logo_img_1_main
    lv_style_set_image_recolor(&style_logo_img_1_main, LV_STATE_DEFAULT, lv_color_make(0xff, 0xff, 0xff));
    lv_style_set_image_recolor_opa(&style_logo_img_1_main, LV_STATE_DEFAULT, 0);
    lv_style_set_image_opa(&style_logo_img_1_main, LV_STATE_DEFAULT, 255);
    lv_obj_add_style(ui->logo_img_1, LV_IMG_PART_MAIN, &style_logo_img_1_main);
    lv_obj_align(ui->logo_img_1, ui->logo, LV_ALIGN_CENTER, -83, -100); // 居中对齐
    lv_obj_set_click(ui->logo_img_1, true);
    lv_img_set_src(ui->logo_img_1, &_logo_alpha_200x200);
    lv_obj_set_size(ui->logo_img_1, 200, 200);
    lv_img_set_zoom(ui->logo_img_1, 320);
    lv_img_set_pivot(ui->logo_img_1, 0, 0);
    lv_img_set_angle(ui->logo_img_1, 0);
}

// 初始化ui
void setup_ui(lv_ui *ui)
{
    LV_THEME_DEFAULT_INIT(LV_THEME_DEFAULT_COLOR_PRIMARY, LV_THEME_DEFAULT_COLOR_SECONDARY,
                          LV_THEME_MATERIAL_FLAG_LIGHT, LV_THEME_DEFAULT_FONT_SMALL, LV_THEME_DEFAULT_FONT_NORMAL,
                          LV_THEME_DEFAULT_FONT_SUBTITLE, LV_THEME_DEFAULT_FONT_TITLE);
    setup_scr_screen(ui);
    lv_scr_load(ui->screen);
    setup_scr_volume(ui);
    lv_scr_load_anim(ui->volume, LV_SCR_LOAD_ANIM_FADE_ON, 500, 1200, false);
    setup_scr_menu(ui);
    setup_scr_slide(ui);
    setup_scr_media(ui);
    setup_scr_setup1(ui);
    setup_scr_setup2(ui);
    setup_scr_logo(ui);
}

// ui循环
void ui_loop(lv_ui *ui)
{
    if (conf == prevous_conf)
    {
        return;
    }
    else
    {
        // 5是菜单
        if (conf == 5)
        {
            lv_scr_load_anim(ui->menu, LV_SCR_LOAD_ANIM_OVER_BOTTOM, 500, 0, false);
        }
        else
        {
            switch (conf)
            {
            case 0:
                lv_scr_load_anim(ui->volume, LV_SCR_LOAD_ANIM_OVER_TOP, 500, 0, false);
                break;
            case 1:
                lv_scr_load_anim(ui->slide, LV_SCR_LOAD_ANIM_OVER_TOP, 500, 0, false);
                break;
            case 2:
                lv_scr_load_anim(ui->media, LV_SCR_LOAD_ANIM_OVER_TOP, 500, 0, false);
                break;
            case 3:
                lv_scr_load_anim(ui->setup1, LV_SCR_LOAD_ANIM_OVER_TOP, 500, 0, false);
                break;
            default:
                break;
            }
        }
        prevous_conf = conf;
    }
}

// 初始化
void gui_init(void)
{
    lv_init();

    tft.begin();        /* TFT init */
    tft.setRotation(0); /* Landscape orientation */

    lv_disp_buf_init(&disp_buf, buf, NULL, LV_HOR_RES_MAX * 10);

    /*Initialize the display*/
    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = 240;
    disp_drv.ver_res = 240;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.buffer = &disp_buf;
    lv_disp_drv_register(&disp_drv);

    guider_ui.screen = lv_obj_create(NULL, NULL);
    guider_ui.volume = lv_obj_create(NULL, NULL);
    guider_ui.menu = lv_obj_create(NULL, NULL);
    guider_ui.slide = lv_obj_create(NULL, NULL);
    guider_ui.media = lv_obj_create(NULL, NULL);
    guider_ui.setup1 = lv_obj_create(NULL, NULL);
    guider_ui.setup2 = lv_obj_create(NULL, NULL);
    guider_ui.logo = lv_obj_create(NULL, NULL);

    setup_ui(&guider_ui);
}

void gui_loop(void)
{
    ui_loop(&guider_ui);
}