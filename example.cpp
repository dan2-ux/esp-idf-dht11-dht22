#include "dht.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

void dht_11(void *arg)
{
    static const char *TAG = "DHT11";
    while (1)
    {
        if (!dht11::getData(GPIO_NUM_4))
        {
            ESP_LOGE(TAG, "Error");
        }

        ESP_LOGI(TAG, "Temp: %.2f", dht11::getTEMP());
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void dht_22(void *arg)
{
    static const char *TAG = "DHT22";
    while (1)
    {
        if (!dht22::getData(GPIO_NUM_4))
        {
            ESP_LOGE(TAG, "Error");
        }

        ESP_LOGI(TAG, "Temp: %.2f", dht22::getTEMP());
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

extern "C" void app_main()
{

    dht11::init(GPIO_NUM_4);
    xTaskCreatePinnedToCore(dht_11, "get temp and hum", 2048, NULL, 5, NULL, 1);

    dht22::init(GPIO_NUM_5);
    xTaskCreatePinnedToCore(dht_22, "get temp and hum", 2048, NULL, 4, NULL, 1);
}
