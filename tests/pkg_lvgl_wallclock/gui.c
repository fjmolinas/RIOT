
/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       LittlevGL example application
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <string.h>

#include <time.h>
#include "timex.h"

#include "lvgl/lvgl.h"
#include "lvgl_riot.h"
#include "disp_dev.h"

#include "controller.h"

/* Must be lower than LVGL_INACTIVITY_PERIOD_MS for autorefresh */
#define REFR_TIME           MS_PER_SEC

static lv_obj_t *time_label;
static lv_timer_t *refr_task;
static lv_obj_t *root_page;

extern void get_time(struct tm *time);

enum {
    LV_MENU_ITEM_BUILDER_VARIANT_1,
    LV_MENU_ITEM_BUILDER_VARIANT_2
};
typedef uint8_t lv_menu_builder_variant_t;

static lv_obj_t *create_text(lv_obj_t *parent, const char *icon, const char *txt,
                             lv_menu_builder_variant_t builder_variant)
{
    lv_obj_t *obj = lv_menu_cont_create(parent);

    lv_obj_t *img = NULL;
    lv_obj_t *label = NULL;

    if (icon) {
        img = lv_img_create(obj);
        lv_img_set_src(img, icon);
    }

    if (txt) {
        label = lv_label_create(obj);
        lv_label_set_text(label, txt);
        lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
        lv_obj_set_flex_grow(label, 1);
    }

    if (builder_variant == LV_MENU_ITEM_BUILDER_VARIANT_2 && icon && txt) {
        lv_obj_add_flag(img, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
        lv_obj_swap(img, label);
    }

    return obj;
}

static lv_obj_t *create_slider(lv_obj_t *parent, const char *icon, const char *txt, int32_t min,
                               int32_t max, int32_t val, lv_event_cb_t cb, void *arg)
{
    lv_obj_t *obj = create_text(parent, icon, txt, LV_MENU_ITEM_BUILDER_VARIANT_2);

    lv_obj_t *slider = lv_slider_create(obj);

    lv_obj_set_flex_grow(slider, 1);
    lv_slider_set_range(slider, min, max);
    lv_slider_set_value(slider, val, LV_ANIM_OFF);

    if (icon == NULL) {
        lv_obj_add_flag(slider, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
    }

    if (cb) {
        lv_obj_add_event_cb(slider, cb, LV_EVENT_VALUE_CHANGED, arg);
    }

    return obj;
}

static lv_obj_t *create_switch(lv_obj_t *parent, const char *icon, const char *txt, bool chk)
{
    lv_obj_t *obj = create_text(parent, icon, txt, LV_MENU_ITEM_BUILDER_VARIANT_1);

    lv_obj_t *sw = lv_switch_create(obj);

    lv_obj_add_state(sw, chk ? LV_STATE_CHECKED : 0);

    return obj;
}

static void switch_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *menu = lv_event_get_user_data(e);
    lv_obj_t *obj = lv_event_get_target(e);

    if (code == LV_EVENT_VALUE_CHANGED) {
        if (lv_obj_has_state(obj, LV_STATE_CHECKED)) {
            lv_menu_set_page(menu, NULL);
            lv_menu_set_sidebar_page(menu, root_page);
            lv_event_send(lv_obj_get_child(lv_obj_get_child(lv_menu_get_cur_sidebar_page(menu), 0),
                                           0), LV_EVENT_CLICKED, NULL);
        }
        else {
            lv_menu_set_sidebar_page(menu, NULL);
            lv_menu_clear_history(menu); /* Clear history because we will be showing the root page later */
            lv_menu_set_page(menu, root_page);
        }
    }
}

static lv_obj_t *cont_ble_time_adv;
static lv_obj_t *cont_ble_auto_conn;
static lv_obj_t *cont_ble_stdio;
static lv_obj_t *cont_slider;

static void state_change_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *cont = lv_event_get_user_data(e);
    lv_obj_t *obj = lv_event_get_target(e);

    if (code == LV_EVENT_VALUE_CHANGED) {
        controller_t *controller = controller_adquire();
        if (cont == cont_ble_auto_conn) {
            if (lv_obj_has_state(obj, LV_STATE_CHECKED)) {
                controller->status |= CONTROLLER_STATUS_BLE_AUTO_CONN;
            }
            else {

                controller->status &= ~CONTROLLER_STATUS_BLE_AUTO_CONN;
            }
        }
        else if (cont == cont_ble_time_adv) {
            if (lv_obj_has_state(obj, LV_STATE_CHECKED)) {
                controller->status |= CONTROLLER_STATUS_BLE_TIME_ADV;
            }
            else {

                controller->status &= ~CONTROLLER_STATUS_BLE_TIME_ADV;
            }
        }
        else if (cont == cont_ble_stdio) {
            if (lv_obj_has_state(obj, LV_STATE_CHECKED)) {
                controller->status |= CONTROLLER_STATUS_BLE_STDIO;
            }
            else {

                controller->status &= ~CONTROLLER_STATUS_BLE_STDIO;
            }
        }
        else if (cont == cont_slider) {
            controller->brightness = lv_slider_get_value(obj);
            controller->status |= CONTROLLER_STATUS_BRIGHTNESS;
        }
        controller_release();
        controller_status_changed();
    }
}

