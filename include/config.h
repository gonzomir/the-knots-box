#include <Arduino.h>

#define GNSS_RX 1
#define GNSS_TX 2
#define GNSS_PPS 3
#define GNSS_EN 17
#define GNSS_EN_GPIO GPIO_NUM_17

#define MAIN_BTN 0
#define MAIN_BTN_GPIO GPIO_NUM_0

#define MODE_BTN 18
#define NODE_BTN_GPIO GPIO_NUM_18

#define BATTERY_STATUS 4
#define BATTERY_STATUS_ADC_CHANNEL ADC1_CHANNEL_3
#define BATTERY_STATUS_ATTENUATION ADC_ATTEN_DB_12

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
