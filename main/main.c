#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_err.h"
#include "esp_log.h"
#include "hd44780.h"
#include "i2cdev.h"
#include "pcf8574.h"

#define I2C_PORT    I2C_NUM_0
#define I2C_SDA     GPIO_NUM_8
#define I2C_SCL     GPIO_NUM_9
#define LCD_ADDR    0x27

static const char *TAG = "LCD_TEST";

static i2c_dev_t pcf8574;

static esp_err_t lcd_write_cb(const hd44780_t *lcd, uint8_t data)
{
    (void)lcd;
    return pcf8574_port_write(&pcf8574, data);
}

void app_main(void)
{
    hd44780_t lcd = {
        .write_cb = lcd_write_cb,
        .font = HD44780_FONT_5X8,
        .lines = 2,
        .pins = {
            .rs = 0,
            .e = 2,
            .d4 = 4,
            .d5 = 5,
            .d6 = 6,
            .d7 = 7,
            .bl = 3,
        },
    };

    ESP_LOGI(TAG, "Inicializando I2C y LCD con libreria");

    memset(&pcf8574, 0, sizeof(pcf8574));
    ESP_ERROR_CHECK(i2cdev_init());
    ESP_ERROR_CHECK(pcf8574_init_desc(&pcf8574, LCD_ADDR, I2C_PORT, I2C_SDA, I2C_SCL));
    ESP_ERROR_CHECK(hd44780_init(&lcd));
    ESP_ERROR_CHECK(hd44780_switch_backlight(&lcd, true));

    ESP_ERROR_CHECK(hd44780_clear(&lcd));
    ESP_ERROR_CHECK(hd44780_gotoxy(&lcd, 0, 0));
    ESP_ERROR_CHECK(hd44780_puts(&lcd, "TD3 ESP32-S3"));
    ESP_ERROR_CHECK(hd44780_gotoxy(&lcd, 0, 1));
    ESP_ERROR_CHECK(hd44780_puts(&lcd, "LCD con lib OK"));

    uint16_t counter = 0;

    while (1) {
        char text[17];

        snprintf(text, sizeof(text), "Contador:%03u", counter);
        ESP_ERROR_CHECK(hd44780_gotoxy(&lcd, 0, 1));
        ESP_ERROR_CHECK(hd44780_puts(&lcd, "                "));
        ESP_ERROR_CHECK(hd44780_gotoxy(&lcd, 0, 1));
        ESP_ERROR_CHECK(hd44780_puts(&lcd, text));

        counter = (counter + 1) % 1000;
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
