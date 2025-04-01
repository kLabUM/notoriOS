#ifndef sontek_flow
#define sontek_flow

#include <stdint.h>

// Constants
#define N_READINGS 11
#define READING_INVALID 0xFFF  // Invalid reading for 12-bit value
#define ON 1
#define OFF 0
#define HIGH 1
#define LOW 0

// Structure to hold flow sensor readings
typedef struct {
    int16_t all_level_readings[N_READINGS];
    uint16_t flow_reading;
    uint8_t num_valid_readings;
    uint8_t num_invalid_readings;
} sontek_flow_t;

// Function declarations
void sontek_flow_Init(void);
uint16_t spi_read_value(void);
sontek_flow_t sontek_flow_take_reading(void);
uint8_t sontek_flow(void);
test_t sontek_flow_test(void);
void sort(int16_t a[], int16_t n);
void swap(int16_t *p, int16_t *q);
uint16_t find_median(int16_t array[], uint8_t n);

#endif /* sontek_flow */ 