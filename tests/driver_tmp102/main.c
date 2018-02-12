#include <stdio.h>

#include "xtimer.h"
#include "tmp102.h"
#include "tmp102_params.h"

int main(void)
{
    tmp102_t dev;
    int16_t rawtemp;
    float tamb;

    puts("TMP102 infrared thermopile sensor driver test application\n");
    printf("Initializing TMP102 sensor at I2C_%i ... ", tmp102_params[0].i2c);
    if (tmp102_init(&dev,  &tmp102_params[0]) != TMP102_OK) {
        puts("init device [ERROR]");
        return -1;
    }

    xtimer_usleep(TMP102_CONVERSION_TIME);
    puts("[SUCCESS]\n");

    while (1) {
        tmp102_read(&dev, &rawtemp);

        printf("Raw data T: %5d \n", rawtemp);
        tmp102_convert(rawtemp,  &tamb);
        printf("Data Tabm: %d \n", (int)(tamb*100));

        xtimer_usleep(TMP102_CONVERSION_TIME);
    }

    return 0;
}
