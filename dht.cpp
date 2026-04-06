#include "dht.h"
#include "esp_rom_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

namespace DHT
{
    class Setup
    {

    public:
        Setup(gpio_num_t p) : pin(p) {}
        void dht_init()
        {
            gpio_reset_pin(pin);
            gpio_set_direction(pin, GPIO_MODE_OUTPUT);
        }

    private:
        gpio_num_t pin;
    };

    class DHT11
    {
    public:
        DHT11(gpio_num_t p) : pin(p) {}

        bool read()
        {
            memset(data, 0, 5);

            gpio_set_direction(pin, GPIO_MODE_OUTPUT);
            gpio_set_level(pin, 0);
            vTaskDelay(pdMS_TO_TICKS(20));
            gpio_set_level(pin, 1);
            esp_rom_delay_us(40);

            gpio_set_direction(pin, GPIO_MODE_INPUT);

            // wait for response...
            if (!waitLevel(0) || !waitLevel(1))
                return false;

            for (int i = 0; i < 40; i++)
            {
                if (!waitLevel(0))
                    return false;

                int count = measureHighTime();
                if (count < 0)
                    return false;

                if (count > 40)
                    data[i / 8] |= (1 << (7 - (i % 8)));
            }

            // checksum
            if (data[4] != (uint8_t)(data[0] + data[1] + data[2] + data[3]))
                return false;

            return true;
        }

        float getTemp() { return data[2]; }
        float getHum() { return data[0]; }

    private:
        gpio_num_t pin;
        uint8_t data[5];

        bool waitLevel(int level)
        {
            int count = 0;
            while (gpio_get_level(pin) == level)
            {
                if (++count > 10000)
                    return false;
            }
            return true;
        }

        int measureHighTime()
        {
            int count = 0;
            while (gpio_get_level(pin) == 1)
            {
                esp_rom_delay_us(1);
                if (++count > 100)
                    break;
            }
            return count;
        }
    };

    class DHT22
    {
    public:
        DHT22(gpio_num_t p) : pin(p) {}

        bool read()
        {
            memset(data, 0, 5);

            gpio_set_direction(pin, GPIO_MODE_OUTPUT);
            gpio_set_level(pin, 0);
            vTaskDelay(pdMS_TO_TICKS(2)); // DHT22 uses shorter start
            gpio_set_level(pin, 1);
            esp_rom_delay_us(40);

            gpio_set_direction(pin, GPIO_MODE_INPUT);

            if (!waitLevel(0) || !waitLevel(1))
                return false;

            for (int i = 0; i < 40; i++)
            {
                if (!waitLevel(0))
                    return false;

                int count = measureHighTime();
                if (count < 0)
                    return false;

                if (count > 40)
                    data[i / 8] |= (1 << (7 - (i % 8)));
            }

            // checksum
            if (data[4] != (uint8_t)(data[0] + data[1] + data[2] + data[3]))
                return false;

            return true;
        }

        float getTemp()
        {
            int16_t raw = (data[2] << 8) | data[3];

            // check negative bit
            if (raw & 0x8000)
            {
                raw &= 0x7FFF;
                return -raw / 10.0;
            }

            return raw / 10.0;
        }

        float getHum()
        {
            uint16_t raw = (data[0] << 8) | data[1];
            return raw / 10.0;
        }

    private:
        gpio_num_t pin;
        uint8_t data[5];

        bool waitLevel(int level)
        {
            int count = 0;
            while (gpio_get_level(pin) == level)
            {
                if (++count > 10000)
                    return false;
            }
            return true;
        }

        int measureHighTime()
        {
            int count = 0;
            while (gpio_get_level(pin) == 1)
            {
                esp_rom_delay_us(1);
                if (++count > 100)
                    break;
            }
            return count;
        }
    };
};

void dht_init(gpio_num_t pin)
{
    DHT::Setup s(pin);
    s.dht_init();
}

bool dht11_status(gpio_num_t pin)
{
    DHT::DHT11 status(pin);
    return status.read();
}
bool dht22_status(gpio_num_t pin)
{
    DHT::DHT22 status(pin);
    return status.read();
}

float dht11_getTEMP(gpio_num_t pin)
{
    DHT::DHT11 temp(pin);
    return temp.getTemp();
}

float dht11_getHUM(gpio_num_t pin)
{
    DHT::DHT11 hum(pin);
    return hum.getHum();
}

float dht22_getTEMP(gpio_num_t pin)
{
    DHT::DHT22 temp(pin);
    return temp.getTemp();
}

float dht22_getHUM(gpio_num_t pin)
{
    DHT::DHT22 hum(pin);
    return hum.getHum();
}