#include <stdio.h>
#include <unistd.h>
#include "raspberry_gpio_op.h"

int main(int argc, char *argv[])
{
    int i = 0;

    GPIOExport(OPEN_DOOR_LED);
    GPIODirection(OPEN_DOOR_LED, OUT);

    for (i = 0; i < 20; i++) {
        GPIOWrite(OPEN_DOOR_LED, i % 2);
        usleep(2000 * 1000);
    }

    GPIOUnexport(POUT);
    return(0);
}
