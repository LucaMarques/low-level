// -----general configs-----
#define BOARDID_01           1
#define SERIAL_FREQ          115200      // serial monitor baud rate
#define DEBUG_DELAY          5000        // delay between debug messages
#define DEBUG_MODE           false       // config if debug mode is active
#define BUFFER_SIZE          128         // block of data size to be written on SD card
#define QUEUE_SIZE           BUFFER_SIZE // queue size (<= BUFFER_SIZE)
#define QUEUE_TIMEOUT        1000        // queue timeout in millisecond
#define RATE_Hz              100         // reading rate in hertz
#define RATE_ms              (int(1000.0 / float(RATE_Hz))) // reading rate perido in milliseconds
#define REBOOT_ON_ERROR      false       // reboot on error flag
#define REBOOT_ON_DISCONNECT false       // reboot on disconnect flag
#define ESPNOW_BUFFER_SIZE   48          // espnow buffer char array size

// -----microSD configs-----
#define FILE      "/powertrain/temp"           // should be the test or subsystem name
#define TITLE     "time,temp_obj1,temp_amb1,temp_obj2,temp_amb2\r\n"    // file title
#define EXTENSION ".csv"                                                   // file extension (.csv, .txt, ...)

// -----pinout-----
#define CHIPSELECT  5  // micro SD chip select pin
#define PIN_EXAMPLE 35 // example sensor pin
#define SDA_2 33       // Second SDA port for I2C communication
#define SCL_2 32       // Second SCL port for I2C communication

// -----esp-now addresses-----
uint8_t address_control[] = {0x5C, 0xCF, 0x7F, 0x65, 0xF1, 0x9E};