static void create_settings(lv_obj_t *parent)
{
    lv_obj_t *menu = lv_menu_create(parent);

    lv_menu_set_mode_root_back_btn(menu, LV_MENU_ROOT_BACK_BTN_ENABLED);
    /* TODO: parametrize this */
    lv_obj_set_size(menu, lv_disp_get_hor_res(NULL) - 30, lv_disp_get_ver_res(NULL) - 40);
    lv_obj_align(menu, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *cont;
    lv_obj_t *section;

    /*Create sub pages*/
    lv_obj_t *sub_bluetooth_page = lv_menu_page_create(menu, NULL);

    lv_obj_set_style_pad_hor(sub_bluetooth_page,
                             lv_obj_get_style_pad_left(lv_menu_get_main_header(menu), 0), 0);
    lv_menu_separator_create(sub_bluetooth_page);
    section = lv_menu_section_create(sub_bluetooth_page);
    cont_ble_time_adv = create_switch(section, LV_SYMBOL_BLUETOOTH, "Time Advertiser", false);
    lv_obj_add_event_cb(lv_obj_get_child(cont_ble_time_adv,
                                         2), state_change_handler, LV_EVENT_VALUE_CHANGED,
                        cont_ble_time_adv);
    cont_ble_auto_conn = create_switch(section, LV_SYMBOL_BLUETOOTH, "IPSP auto-conn", false);
    lv_obj_add_event_cb(lv_obj_get_child(cont_ble_auto_conn,
                                         2), state_change_handler, LV_EVENT_VALUE_CHANGED,
                        cont_ble_auto_conn);
    cont_ble_stdio = create_switch(section, LV_SYMBOL_BLUETOOTH, "stdio", false);
    lv_obj_add_event_cb(lv_obj_get_child(cont_ble_stdio,
                                         2), state_change_handler, LV_EVENT_VALUE_CHANGED,
                        cont_ble_stdio);

    lv_obj_t *sub_display_page = lv_menu_page_create(menu, NULL);

    lv_obj_set_style_pad_hor(sub_display_page,
                             lv_obj_get_style_pad_left(lv_menu_get_main_header(menu), 0), 0);
    lv_menu_separator_create(sub_display_page);
    section = lv_menu_section_create(sub_display_page);
    cont_slider = create_slider(section, LV_SYMBOL_SETTINGS, "Brightness", 0, 255, 255, NULL, NULL);
    lv_obj_add_event_cb(lv_obj_get_child(cont_slider,
                                         2), state_change_handler, LV_EVENT_VALUE_CHANGED,
                        cont_slider);

    lv_obj_t *sub_menu_mode_page = lv_menu_page_create(menu, NULL);

    lv_obj_set_style_pad_hor(sub_menu_mode_page,
                             lv_obj_get_style_pad_left(lv_menu_get_main_header(menu), 0), 0);
    lv_menu_separator_create(sub_menu_mode_page);
    section = lv_menu_section_create(sub_menu_mode_page);
    cont = create_switch(section, LV_SYMBOL_LOOP, "Sidebar enable", true);
    lv_obj_add_event_cb(lv_obj_get_child(cont, 2), switch_handler, LV_EVENT_VALUE_CHANGED, menu);

    /*Create a root page*/
    root_page = lv_menu_page_create(menu, "Settings");
    lv_obj_set_style_pad_hor(root_page, lv_obj_get_style_pad_left(lv_menu_get_main_header(
                                                                      menu), 0), 0);
    section = lv_menu_section_create(root_page);
    cont = create_text(section, LV_SYMBOL_BLUETOOTH, "Ble", LV_MENU_ITEM_BUILDER_VARIANT_1);
    lv_menu_set_load_page_event(menu, cont, sub_bluetooth_page);
    cont = create_text(section, LV_SYMBOL_SETTINGS, "Disp.", LV_MENU_ITEM_BUILDER_VARIANT_1);
    lv_menu_set_load_page_event(menu, cont, sub_display_page);

    create_text(root_page, NULL, "Others", LV_MENU_ITEM_BUILDER_VARIANT_1);
    section = lv_menu_section_create(root_page);
    cont = create_text(section, LV_SYMBOL_SETTINGS, "Mode", LV_MENU_ITEM_BUILDER_VARIANT_1);
    lv_menu_set_load_page_event(menu, cont, sub_menu_mode_page);

    lv_menu_set_sidebar_page(menu, root_page);

    lv_event_send(lv_obj_get_child(lv_obj_get_child(lv_menu_get_cur_sidebar_page(
                                                        menu), 0), 0), LV_EVENT_CLICKED, NULL);
}


static void wallclock_task(lv_timer_t *param)
{
    (void)param;

    struct tm time;

    get_time(&time);
    /* Set the text info */
    (void)time;
    lv_label_set_text_fmt(time_label, "%02d:%02d:%02d", time.tm_hour, time.tm_min, time.tm_sec);
}

void create_wallclock(lv_obj_t *parent)
{
    static lv_style_t style;

    lv_style_init(&style);
    lv_style_set_bg_opa(&style, LV_OPA_COVER);
    lv_style_set_bg_color(&style, lv_color_black());

    /* Create a label for the details of Memory and CPU usage */
    time_label = lv_label_create(parent);
    lv_obj_set_style_text_font(time_label, &lv_font_montserrat_32, 0);
    lv_obj_set_style_text_align(time_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(time_label, LV_ALIGN_CENTER, 0, 0);

    /* Create the task used to refresh the chart and label */
    refr_task = lv_timer_create(wallclock_task, REFR_TIME, NULL);
}

void mainface_create(void)
{
    lv_obj_t *tv = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, 40);

    lv_obj_t *tab0 = lv_tabview_add_tab(tv, "clock");
    lv_obj_t *tab1 = lv_tabview_add_tab(tv, "system");

    (void)tab1;

    create_wallclock(tab0);
    create_settings(tab1);
}

void *lvgl_thread(void *arg)
{
    (void)arg;
    /* Enable backlight */
    disp_dev_backlight_on();
    /* Create the wall clock widget */
    mainface_create();
    /* run lvgl */
    lvgl_run();

    return NULL;
}
