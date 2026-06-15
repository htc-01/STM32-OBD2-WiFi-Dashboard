/*
* Copyright 2026 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "lvgl.h"
#include <stdio.h>
#include "gui_guider.h"
#include "events_init.h"
#include "widgets_init.h"
#include "custom.h"



void setup_scr_speed(lv_ui *ui)
{
    //Write codes speed
    ui->speed = lv_obj_create(NULL);
    lv_obj_set_size(ui->speed, 240, 240);
    lv_obj_set_scrollbar_mode(ui->speed, LV_SCROLLBAR_MODE_OFF);

    //Write style for speed, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->speed, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->speed, lv_color_hex(0x364498), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->speed, LV_GRAD_DIR_HOR, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_color(ui->speed, lv_color_hex(0x0D2C73), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_main_stop(ui->speed, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_stop(ui->speed, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes speed_cont_box
    ui->speed_cont_box = lv_obj_create(ui->speed);
    lv_obj_set_pos(ui->speed_cont_box, 0, 0);
    lv_obj_set_size(ui->speed_cont_box, 178, 239);
    lv_obj_set_scrollbar_mode(ui->speed_cont_box, LV_SCROLLBAR_MODE_OFF);

    //Write style for speed_cont_box, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->speed_cont_box, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->speed_cont_box, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->speed_cont_box, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->speed_cont_box, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->speed_cont_box, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->speed_cont_box, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->speed_cont_box, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->speed_cont_box, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes speed_meter_board
    ui->speed_meter_board = lv_scale_create(ui->speed_cont_box);
    lv_obj_set_pos(ui->speed_meter_board, 0, 0);
    lv_obj_set_size(ui->speed_meter_board, 240, 240);
    lv_obj_update_layout(ui->speed_meter_board);
    lv_scale_set_mode(ui->speed_meter_board, LV_SCALE_MODE_ROUND_INNER);
    lv_scale_set_total_tick_count(ui->speed_meter_board, 61);
    lv_scale_set_major_tick_every(ui->speed_meter_board, 5);
    lv_scale_set_label_show(ui->speed_meter_board, true);
    lv_scale_set_range(ui->speed_meter_board, 0, 120);
    lv_scale_set_angle_range(ui->speed_meter_board, 300);
    lv_scale_set_rotation(ui->speed_meter_board, 120);
    lv_scale_set_post_draw(ui->speed_meter_board, true);
    lv_scale_section_t * speed_meter_board_section_0 = lv_scale_add_section(ui->speed_meter_board);
    static lv_style_t speed_meter_board_section_0_minor_tick_style;
    static lv_style_t speed_meter_board_section_0_label_style;
    static lv_style_t speed_meter_board_section_0_main_line_style;
    lv_style_init(&speed_meter_board_section_0_label_style);
    lv_style_init(&speed_meter_board_section_0_minor_tick_style);
    lv_style_init(&speed_meter_board_section_0_main_line_style);

    lv_scale_section_set_range(speed_meter_board_section_0, 1, 50);
    lv_scale_section_set_style(speed_meter_board_section_0, LV_PART_INDICATOR, &speed_meter_board_section_0_label_style);
    lv_scale_section_set_style(speed_meter_board_section_0, LV_PART_MAIN, &speed_meter_board_section_0_main_line_style);
    lv_scale_section_set_style(speed_meter_board_section_0, LV_PART_ITEMS, &speed_meter_board_section_0_minor_tick_style);

    lv_style_set_text_color(&speed_meter_board_section_0_label_style, lv_color_hex(0xffffff));
    lv_style_set_line_color(&speed_meter_board_section_0_label_style, lv_color_hex(0xFFFFFF));
    lv_style_set_line_color(&speed_meter_board_section_0_minor_tick_style, lv_color_hex(0xFFFFFF));
    lv_style_set_line_width(&speed_meter_board_section_0_minor_tick_style, 3);
    lv_style_set_line_color(&speed_meter_board_section_0_label_style, lv_color_hex(0xFFFFFF));
    lv_style_set_line_width(&speed_meter_board_section_0_label_style, 3);
    lv_style_set_arc_color(&speed_meter_board_section_0_main_line_style, lv_color_hex(0x00d6c2));
    lv_style_set_arc_width(&speed_meter_board_section_0_main_line_style, 10);
    lv_scale_section_t * speed_meter_board_section_1 = lv_scale_add_section(ui->speed_meter_board);
    static lv_style_t speed_meter_board_section_1_minor_tick_style;
    static lv_style_t speed_meter_board_section_1_label_style;
    static lv_style_t speed_meter_board_section_1_main_line_style;
    lv_style_init(&speed_meter_board_section_1_label_style);
    lv_style_init(&speed_meter_board_section_1_minor_tick_style);
    lv_style_init(&speed_meter_board_section_1_main_line_style);

    lv_scale_section_set_range(speed_meter_board_section_1, 50, 80);
    lv_scale_section_set_style(speed_meter_board_section_1, LV_PART_INDICATOR, &speed_meter_board_section_1_label_style);
    lv_scale_section_set_style(speed_meter_board_section_1, LV_PART_MAIN, &speed_meter_board_section_1_main_line_style);
    lv_scale_section_set_style(speed_meter_board_section_1, LV_PART_ITEMS, &speed_meter_board_section_1_minor_tick_style);

    lv_style_set_text_color(&speed_meter_board_section_1_label_style, lv_color_hex(0xffffff));
    lv_style_set_line_color(&speed_meter_board_section_1_label_style, lv_color_hex(0xffffff));
    lv_style_set_line_color(&speed_meter_board_section_1_minor_tick_style, lv_color_hex(0xffffff));
    lv_style_set_line_width(&speed_meter_board_section_1_minor_tick_style, 3);
    lv_style_set_line_color(&speed_meter_board_section_1_label_style, lv_color_hex(0xffffff));
    lv_style_set_line_width(&speed_meter_board_section_1_label_style, 3);
    lv_style_set_arc_color(&speed_meter_board_section_1_main_line_style, lv_color_hex(0xb4ff00));
    lv_style_set_arc_width(&speed_meter_board_section_1_main_line_style, 10);
    lv_scale_section_t * speed_meter_board_section_2 = lv_scale_add_section(ui->speed_meter_board);
    static lv_style_t speed_meter_board_section_2_minor_tick_style;
    static lv_style_t speed_meter_board_section_2_label_style;
    static lv_style_t speed_meter_board_section_2_main_line_style;
    lv_style_init(&speed_meter_board_section_2_label_style);
    lv_style_init(&speed_meter_board_section_2_minor_tick_style);
    lv_style_init(&speed_meter_board_section_2_main_line_style);

    lv_scale_section_set_range(speed_meter_board_section_2, 80, 120);
    lv_scale_section_set_style(speed_meter_board_section_2, LV_PART_INDICATOR, &speed_meter_board_section_2_label_style);
    lv_scale_section_set_style(speed_meter_board_section_2, LV_PART_MAIN, &speed_meter_board_section_2_main_line_style);
    lv_scale_section_set_style(speed_meter_board_section_2, LV_PART_ITEMS, &speed_meter_board_section_2_minor_tick_style);

    lv_style_set_text_color(&speed_meter_board_section_2_label_style, lv_color_hex(0xffffff));
    lv_style_set_line_color(&speed_meter_board_section_2_label_style, lv_color_hex(0xffffff));
    lv_style_set_line_color(&speed_meter_board_section_2_minor_tick_style, lv_color_hex(0xffffff));
    lv_style_set_line_width(&speed_meter_board_section_2_minor_tick_style, 3);
    lv_style_set_line_color(&speed_meter_board_section_2_label_style, lv_color_hex(0xffffff));
    lv_style_set_line_width(&speed_meter_board_section_2_label_style, 3);
    lv_style_set_arc_color(&speed_meter_board_section_2_main_line_style, lv_color_hex(0xff00ff));
    lv_style_set_arc_width(&speed_meter_board_section_2_main_line_style, 10);

    ui->speed_meter_board_ndline_0 = lv_line_create(ui->speed_meter_board);
    lv_obj_set_style_line_width(ui->speed_meter_board_ndline_0, 5, LV_PART_MAIN);
    lv_obj_set_style_line_color(ui->speed_meter_board_ndline_0, lv_color_hex(0xffffff), LV_PART_MAIN);
    lv_obj_set_style_line_rounded(ui->speed_meter_board_ndline_0, true, LV_PART_MAIN);
    lv_scale_set_line_needle_value(ui->speed_meter_board, ui->speed_meter_board_ndline_0, 103, 5);


    //Write style for speed_meter_board, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->speed_meter_board, 244, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->speed_meter_board, lv_color_hex(0x142c58), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->speed_meter_board, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->speed_meter_board, 345, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->speed_meter_board, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_width(ui->speed_meter_board, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(ui->speed_meter_board, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_color(ui->speed_meter_board, lv_color_hex(0x757575), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_arc_rounded(ui->speed_meter_board, true, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->speed_meter_board, lv_color_hex(0x0D3055), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->speed_meter_board, &lv_font_LiberationSans_10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->speed_meter_board, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for speed_meter_board, Part: LV_PART_ITEMS, State: LV_STATE_DEFAULT.
    lv_obj_set_style_length(ui->speed_meter_board, 15, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_line_width(ui->speed_meter_board, 2, LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_line_color(ui->speed_meter_board, lv_color_hex(0x4A69FF), LV_PART_ITEMS|LV_STATE_DEFAULT);
    lv_obj_set_style_line_opa(ui->speed_meter_board, 255, LV_PART_ITEMS|LV_STATE_DEFAULT);

    //Write style for speed_meter_board, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
    lv_obj_set_style_length(ui->speed_meter_board, 20, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_line_width(ui->speed_meter_board, 4, LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_line_color(ui->speed_meter_board, lv_color_hex(0xFFFFFF), LV_PART_INDICATOR|LV_STATE_DEFAULT);
    lv_obj_set_style_line_opa(ui->speed_meter_board, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);

    //Write codes speed_label_speed_unit
    ui->speed_label_speed_unit = lv_label_create(ui->speed_cont_box);
    lv_obj_set_pos(ui->speed_label_speed_unit, 99, 202);
    lv_obj_set_size(ui->speed_label_speed_unit, 27, 25);
    lv_label_set_text(ui->speed_label_speed_unit, "Km/h");
    lv_label_set_long_mode(ui->speed_label_speed_unit, LV_LABEL_LONG_WRAP);

    //Write style for speed_label_speed_unit, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->speed_label_speed_unit, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->speed_label_speed_unit, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->speed_label_speed_unit, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->speed_label_speed_unit, &lv_font_LiberationSans_10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->speed_label_speed_unit, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->speed_label_speed_unit, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->speed_label_speed_unit, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->speed_label_speed_unit, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->speed_label_speed_unit, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->speed_label_speed_unit, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->speed_label_speed_unit, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->speed_label_speed_unit, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->speed_label_speed_unit, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->speed_label_speed_unit, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes speed_label_digit
    ui->speed_label_digit = lv_label_create(ui->speed_cont_box);
    lv_obj_set_pos(ui->speed_label_digit, 99, 176);
    lv_obj_set_size(ui->speed_label_digit, 27, 25);
    lv_label_set_text(ui->speed_label_digit, "0");
    lv_label_set_long_mode(ui->speed_label_digit, LV_LABEL_LONG_WRAP);

    //Write style for speed_label_digit, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->speed_label_digit, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->speed_label_digit, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->speed_label_digit, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->speed_label_digit, &lv_font_LiberationSans_10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->speed_label_digit, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->speed_label_digit, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->speed_label_digit, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->speed_label_digit, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->speed_label_digit, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->speed_label_digit, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->speed_label_digit, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->speed_label_digit, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->speed_label_digit, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->speed_label_digit, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes speed_label_title
    ui->speed_label_title = lv_label_create(ui->speed_cont_box);
    lv_obj_set_pos(ui->speed_label_title, 94, 161);
    lv_obj_set_size(ui->speed_label_title, 41, 18);
    lv_label_set_text(ui->speed_label_title, "Speed");
    lv_label_set_long_mode(ui->speed_label_title, LV_LABEL_LONG_WRAP);

    //Write style for speed_label_title, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->speed_label_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->speed_label_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->speed_label_title, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->speed_label_title, &lv_font_LiberationSans_10, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->speed_label_title, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->speed_label_title, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->speed_label_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->speed_label_title, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->speed_label_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->speed_label_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->speed_label_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->speed_label_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->speed_label_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->speed_label_title, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of speed.


    //Update current screen layout.
    lv_obj_update_layout(ui->speed);

    //Init events for screen.
    events_init_speed(ui);
}
