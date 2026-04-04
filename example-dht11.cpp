#include "dht.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#define pin GPIO_NUM_4

void dht_11(void *arg)
{
    static const char *TAG = "DHT11";
    while (1)
    {
        if (!dht11_status(pin))
        {
            ESP_LOGE(TAG, "Error");
        }

        ESP_LOGI(TAG, "Temp: %.2f", dht11_getTEMP());
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

extern "C" void app_main()
{

    dht_init(pin);
    xTaskCreatePinnedToCore(dht_11, "get temp and hum", 2048, NULL, 5, NULL, 1);
}
