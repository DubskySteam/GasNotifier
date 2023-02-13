
#include <string.h>
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include "driver/gpio.h"
#include "rom/gpio.h"
#include "nvs_flash.h"
#include "inits.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

#define AO 35
#define D0 26

static esp_adc_cal_characteristics_t adc1_chars;

void app_main() {
    
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_ERROR_CHECK(esp_netif_init());

    initAP();
    initGPIO();
    initMQ7();

    while (1) 
    {
        vTaskDelay(pdMS_TO_TICKS(100));
        ESP_LOGI("SENSOR", "DIGITAL: %d   ANALOG: %ld mV", gpio_get_level(D0) , esp_adc_cal_raw_to_voltage(adc1_get_raw(ADC1_CHANNEL_7), &adc1_chars));
    }
}

void initMQ7() {
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_7, ADC_ATTEN_DB_11);
}

void readGas() {
    int ao = gpio_get_level(AO);
    int d0 = gpio_get_level(D0);

    printf("AO: %d, D0: %d\n", ao, d0);
}

void initGPIO() {
    gpio_pad_select_gpio(AO);
    gpio_set_direction(AO, GPIO_MODE_INPUT);

    gpio_pad_select_gpio(D0);
    gpio_set_direction(D0, GPIO_MODE_INPUT);
}

void initAP() {
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = "GasNotifier",
            .ssid_len = strlen("GasNotifier"),
            .password = "gasnotifier",
            .max_connection = 10,
            .authmode = WIFI_AUTH_OPEN,
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}