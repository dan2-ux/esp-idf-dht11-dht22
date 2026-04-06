#pragma once

#include "driver/gpio.h"

namespace dht11
{
    void init(gpio_num_t pin);
    bool getData(gpio_num_t pin);
    float getHUM();
    float getTEMP();
}

namespace dht22
{
    void init(gpio_num_t pin);
    bool getData(gpio_num_t pin);
    float getHUM();
    float getTEMP();
}

#ifdef __cplusplus
extern "C"
{
#endif

    void dht_init(gpio_num_t pin);
    bool dht_status(gpio_num_t pin);

    float dht11_getTEMP(gpio_num_t pin);
    float dht11_getHUM(gpio_num_t pin);

    float dht22_getTEMP(gpio_num_t pin);
    float dht22_getHUM(gpio_num_t pin);

#ifdef __cplusplus
}
#endif