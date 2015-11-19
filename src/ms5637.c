#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>

#include "ms5637.h"

int     file;
unsigned short prom[7];
unsigned short initialized = 0;

int init(char* dev, int reset)
{
    int i;
    char promBuffer[2];

    /* Open file descriptor for i2c device */
    if ((file = open(dev, O_RDWR)) < 0) {
        return errno;
    }

    /* Establish communication with the MS5637 module */
    if (ioctl(file, I2C_SLAVE, MS5637_ADDR) < 0) {
        return errno;
    }

    /* Reset prom sequence - Loads calibration data */
    if(reset == 1)
    {
        char reset = 0x1E;
        if (write(file,&reset,1) != 1) {
            return errno;
        }
    }

    /* Read PROM data */
    for(i = 0; i < 7; i++) {
        char wr = 0xA0+(i<<1);
        if (write(file,&wr,1) != 1) {
            return errno;
        }
        if (read(file,promBuffer,2) != 2) {
            return errno;
        } else {
            prom[i] = ((short)promBuffer[0]) << 8;
            prom[i] |= promBuffer[1];
        }
    }

    initialized = 1;
    return 0;
}

int readTemperature(float *temperature, osr_t osr, TempUnit tempScale)
{
    char adcBuffer[3];
    unsigned int d2;
    int t1, dt;
    long long t2;
    char cmd = 0x05<<4;
    cmd |= osr;

    if (write(file,&cmd,1) != 1)
    {
        return errno;
    }
    else
    {
        usleep(20000);
        char readAdc = 0x00;
        if (write(file,&readAdc,1) != 1)
        {
            return errno;
        }
        else
        {
            if (read(file,adcBuffer,3) != 3)
            {
                return errno;
            }
            else
            {
                d2 = ((int)adcBuffer[0]) << 16;
                d2 |= ((int)adcBuffer[1]) << 8;
                d2 |= adcBuffer[2];

                if(d2 == 0)
                {
                    return -1;
                }

                dt = d2 - prom[5] * (1L<<8);
                t1 = 2000 + dt * prom[6] / (1L<<23);

                if(t1 >= 2000) {
                    t2 = 5 * (dt * dt) / (1LL<<38);
                } else {
                    t2 = 3 * (dt * dt) / (1LL<<33);
                }

                if(temperature != NULL)
                {
                    *temperature = (float)(t1 - t2) / 100;
                    if(tempScale == FAHRENHEIT)
                    {
                        *temperature = *temperature * 9 / 5 + 32;
                    }
                }
                else
                {
                    return -1;
                }
            }
        }
    }

    return 0;
}
