#include "hw_config.h"
#include <stddef.h>

static spi_t spi = {
    .hw_inst = spi0,
    .miso_gpio = 16,
    .mosi_gpio = 19,
    .sck_gpio = 18,
    .baud_rate = 12500000
};

static sd_card_t sd_card = {
    .pcName = "0:",
    .spi = &spi,
    .ss_gpio = 17
};

size_t sd_get_num() { return 1; }
sd_card_t *sd_get_by_num(size_t num) {
    if (num == 0) return &sd_card;
    return NULL;
}

size_t spi_get_num() { return 1; }
spi_t *spi_get_by_num(size_t num) {
    if (num == 0) return &spi;
    return NULL;
}