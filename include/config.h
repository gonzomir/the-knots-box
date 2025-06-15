#include <Arduino.h>

#define GNSS_RX 17
#define GNSS_TX 18
#define GNSS_PPS 16
#define GNSS_EN 15
#define GNSS_EN_GPIO GPIO_NUM_15

#define MAIN_BTN 6
#define MAIN_BTN_GPIO GPIO_NUM_6

#define MODE_BTN 7
#define NODE_BTN_GPIO GPIO_NUM_7

#define BATTERY_STATUS 5
#define BATTERY_STATUS_ADC_CHANNEL ADC1_CHANNEL_4

#define DISPLAY_DIN -1
#define DISPLAY_CLK -1
#define DISPLAY_CS -1
#define DISPLAY_DC -1
#define DISPLAY_RST -1
#define DISPLAY_BUSY -1

#define GFX_BL 1

#define LCD_QSPI_HOST           (SPI2_HOST)
#define PIN_NUM_QSPI_CS 45
#define PIN_NUM_QSPI_PCLK 47
#define PIN_NUM_QSPI_DATA0 21
#define PIN_NUM_QSPI_DATA1 48
#define PIN_NUM_QSPI_DATA2 40
#define PIN_NUM_QSPI_DATA3 39
#define PIN_NUM_QSPI_RST        (GPIO_NUM_NC)
#define PIN_NUM_QSPI_DC         (GPIO_NUM_8)
#define PIN_NUM_QSPI_TE         (GPIO_NUM_38)
#define PIN_NUM_QSPI_BL         (GPIO_NUM_1)

#define PIN_NUM_QSPI_TOUCH_SCL  (GPIO_NUM_8)
#define PIN_NUM_QSPI_TOUCH_SDA  (GPIO_NUM_4)
#define PIN_NUM_QSPI_TOUCH_RST  (-1)
#define PIN_NUM_QSPI_TOUCH_INT  (-1)

#ifndef CONFIG_H
#define CONFIG_H

enum tkb_mode { speed, start };

extern bool should_sleep;
extern bool gps_is_ready;
extern bool do_read_gnss;
extern bool do_start_timer;
extern tkb_mode display_mode;
extern bool start_timer_started;

#endif
