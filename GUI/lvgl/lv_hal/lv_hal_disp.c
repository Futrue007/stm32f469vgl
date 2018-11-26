
/**
 * @file hal_disp.c
 *
 * @description HAL layer for display driver
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include <stdint.h>
#include <stddef.h>
#include "../lv_hal/lv_hal_disp.h"
#include "../lv_misc/lv_mem.h"
#include "../lv_core/lv_obj.h"
#include "lv_conf.h"
#include "lvgl/lv_core/lv_vdb.h"
#include "lvgl/lv_hal/lv_hal.h"
#include "lcd.h"
#include "stm32f4xx_hal.h"
/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_disp_t *disp_list = NULL;
static lv_disp_t *active;

/**********************
 *      MACROS
 **********************/
#define TFT_HOR_RES 800
#define TFT_VER_RES 480
/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Initialize a display driver with default values.
 * It is used to surly have known values in the fields ant not memory junk.
 * After it you can set the fields.
 * @param driver pointer to driver variable to initialize
 */
void lv_disp_drv_init(lv_disp_drv_t *driver)
{
    driver->disp_fill = NULL;
    driver->disp_map = NULL;
    driver->disp_flush = NULL;

#if USE_LV_GPU
    driver->mem_blend = NULL;
    driver->mem_fill = NULL;
#endif
}

/**
 * Register an initialized display driver.
 * Automatically set the first display as active.
 * @param driver pointer to an initialized 'lv_disp_drv_t' variable (can be local variable)
 * @return pointer to the new display or NULL on error
 */
lv_disp_t * lv_disp_drv_register(lv_disp_drv_t *driver)
{
    lv_disp_t *node;

    node = lv_mem_alloc(sizeof(lv_disp_t));
    if (!node) return NULL;

    memcpy(&node->driver,driver, sizeof(lv_disp_drv_t));
    node->next = NULL;

    /* Set first display as active by default */
    if (disp_list == NULL) {
        disp_list = node;
        active = node;
        lv_obj_invalidate(lv_scr_act());
    } else {
        node->next = disp_list;
    }

    return node;
}


/**
 * Set the active display
 * @param disp pointer to a display (return value of 'lv_disp_register')
 */
void lv_disp_set_active(lv_disp_t * disp)
{
    active = disp;
    lv_obj_invalidate(lv_scr_act());
}

/**
 * Get a pointer to the active display
 * @return pointer to the active display
 */
lv_disp_t * lv_disp_get_active(void)
{
    return active;
}

/**
 * Get the next display.
 * @param disp pointer to the current display. NULL to initialize.
 * @return the next display or NULL if no more. Give the first display when the parameter is NULL
 */
lv_disp_t * lv_disp_next(lv_disp_t * disp)
{
    if(disp == NULL) {
        return disp_list;
    } else {
        if(disp_list->next == NULL) return NULL;
        else return disp_list->next;
    }
}

/**
 * Write the content of the internal buffer (VDB) to the display
 * @param x1 left coordinate of the rectangle
 * @param x2 right coordinate of the rectangle
 * @param y1 top coordinate of the rectangle
 * @param y2 bottom coordinate of the rectangle
 * @param color_p fill color
 */
void lv_disp_fill(int32_t x1, int32_t y1, int32_t x2, int32_t y2, lv_color_t color)
{
    if(active == NULL) return;
    if(active->driver.disp_fill != NULL) active->driver.disp_fill(x1, y1, x2, y2, color);
}

/**
 * Fill a rectangular area with a color on the active display
 * @param x1 left coordinate of the rectangle
 * @param x2 right coordinate of the rectangle
 * @param y1 top coordinate of the rectangle
 * @param y2 bottom coordinate of the rectangle
 * @param color_p pointer to an array of colors
 */
void lv_disp_flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, lv_color_t *color_p)
{
    if(active == NULL) return;
    if(active->driver.disp_flush != NULL) active->driver.disp_flush(x1, y1, x2, y2, color_p);
}

/**
 * Put a color map to a rectangular area on the active display
 * @param x1 left coordinate of the rectangle
 * @param x2 right coordinate of the rectangle
 * @param y1 top coordinate of the rectangle
 * @param y2 bottom coordinate of the rectangle
 * @param color_map pointer to an array of colors
 */
void lv_disp_map(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t * color_map)
{
    if(active == NULL) return;
    if(active->driver.disp_map != NULL)  active->driver.disp_map(x1, y1, x2, y2, color_map);
}

#if USE_LV_GPU

/**
 * Blend pixels to a destination memory from a source memory
 * In 'lv_disp_drv_t' 'mem_blend' is optional. (NULL if not available)
 * @param dest a memory address. Blend 'src' here.
 * @param src pointer to pixel map. Blend it to 'dest'.
 * @param length number of pixels in 'src'
 * @param opa opacity (0, LV_OPA_TRANSP: transparent ... 255, LV_OPA_COVER, fully cover)
 */
void lv_disp_mem_blend(lv_color_t * dest, const lv_color_t * src, uint32_t length, lv_opa_t opa)
{
    if(active == NULL) return;
    if(active->driver.mem_blend != NULL) active->driver.mem_blend(dest, src, length, opa);
}

/**
 * Fill a memory with a color (GPUs may support it)
 * In 'lv_disp_drv_t' 'mem_fill' is optional. (NULL if not available)
 * @param dest a memory address. Copy 'src' here.
 * @param src pointer to pixel map. Copy it to 'dest'.
 * @param length number of pixels in 'src'
 * @param opa opacity (0, LV_OPA_TRANSP: transparent ... 255, LV_OPA_COVER, fully cover)
 */
