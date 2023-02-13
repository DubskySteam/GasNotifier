
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

void app_main() {
    
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_ERROR_CHECK(esp_netif_init());
   
    gpio_pad_select_gpio(AO);
    gpio_set_direction(AO, GPIO_MODE_INPUT);

    gpio_pad_select_gpio(D0);
    gpio_set_direction(D0, GPIO_MODE_INPUT);

    while(1)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        readGas();
    }
}

void readGas() {
    int ao = gpio_get_level(AO);
    int d0 = gpio_get_level(D0);

    printf("AO: %d, D0: %d\n", ao, d0);
}

void initGPIO() {
    
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