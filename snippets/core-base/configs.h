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
#define FILE      "/test/file"    // should be the test or subsystem name
#define TITLE     "Time,read\r\n" // file title
#define EXTENSION ".csv"          // file extension (.csv, .txt, ...)

// -----pinout-----
#define CHIPSELECT  5  // micro SD chip select pin
#define PIN_EXAMPLE 35 // example sensor pin

// -----esp-now addresses-----
uint8_t address_control[] = {0x0C, 0xB8, 0x15, 0xC3, 0xF0, 0x50};