void lv_disp_mem_fill(lv_color_t * dest, uint32_t length, lv_color_t color)
{
    if(active == NULL) return;
    if(active->driver.mem_fill != NULL) active->driver.mem_fill(dest, length, color);
}

/**
 * Shows if memory blending (by GPU) is supported or not
 * @return false: 'mem_blend' is not supported in the driver; true: 'mem_blend' is supported in the driver
 */
bool lv_disp_is_mem_blend_supported(void)
{
    if(active->driver.mem_blend) return true;
    else return false;
}

/**
 * Shows if memory fill (by GPU) is supported or not
 * @return false: 'mem_fill' is not supported in the drover; true: 'mem_fill' is supported in the driver
 */
bool lv_disp_is_mem_fill_supported(void)
{
    if(active->driver.mem_fill) return true;
    else return false;
}
#endif

/**********************
 *   STATIC FUNCTIONS
 **********************/
//littlevg �ӿڲ���

/**
 * Fill a rectangular area with a color
 * @param x1 left coordinate of the rectangle
 * @param x2 right coordinate of the rectangle
 * @param y1 top coordinate of the rectangle
 * @param y2 bottom coordinate of the rectangle
 * @param color fill color
 */
void tft_fill(int32_t x1, int32_t y1, int32_t x2, int32_t y2, lv_color_t color)
{
    /*Return if the area is out the screen*/
    if(x2 < 0) return;

    if(y2 < 0) return;

    if(x1 > TFT_HOR_RES - 1) return;

    if(y1 > TFT_VER_RES - 1) return;

    /*Truncate the area to the screen*/
    int32_t act_x1 = x1 < 0 ? 0 : x1;
    int32_t act_y1 = y1 < 0 ? 0 : y1;
    int32_t act_x2 = x2 > TFT_HOR_RES - 1 ? TFT_HOR_RES - 1 : x2;
    int32_t act_y2 = y2 > TFT_VER_RES - 1 ? TFT_VER_RES - 1 : y2;

    BSP_LCD_FillRect_color(act_x1, act_y1, act_x2 - act_x1 + 1, act_y2 - act_y1 + 1, ((color.full) | 0xFF000000));
}

/**
 * Put a color map to a rectangular area
 * @param x1 left coordinate of the rectangle
 * @param x2 right coordinate of the rectangle
 * @param y1 top coordinate of the rectangle
 * @param y2 bottom coordinate of the rectangle
 * @param color_p pointer to an array of colors
 */
void tft_map(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t * color_p)
{
    /*Return if the area is out the screen*/
    if(x2 < 0) return;

    if(y2 < 0) return;

    if(x1 > TFT_HOR_RES - 1) return;

    if(y1 > TFT_VER_RES - 1) return;

    /*Truncate the area to the screen*/
    int32_t act_x1 = x1 < 0 ? 0 : x1;
    int32_t act_y1 = y1 < 0 ? 0 : y1;
    int32_t act_x2 = x2 > TFT_HOR_RES - 1 ? TFT_HOR_RES - 1 : x2;
    int32_t act_y2 = y2 > TFT_VER_RES - 1 ? TFT_VER_RES - 1 : y2;


    uint32_t x;
    uint32_t y;

    /*Put the map to the remaining area*/
    for(y = act_y1; y <= act_y2; y++)
    {
        for(x = act_x1; x <= act_x2; x++)
        {
            /*Your specific function comes here!*/
            /*my_put_pixel(x, y, color_p->full);*/
            BSP_LCD_DrawPixel(x, y,  ((color_p->full) | 0xFF000000));
            color_p++;
        }

        color_p += x2 - act_x2;	/*Skip the parts out of the screen*/
    }
    lv_flush_ready();
}

/**
 * Flush a color buffer
 * @param x1 left coordinate of the rectangle
 * @param x2 right coordinate of the rectangle
 * @param y1 top coordinate of the rectangle
 * @param y2 bottom coordinate of the rectangle
 * @param color_p pointer to an array of colors
 */
static void tft_flush(int32_t x1, int32_t y1, int32_t x2, int32_t y2, const lv_color_t * color_p)
{
    /*Return if the area is out the screen*/
    if(x2 < 0) return;

    if(y2 < 0) return;

    if(x1 > TFT_HOR_RES - 1) return;

    if(y1 > TFT_VER_RES - 1) return;

    /*Truncate the area to the screen*/
    int32_t act_x1 = x1 < 0 ? 0 : x1;
    int32_t act_y1 = y1 < 0 ? 0 : y1;
    int32_t act_x2 = x2 > TFT_HOR_RES - 1 ? TFT_HOR_RES - 1 : x2;
    int32_t act_y2 = y2 > TFT_VER_RES - 1 ? TFT_VER_RES - 1 : y2;

    uint32_t x;
    uint32_t y;

    /*Put the map to the remaining area*/
    for(y = act_y1; y <= act_y2; y++)
    {
        for(x = act_x1; x <= act_x2; x++)
        {
            /*Your specific function comes here!*/
            /*my_put_pixel(x, y, color_p->full);*/
            BSP_LCD_DrawPixel(x, y,  ((color_p->full) | 0xFF000000));
            color_p++;
        }

        color_p += x2 - act_x2;	/*Skip the parts out of the screen*/
    }
    lv_flush_ready();
}

void tft_init(void)
{
	lv_disp_drv_t disp_drv;
	lv_disp_drv_init(&disp_drv);



	disp_drv.disp_fill = tft_fill;
	disp_drv.disp_map = tft_map;
	disp_drv.disp_flush = tft_flush;

	lv_disp_drv_register(&disp_drv);
}

