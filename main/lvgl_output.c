#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "driver/gpio.h"
#include "esp_freertos_hooks.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "lvgl.h"
#include "lvgl_helpers.h"

/*********************
 *      DEFINES
 *********************/
#define TAG "demo"
#define LV_TICK_PERIOD_MS 1

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_tick_task(void *arg);
static void guiTask(void *pvParameter);
static void set_label_text(void);
static lv_obj_t *scr;
static lv_obj_t *label1;

static char *output = " ";

/**********************
 *   APPLICATION MAIN
 **********************/

SemaphoreHandle_t xGuiSemaphore;

static void guiTask(void *pvParameter) {
  (void)pvParameter;
  xGuiSemaphore = xSemaphoreCreateMutex();

  lv_init();

  /* Initialize SPI or I2C bus used by the drivers */
  lvgl_driver_init();

  lv_color_t *buf1 =
      heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
  assert(buf1 != NULL);

  lv_color_t *buf2 =
      heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
  assert(buf2 != NULL);

  static lv_disp_buf_t disp_buf;

  uint32_t size_in_px = DISP_BUF_SIZE;

  lv_disp_buf_init(&disp_buf, buf1, buf2, size_in_px);

  lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.flush_cb = disp_driver_flush;

  disp_drv.buffer = &disp_buf;
  lv_disp_drv_register(&disp_drv);

  /* Create and start a periodic timer interrupt to call lv_tick_inc */
  const esp_timer_create_args_t periodic_timer_args = {
      .callback = &lv_tick_task, .name = "periodic_gui"};
  esp_timer_handle_t periodic_timer;
  ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
  ESP_ERROR_CHECK(
      esp_timer_start_periodic(periodic_timer, LV_TICK_PERIOD_MS * 1000));

  scr = lv_disp_get_scr_act(NULL);

  /*Create a Label on the currently active screen*/
  label1 = lv_label_create(scr, NULL);
  lv_obj_set_size(label1, LV_HOR_RES, LV_VER_RES);
  lv_label_set_text(label1, "Loading...");
  lv_label_set_long_mode(label1, LV_LABEL_LONG_BREAK); /*Break the long lines*/
  lv_obj_set_width(label1, 240);
  lv_obj_set_height(label1, 320);
  lv_task_create(set_label_text, 100, LV_TASK_PRIO_LOW, NULL);

  while (1) {
    /* Delay 1 tick (assumes FreeRTOS tick is 10ms */
    vTaskDelay(pdMS_TO_TICKS(10));

    /* Try to take the semaphore, call lvgl related function on success */
    if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY)) {
      lv_task_handler();
      xSemaphoreGive(xGuiSemaphore);
    }
  }

  free(buf1);
  free(buf2);
  vTaskDelete(NULL);
}

static void set_label_text(void) {
  char buf[100];
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  strftime(buf, sizeof(buf) - 1, "%X", t);
  lv_label_set_text(label1, buf);
  lv_label_ins_text(label1, LV_LABEL_POS_LAST, output);

}

static void lv_tick_task(void *arg) {
  (void)arg;
  lv_tick_inc(LV_TICK_PERIOD_MS);
}
