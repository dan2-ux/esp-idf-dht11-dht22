#include "dht.h"
#include "esp_rom_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

static uint8_t data1[5];
static uint8_t data2[5];

namespace dht11
{
    void init(gpio_num_t pin)
    {
        gpio_reset_pin(pin);
    }
    bool getData(gpio_num_t pin)
    {
        uint8_t dataPos = 0, pos = 7;
        uint32_t count = 0;

        for (int i = 0; i < 5; i++)
            data1[i] = 0;

        gpio_set_direction(pin, GPIO_MODE_OUTPUT);
        gpio_set_level(pin, 0);
        vTaskDelay(pdMS_TO_TICKS(20));
        gpio_set_level(pin, 1);
        esp_rom_delay_us(40);

        gpio_set_direction(pin, GPIO_MODE_INPUT);

        count = 0;
        while (gpio_get_level(pin) == 0)
        {
            if (++count > 10000)
            {
                return false;
            }
        }

        count = 0;
        while (gpio_get_level(pin) == 1)
        {
            if (++count > 10000)
            {
                return false;
            }
        }

        for (int i = 0; i < 40; i++)
        {
            count = 0;
            while (gpio_get_level(pin) == 0)
            {
                if (++count > 10000)
                {
                    return false;
                }
            }

            count = 0;
            while (gpio_get_level(pin) == 1)
            {
                count += 1;
                esp_rom_delay_us(1);
                if (count > 100)
                    break;
            }

            if (count > 40)
                data1[dataPos] |= (1 << pos);

            if (pos == 0)
            {
                dataPos += 1;
                pos = 7;
            }
            else
            {
                pos -= 1;
            }
        }

        if (data1[4] != (uint8_t)(data1[0] + data1[1] + data1[2] + data1[3]))
            return false;

        return true;
    }

    float getHUM()
    {
        return data1[0];
    }
    float getTEMP()
    {
        return data1[2];
    }
}

int getSignalLevel(gpio_num_t pin, int usTimeOut, bool state)
{

    int uSec = 0;
    while (gpio_get_level(pin) == state)
    {

        if (uSec > usTimeOut)
            return -1;

        ++uSec;
        esp_rom_delay_us(1);
    }

    return uSec;
}

namespace dht22
{
    void init(gpio_num_t pin)
    {
        gpio_reset_pin(pin);
    }
    bool getData(gpio_num_t pin)
    {
        uint8_t dataPos = 0, pos = 7;
        uint32_t count = 0;

        for (int i = 0; i < 5; i++)
            data2[i] = 0;

        gpio_set_direction(pin, GPIO_MODE_OUTPUT);
        gpio_set_level(pin, 0);
        vTaskDelay(pdMS_TO_TICKS(3));
        gpio_set_level(pin, 1);
        esp_rom_delay_us(30);

        gpio_set_direction(pin, GPIO_MODE_INPUT);

        int uSec = getSignalLevel(pin, 85, 0);
        if (uSec < 0)
        {
            ESP_LOGE("DHT22", "Error at first loop");
            return false;
        }

        uSec = getSignalLevel(pin, 85, 1);
        if (uSec < 0)
        {
            ESP_LOGE("DHT22", "Error at second loop");
            return false;
        }

        for (int i = 0; i < 40; i++)
        {
            uSec = getSignalLevel(pin, 56, 0);
            if (uSec < 0)
            {
                ESP_LOGE("DHT22", "Error at third loop");
                return false;
            }

            count = 0;
            while (gpio_get_level(pin) == 1)
            {
                count += 1;
                esp_rom_delay_us(1);
                if (count > 100)
                    break;
            }

            if (count > 40)
                data2[dataPos] |= (1 << pos);

            if (pos == 0)
            {
                dataPos += 1;
                pos = 7;
            }
            else
            {
                pos -= 1;
            }
        }

        if (data2[4] != (uint8_t)(data2[0] + data2[1] + data2[2] + data2[3]))
        {
            ESP_LOGE("DHT22", "Error at data[4]");
            return false;
        }

        return true;
    }

    float getHUM()
    {
        uint16_t raw = (uint16_t)(data2[0] << 8) | data2[1];
        return raw * 0.1f;
    }

    float getTEMP()
    {
        int16_t raw = (int16_t)(data2[2] << 8) | data2[3];
        if (raw & 0x8000)
        {
            raw &= 0x7FFF;
            return -raw * 0.1f;
        }
        return raw * 0.1f;
    }
}
