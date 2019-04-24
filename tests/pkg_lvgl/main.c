/*
 * Copyright (C) 2018 Koen Zandberg <koen@bergzand.net>
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
 * @brief       Test application for the ili9431 tft display
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include <stdio.h>
#include "byteorder.h"
#include "xtimer.h"

#ifdef MODULE_ST7735
#include "st7735.h"
#include "st7735_params.h"
#elif defined(MODULE_ILI9341)
#include "ili9341.h"
#include "ili9341_params.h"
#endif

#include "lv_conf.h"
#include "lv_port_indev.h"

#ifdef MODULE_ST7735
static st7735_t dev;
#elif defined(MODULE_ILI9341)
static ili9341_t dev;
#endif

/* Flush the content of the internal buffer the specific area on the display
 * You can use DMA or any hardware acceleration to do this operation in the background but
 * 'lv_flush_ready()' has to be called when finished
 * This function is required only when LV_VDB_SIZE != 0 in lv_conf.h*/
static void ex_disp_flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t * color_p)
{
    /*The most simple case (but also the slowest) to put all pixels to the screen one-by-one*/
#ifdef MODULE_ST7735
    st7735_map(&dev, (uint16_t) x1, (uint16_t) x2, (uint16_t) y1, (uint16_t) y2, (const uint16_t *) color_p);
#elif defined(MODULE_ILI9341)
    ili9341_map(&dev, (uint16_t) x1, (uint16_t) x2, (uint16_t) y1, (uint16_t) y2, (const uint16_t *) color_p);
#endif

    /* IMPORTANT!!!
     * Inform the graphics library that you are ready with the flushing*/
    lv_flush_ready();
}


/* Write a pixel array (called 'map') to the a specific area on the display
 * This function is required only when LV_VDB_SIZE == 0 in lv_conf.h*/
static void ex_disp_map(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t * color_p)
{
    /*The most simple case (but also the slowest) to put all pixels to the screen one-by-one*/
    /*The most simple case (but also the slowest) to put all pixels to the screen one-by-one*/
#ifdef MODULE_ST7735
    st7735_map(&dev, (uint16_t) x1, (uint16_t) x2, (uint16_t) y1, (uint16_t) y2, (const uint16_t *) color_p);
#elif defined(MODULE_ILI9341)
    ili9341_map(&dev, (uint16_t) x1, (uint16_t) x2, (uint16_t) y1, (uint16_t) y2, (const uint16_t *) color_p);
#endif
}


/* Write a pixel array (called 'map') to the a specific area on the display
 * This function is required only when LV_VDB_SIZE == 0 in lv_conf.h*/
static void ex_disp_fill(int32_t x1, int32_t y1, int32_t x2, int32_t y2,  lv_color_t color)
{
    /*The most simple case (but also the slowest) to put all pixels to the screen one-by-one*/
#ifdef MODULE_ST7735
    st7735_fill(&dev, (uint16_t) x1, (uint16_t) x2, (uint16_t) y1, (uint16_t) y2, (uint16_t) color.full);
#elif defined(MODULE_ILI9341)
    ili9341_fill(&dev, (uint16_t) x1, (uint16_t) x2, (uint16_t) y1, (uint16_t) y2, (uint16_t) color.full);
#endif
}




lv_style_t btn3_style;


/**
 * Create a simple 'Hello world!' label
 */
