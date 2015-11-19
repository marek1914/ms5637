#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

#include "src/ms5637.h"

#define DEV_FS "/dev/i2c-1"

void writelog(char *str)
{
    time_t     now;
    struct tm  *ts;
    char       buf[256];

    now = time(NULL);

    /* Format and print the time, "ddd yyyy-mm-dd hh:mm:ss zzz" */
	ts = localtime(&now);
	strftime(buf, sizeof(buf), "\n%a %Y-%m-%d %H:%M:%S ", ts);
	strcat(buf,str);

    /* log */
	printf("%s", buf);
}

int handleError()
{
    char *charBuffer = strerror(errno);
    writelog(charBuffer);
    return -1;
}

int main()
{
    if(init(DEV_FS, 0) == 0) {
        float temperature = 0.0;
        while(1) {
            if(readTemperature(&temperature, OSR_8192, CELSIUS) == 0) {
                char log[8];
                sprintf(log, "%f", temperature);
                writelog(log);
            } else {
                handleError();
            }
            sleep(5);
        }
    } else {
        return handleError();
    }
    return 0;
}
