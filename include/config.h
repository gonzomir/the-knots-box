#include <Arduino.h>

#ifdef EINK
#define GNSS_RX 16
#define GNSS_TX 17
#define GNSS_PPS 18
#define GNSS_EN 19
#define GNSS_EN_GPIO GPIO_NUM_19

#define POWER_BTN 33
#define POWER_BTN_GPIO GPIO_NUM_33
#define BATTERY_STATUS 35
#define BATTERY_STATUS_ADC_CHANNEL ADC1_CHANNEL_7

#define DISPLAY_DIN 14
#define DISPLAY_CLK 13
#define DISPLAY_CS 15
#define DISPLAY_DC 27
#define DISPLAY_RST 26
#define DISPLAY_BUSY 25

#define LCD_QSPI_HOST           (SPI2_HOST)
#define PIN_NUM_QSPI_CS         (-1)
#define PIN_NUM_QSPI_PCLK       (-1)
#define PIN_NUM_QSPI_DATA0      (-1)
#define PIN_NUM_QSPI_DATA1      (-1)
#define PIN_NUM_QSPI_DATA2      (-1)
#define PIN_NUM_QSPI_DATA3      (-1)
#define PIN_NUM_QSPI_RST        (-1)
#define PIN_NUM_QSPI_DC         (-1)
#define PIN_NUM_QSPI_TE         (-1)
#define PIN_NUM_QSPI_BL         (-1)

#define PIN_NUM_QSPI_TOUCH_SCL  (-1)
#define PIN_NUM_QSPI_TOUCH_SDA  (-1)
#define PIN_NUM_QSPI_TOUCH_RST  (-1)
#define PIN_NUM_QSPI_TOUCH_INT  (-1)
#endif

#ifdef TFT
#define GNSS_RX 17
#define GNSS_TX 18
#define GNSS_PPS 6
#define GNSS_EN 7
#define GNSS_EN_GPIO GPIO_NUM_7

#define POWER_BTN 15
#define POWER_BTN_GPIO GPIO_NUM_15
#define BATTERY_STATUS 5
#define BATTERY_STATUS_ADC_CHANNEL ADC1_CHANNEL_4

#define DISPLAY_DIN -1
#define DISPLAY_CLK -1
#define DISPLAY_CS -1
#define DISPLAY_DC -1
#define DISPLAY_RST -1
#define DISPLAY_BUSY -1

#define LCD_QSPI_HOST           (SPI2_HOST)
#define PIN_NUM_QSPI_CS         (GPIO_NUM_45)
#define PIN_NUM_QSPI_PCLK       (GPIO_NUM_47)
#define PIN_NUM_QSPI_DATA0      (GPIO_NUM_21)
#define PIN_NUM_QSPI_DATA1      (GPIO_NUM_48)
#define PIN_NUM_QSPI_DATA2      (GPIO_NUM_40)
#define PIN_NUM_QSPI_DATA3      (GPIO_NUM_39)
#define PIN_NUM_QSPI_RST        (GPIO_NUM_NC)
#define PIN_NUM_QSPI_DC         (GPIO_NUM_8)
#define PIN_NUM_QSPI_TE         (GPIO_NUM_38)
#define PIN_NUM_QSPI_BL         (GPIO_NUM_1)

#define PIN_NUM_QSPI_TOUCH_SCL  (GPIO_NUM_8)
#define PIN_NUM_QSPI_TOUCH_SDA  (GPIO_NUM_4)
#define PIN_NUM_QSPI_TOUCH_RST  (-1)
#define PIN_NUM_QSPI_TOUCH_INT  (-1)
#endif

enum tkb_mode { speed, start };

extern bool should_sleep;
extern bool gps_is_ready;
extern bool do_read_gnss;
extern bool do_start_timer;
extern tkb_mode display_mode;
extern bool start_timer_started;