void lv_tutorial_hello_world(void)
{
    /*Create a Label on the currently active screen*/
    lv_obj_t * label1 =  lv_label_create(lv_scr_act(), NULL);

    /*Modify the Label's text*/
    lv_label_set_text(label1, "Hello world!");

    /* Align the Label to the center
     * NULL means align on parent (which is the screen now)
     * 0, 0 at the end means an x, y offset after alignment*/
    lv_obj_align(label1, NULL, LV_ALIGN_CENTER, 0, 0);
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Called when a button is released
 * @param btn pointer to the released button
 * @return LV_RES_OK because the object is not deleted in this function
 */
static  lv_res_t btn_rel_action(lv_obj_t * btn)
{
    /*Increase the button width*/
    lv_coord_t width = lv_obj_get_width(btn);
    lv_obj_set_width(btn, width + 20);

    return LV_RES_OK;
}

/**
 * Called when a new option is chosen in the drop down list
 * @param ddlist pointer to the drop down list
 * @return LV_RES_OK because the object is not deleted in this function
 */
static  lv_res_t ddlist_action(lv_obj_t * ddlist)
{
    uint16_t opt = lv_ddlist_get_selected(ddlist);      /*Get the id of selected option*/

    lv_obj_t * slider = lv_obj_get_free_ptr(ddlist);      /*Get the saved slider*/
    lv_slider_set_value(slider, (opt * 100) / 4);       /*Modify the slider value according to the selection*/

    return LV_RES_OK;
}

/**
 * Create some objects
 */
void lv_tutorial_objects(void)
{

    /********************
     * CREATE A SCREEN
     *******************/
    /* Create a new screen and load it
     * Screen can be created from any type object type
     * Now a Page is used which is an objects with scrollable content*/
    lv_obj_t * scr = lv_page_create(NULL, NULL);
    lv_scr_load(scr);

    /****************
     * ADD A TITLE
     ****************/
    lv_obj_t * label = lv_label_create(scr, NULL); /*First parameters (scr) is the parent*/
    lv_label_set_text(label, "Object usage demo");  /*Set the text*/
    lv_obj_set_x(label, 50);                        /*Set the x coordinate*/

    /***********************
     * CREATE TWO BUTTONS
     ***********************/
    /*Create a button*/
    lv_obj_t * btn1 = lv_btn_create(lv_scr_act(), NULL);         /*Create a button on the currently loaded screen*/
    lv_btn_set_action(btn1, LV_BTN_ACTION_CLICK, btn_rel_action); /*Set function to be called when the button is released*/
    lv_obj_align(btn1, label, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);  /*Align below the label*/

    /*Create a label on the button (the 'label' variable can be reused)*/
    label = lv_label_create(btn1, NULL);
    lv_label_set_text(label, "Button 1");

    /*Copy the previous button*/
    lv_obj_t * btn2 = lv_btn_create(lv_scr_act(), btn1);        /*Second parameter is an object to copy*/
    lv_obj_align(btn2, btn1, LV_ALIGN_OUT_RIGHT_MID, 50, 0);    /*Align next to the prev. button.*/

    /*Create a label on the button*/
    label = lv_label_create(btn2, NULL);
    lv_label_set_text(label, "Button 2");

    /****************
     * ADD A SLIDER
     ****************/
    lv_obj_t * slider = lv_slider_create(scr, NULL);                            /*Create a slider*/
    lv_obj_set_size(slider, lv_obj_get_width(lv_scr_act())  / 3, LV_DPI / 3);   /*Set the size*/
    lv_obj_align(slider, btn1, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);                /*Align below the first button*/
    lv_slider_set_value(slider, 30);                                            /*Set the current value*/

    /***********************
     * ADD A DROP DOWN LIST
     ************************/
    lv_obj_t * ddlist = lv_ddlist_create(lv_scr_act(), NULL);            /*Create a drop down list*/
    lv_obj_align(ddlist, slider, LV_ALIGN_OUT_RIGHT_TOP, 50, 0);         /*Align next to the slider*/
    lv_obj_set_free_ptr(ddlist, slider);                                   /*Save the pointer of the slider in the ddlist (used in 'ddlist_action()')*/
    lv_obj_set_top(ddlist, true);                                        /*Enable to be on the top when clicked*/
    lv_ddlist_set_options(ddlist, "None\nLittle\nHalf\nA lot\nAll"); /*Set the options*/
    lv_ddlist_set_action(ddlist, ddlist_action);                         /*Set function to call on new option is chosen*/

    /****************
     * CREATE A CHART
     ****************/
    lv_obj_t * chart = lv_chart_create(lv_scr_act(), NULL);                         /*Create the chart*/
    lv_obj_set_size(chart, lv_obj_get_width(scr) / 2, lv_obj_get_width(scr) / 4);   /*Set the size*/
    lv_obj_align(chart, slider, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 50);                   /*Align below the slider*/
    lv_chart_set_series_width(chart, 3);                                            /*Set the line width*/

    /*Add a RED data series and set some points*/
    lv_chart_series_t * dl1 = lv_chart_add_series(chart, LV_COLOR_RED);
    lv_chart_set_next(chart, dl1, 10);
    lv_chart_set_next(chart, dl1, 25);
    lv_chart_set_next(chart, dl1, 45);
    lv_chart_set_next(chart, dl1, 80);

    /*Add a BLUE data series and set some points*/
    lv_chart_series_t * dl2 = lv_chart_add_series(chart, LV_COLOR_MAKE(0x40, 0x70, 0xC0));
    lv_chart_set_next(chart, dl2, 10);
    lv_chart_set_next(chart, dl2, 25);
    lv_chart_set_next(chart, dl2, 45);
    lv_chart_set_next(chart, dl2, 80);
    lv_chart_set_next(chart, dl2, 75);
    lv_chart_set_next(chart, dl2, 505);
}

/**
 * Crate some objects an animate them
 */
void lv_tutorial_animations(void)
{
    lv_obj_t * label;


    /*Create a button the demonstrate built-in animations*/
    lv_obj_t * btn1;
    btn1 = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_set_pos(btn1, 10, 10);       /*Set a position. It will be the animation's destination*/
    lv_obj_set_size(btn1, 80, 50);

    label = lv_label_create(btn1, NULL);
    lv_label_set_text(label, "Float");

    /* Float in the button using a built-in function
     * Delay the animation with 2000 ms and float in 300 ms. NULL means no end callback*/
    lv_obj_animate(btn1, LV_ANIM_FLOAT_TOP | LV_ANIM_IN, 300, 2000, NULL);

    /*Create a button to demonstrate user defined animations*/
    lv_obj_t * btn2;
    btn2 = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_set_pos(btn2, 10, 80);       /*Set a position. It will be the animation's destination*/
    lv_obj_set_size(btn2, 80, 50);

    label = lv_label_create(btn2, NULL);
    lv_label_set_text(label, "Move");

    /*Create an animation to move the button continuously left to right*/
    lv_anim_t a;
    a.var = btn2;
    a.start = lv_obj_get_x(btn2);
    a.end = a.start + (100);
    a.fp = (lv_anim_fp_t)lv_obj_set_x;
    a.path = lv_anim_path_linear;
    a.end_cb = NULL;
    a.act_time = -1000;                         /*Negative number to set a delay*/
    a.time = 400;                               /*Animate in 400 ms*/
    a.playback = 1;                             /*Make the animation backward too when it's ready*/
    a.playback_pause = 0;                       /*Wait before playback*/
    a.repeat = 1;                               /*Repeat the animation*/
    a.repeat_pause = 500;                       /*Wait before repeat*/
    lv_anim_create(&a);

    /*Create a button to demonstrate the style animations*/
    lv_obj_t * btn3;
    btn3 = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_set_pos(btn3, 10, 150);       /*Set a position. It will be the animation's destination*/
    lv_obj_set_size(btn3, 80, 50);

    label = lv_label_create(btn3, NULL);
    lv_label_set_text(label, "Style");

    /*Create a unique style for the button*/
    lv_style_copy(&btn3_style, lv_btn_get_style(btn3, LV_BTN_STYLE_REL));
    lv_btn_set_style(btn3, LV_BTN_STATE_REL, &btn3_style);

    /*Animate the new style*/
    lv_style_anim_t sa;
    sa.style_anim = &btn3_style;            /*This style will be animated*/
    sa.style_start = &lv_style_btn_rel;     /*Style in the beginning (can be 'style_anim' as well)*/
    sa.style_end = &lv_style_pretty;        /*Style at the and (can be 'style_anim' as well)*/
    sa.act_time = -500;                     /*These parameters are the same as with the normal animation*/
    sa.time = 1000;
    sa.playback = 1;
    sa.playback_pause = 500;
    sa.repeat = 1;
    sa.repeat_pause = 500;
    sa.end_cb = NULL;
    lv_style_anim_create(&sa);
}



/**
 * Initialize a theme and create the same objects like "lv_tutorial_objects'
 */
void lv_tutorial_themes(void)
{
    /*Initialize the alien theme
     * 210: a green HUE value
     * NULL: use the default font (LV_FONT_DEFAULT)*/
    lv_theme_t * th = lv_theme_alien_init(210, NULL);

    /*Set the surent system theme*/
    lv_theme_set_current(th);

    /*Create the 'lv_tutorial_objects'*/
    lv_tutorial_objects();

}

int main(void)
{
#ifdef MODULE_ST7735
    if (st7735_init(&dev, &st7735_params[0]) == 0) {
#else
    if (ili9341_init(&dev, &ili9341_params[0]) == 0) {
#endif
        puts("[OK]");
    }
    else {
        puts("[Failed]");
        return 1;

    }


    lv_init();

    lv_disp_drv_t disp_drv;                         /*Descriptor of a display driver*/
    lv_disp_drv_init(&disp_drv);                    /*Basic initialization*/

    /*Set up the functions to access to your display*/
    disp_drv.disp_flush = ex_disp_flush;            /*Used in buffered mode (LV_VDB_SIZE != 0  in lv_conf.h)*/

    disp_drv.disp_fill = ex_disp_fill;              /*Used in unbuffered mode (LV_VDB_SIZE == 0  in lv_conf.h)*/
    disp_drv.disp_map = ex_disp_map;                /*Used in unbuffered mode (LV_VDB_SIZE == 0  in lv_conf.h)*/

    /*Finally register the driver*/
    lv_disp_drv_register(&disp_drv);

    // lv_tutorial_hello_world();

    // lv_tutorial_themes();

    lv_tutorial_animations();

    while (1) {
        lv_task_handler();
        xtimer_usleep(5*1000);
    }

    return 0;
}
