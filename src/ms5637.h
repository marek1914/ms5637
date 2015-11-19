#define MS5637_ADDR 0x76

typedef enum {
    OSR_256 = 0x00,
    OSR_512 = 0x01,
    OSR_1024 = 0x02,
    OSR_2048 = 0x03,
    OSR_4096 = 0x04,
    OSR_8192 = 0x05,
} osr_t;

typedef enum {
  CELSIUS,
  FAHRENHEIT
} TempUnit;

int init(char* dev, int reset);

int readTemperature(float *temperature, osr_t osr, TempUnit tempScale);
