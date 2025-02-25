#include "sontek_flow.h"
#include "notoriOS.h"
#include "debug.h"

/* === Driver Functions === */

// Buffer for SPI received data
static uint16_t spi_buffer[N_READINGS] = {0};
static uint8_t num_readings = 0;

// SPI configuration parameters
#define SPI_CLOCK_FREQ      1000000  // 1MHz SPI clock
#define SPI_DATA_WIDTH      12       // 12-bit data width
#define SPI_MODE            0        // Mode 0: CPOL=0, CPHA=0

void sontek_flow_Init(void) {
    // Initialize SPI hardware
    sontek_flow_Start();
    
    // Configure SPI parameters
    sontek_flow_SetActiveSlaveSelect(0);  // Set CS pin
    sontek_flow_SetBitRate(SPI_CLOCK_FREQ);
    
    // Initialize power pin
    sontek_flow_Power_Write(OFF);
    sontek_flow_CS_Write(HIGH);  // CS is active low, so initialize HIGH
}

// Function to perform a single SPI read
uint16_t spi_read_value(void) {
    uint16_t value = 0;
    
    // Assert CS (active low)
    sontek_flow_CS_Write(LOW);
    
    // Small delay to meet sensor timing requirements
    CyDelayUs(10);
    
    // Send dummy byte to receive data
    // For 12-bit reading, we need to read two bytes
    uint8_t msb = sontek_flow_ReadByte();
    uint8_t lsb = sontek_flow_ReadByte();
    
    // De-assert CS
    sontek_flow_CS_Write(HIGH);
    
    // Combine bytes into 12-bit value
    value = ((uint16_t)msb << 8) | lsb;
    value &= 0x0FFF;  // Mask to ensure 12-bit value
    
    return value;
}

sontek_flow_t sontek_flow_take_reading(void) {
    sontek_flow_t sontek_flow_output;
    sontek_flow_output.num_invalid_readings = 0;
    sontek_flow_output.num_valid_readings = 0;
    
    // Power up sensor
    sontek_flow_Power_Write(ON);
    CyDelay(100);  // Startup delay
    
    // Take multiple readings
    for(uint8_t i = 0; i < N_READINGS; i++) {
        uint16_t reading = spi_read_value();
        
        if(reading != 0 && reading != READING_INVALID) {
            sontek_flow_output.all_flow_readings[sontek_flow_output.num_valid_readings] = reading;
            sontek_flow_output.num_valid_readings++;
        } else {
            sontek_flow_output.num_invalid_readings++;
        }
        
        CyDelay(50);  // Delay between readings
    }
    
    // Power down sensor
    sontek_flow_Power_Write(OFF);
    
    // Calculate median reading
    if(sontek_flow_output.num_valid_readings > 0) {
        sontek_flow_output.level_reading = find_median(
            sontek_flow_output.all_level_readings,
            sontek_flow_output.num_valid_readings
        );
    } else {
        sontek_flow_output.level_reading = 0;
    }
    
    return sontek_flow_output;
}

uint8_t sontek_flow(void) {
    char *compare_location;
    
    compare_location = strstr(sontek_flow_inbox, "OFF");        
    if(compare_location != NULL) {
        return 0;
    }
    
    compare_location = strstr(sontek_flow_inbox, "ON");        
    if(compare_location != NULL) {
        // Get clock time and save to timeStamp
        long timeStamp = getTimeStamp();
        char c_timeStamp[32];
        snprintf(c_timeStamp, sizeof(c_timeStamp), "%ld", timeStamp);
        
        char value[DATA_MAX_KEY_LENGTH];
        sontek_flow_t m_sontek_flow = sontek_flow_take_reading();
        
        if(m_sontek_flow.num_valid_readings > 0) {
            snprintf(value, sizeof(value), "%d", m_sontek_flow.flow_reading);
            printNotif(NOTIF_TYPE_EVENT, "spi_flow=%s", value);
            pushData("spi_flow", value, timeStamp);
            
            // Log to SD card
            SD_write("data.txt", "a+", c_timeStamp);
            SD_write("data.txt", "a+", " spi_flow: ");
            SD_write("data.txt", "a+", value);
            SD_write("data.txt", "a+", " ");
        } else {
            printNotif(NOTIF_TYPE_ERROR, "Could not get valid readings from SPI sensor.");
            pushData("spi_flow", "-1", timeStamp);
        }
    }
    
    return 0;
}

test_t sontek_flow_test(void) {
    test_t test;
    test.status = 0;
    snprintf(test.test_name, sizeof(test.test_name), "TEST_SPI_FLOW_SENSOR");
    
    sontek_flow_t sensor = sontek_flow_take_reading();
    
    snprintf(test.reason, sizeof(test.reason), 
        "nWANTED=%d,nRECEIVED=%d,nVALID=%d,nMEDIAN=%d.",
        N_READINGS,
        sensor.num_valid_readings + sensor.num_invalid_readings,
        sensor.num_valid_readings,
        sensor.level_reading
    );
    
    if(sensor.num_valid_readings == N_READINGS) {
        test.status = 1;
    } else if((sensor.num_valid_readings > 0) && 
              (sensor.num_valid_readings < N_READINGS)) {
        test.status = 2;
    }
    
    return test;
}

// Reuse existing sorting functions
void sort(int16_t a[], int16_t n) {
    int16_t i, j;
    for(i = 0; i < n-1; i++) {
        for(j = 0; j < n-i-1; j++) {
            if(a[j] > a[j+1]) {
                swap(&a[j], &a[j+1]);
            }
        }
    }
}

void swap(int16_t *p, int16_t *q) {
    int16_t t = *p;
    *p = *q;
    *q = t;
}

uint16_t find_median(int16_t array[], uint8_t n) {
    int16_t median = 0;
    sort(array, n);
    
    if(n % 2 == 0) {
        median = (array[(n-1)/2] + array[n/2]) / 2.0;
    } else {
        median = array[n/2];
    }
    
    return median;
} 