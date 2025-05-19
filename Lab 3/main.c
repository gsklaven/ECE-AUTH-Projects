#include "platform.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "uart.h"
#include "queue.h"
#include "gpio.h"
#include "timer.h"
#include "delay.h"

// Constants
#define BUFF_SIZE 128           // UART input buffer size
#define LED_PIN PA_5            // LED output pin
#define TOUCH_PIN PC_13         // Touch input pin
#define PASSWORD "1234"         // Password for user authentication

// Variables for storing user AEM and its last two digits
char aem_str[10];
uint8_t aem_digit1 = 0, aem_digit2 = 0;

// Enumerations for display modes and system modes
typedef enum { SHOW_TEMP, SHOW_HUM, SHOW_BOTH } DisplayMode;
typedef enum { MODE_A, MODE_B } SystemMode;

DisplayMode display_mode = SHOW_BOTH;  // Default display both temperature and humidity
SystemMode current_mode = MODE_A;      // Default system mode is Normal (MODE_A)

uint8_t read_interval = 5;              // Sensor read interval in seconds
uint8_t profile_changes = 0;            // Count of profile changes triggered by user

uint8_t last_temp = 0;                  // Last temperature reading
uint8_t last_hum = 0;                   // Last humidity reading

// Counters for touch events and panic/reset logic
uint8_t touch_count = 0;
uint8_t panic_count = 0;
uint8_t safe_count = 0;

// UART receive queue and input buffer
Queue rx_queue;
char input_buffer[BUFF_SIZE];
volatile uint8_t command_ready = 0;

// Helper function to send single char over UART by wrapping into a string
void uart_write(char c) {
    char buf[2] = {c, '\0'};
    uart_print(buf);
}

// UART receive interrupt handler: enqueue incoming char and flag when Enter pressed
void uart_rx_isr(uint8_t rx) {
    queue_enqueue(&rx_queue, rx);
    if (rx == '\r') command_ready = 1; // Mark command ready on carriage return
}

// Read a line from UART until Enter is pressed, echoing typed characters
void read_line(char *buffer, int max_len) {
    int index = 0;
    uint8_t ch;

    while (1) {
        // Wait for data in queue
        while (!queue_dequeue(&rx_queue, &ch));

        // End input on Enter key (CR or LF)
        if (ch == '\r' || ch == '\n') {
            uart_print("\r\n");
            break;
        }

        // Store character in buffer and echo back to UART
        if (index < max_len - 1) {
            buffer[index++] = ch;
            uart_write(ch);
        }
    }

    buffer[index] = '\0'; // Null-terminate input string
}

// Prints the current system status (mode, temperature, humidity, profile changes)
void print_status() {
    char msg[128];
    sprintf(msg,
        "Mode: %s\r\nTemp: %d C\r\nHum: %d %%\r\nProfile changes: %d\r\n",
        (current_mode == MODE_A) ? "Normal" : "Alert",
        last_temp, last_hum, profile_changes
    );
    uart_print(msg);
}

// Process simple single-character commands sent via UART
void process_command(char cmd) {
    switch (cmd) {
        case 'a':
            // Decrease sensor read interval, min 2 sec
            if (read_interval > 2) read_interval--;
            break;
        case 'b':
            // Increase sensor read interval, max 10 sec
            if (read_interval < 10) read_interval++;
            break;
        case 'c':
            // Cycle through display modes: TEMP -> HUM -> BOTH
            display_mode = (display_mode + 1) % 3;
            break;
        case 'd':
            // Print system status
            print_status();
            break;
    }
}

// Request password from user, then prompt for AEM and extract last two digits
void request_password() {
    char input[BUFF_SIZE];

    while (1) {
        uart_print("Enter password: ");
        read_line(input, BUFF_SIZE);

        if (strcmp(input, PASSWORD) == 0) break;  // Correct password entered

        uart_print("Wrong password. Try again.\r\n");
    }

    uart_print("Access granted.\r\n");

    uart_print("Enter your AEM: ");
    read_line(aem_str, BUFF_SIZE);

    // Extract last two digits of AEM to configure read interval
    size_t len = strlen(aem_str);
    if (len >= 2) {
        aem_digit1 = aem_str[len - 2] - '0';
        aem_digit2 = aem_str[len - 1] - '0';
    }
}

// Touch input interrupt handler: toggle system mode and update counters
void touch_callback(int status) {
    (void)status; // Unused parameter

    touch_count++;
    profile_changes++;

    // Toggle system mode between MODE_A and MODE_B
    current_mode = (current_mode == MODE_A) ? MODE_B : MODE_A;

    // Every 3 touches, adjust read interval based on AEM digits sum
    if (touch_count % 3 == 0) {
        read_interval = aem_digit1 + aem_digit2;
        if (read_interval < 2) read_interval = 2;
        if (read_interval > 10) read_interval = 10;
    }
}

// Simulated sensor reading function (replace with real sensor code)
void read_sensor() {
    last_temp = (last_temp + 1) % 40;  // Simulate temperature cycling 0-39 C
    last_hum = (last_hum + 2) % 100;   // Simulate humidity cycling 0-99 %
}

// Timer interrupt handler: update sensor, display data, and manage alert logic
void timer_callback() {
    read_sensor();

    // Show temperature if mode includes it
    if (display_mode == SHOW_TEMP || display_mode == SHOW_BOTH) {
        char msg[32];
        sprintf(msg, "Temp: %d C\r\n", last_temp);
        uart_print(msg);
    }

    // Show humidity if mode includes it
    if (display_mode == SHOW_HUM || display_mode == SHOW_BOTH) {
        char msg[32];
        sprintf(msg, "Hum: %d %%\r\n", last_hum);
        uart_print(msg);
    }

    // In alert mode (MODE_B), check for threshold violations
    if (current_mode == MODE_B) {
        if (last_temp > 25 || last_hum > 60) {
            gpio_toggle(LED_PIN);  // Blink LED to indicate alert
            safe_count = 0;

            if (++panic_count >= 3) {  // If violation persists 3 cycles, reset system
                uart_print("Panic reset: Values exceeded!\r\n");
                NVIC_SystemReset();
            }
        } else {
            panic_count = 0;
            if (++safe_count >= 5) {
                gpio_set(LED_PIN, 0);  // Turn off LED after safe period
            }
        }
    }
}

int main() {
    // Initialize UART, GPIOs, timers, and interrupts
    queue_init(&rx_queue, BUFF_SIZE);
    uart_init(115200);
    uart_set_rx_callback(uart_rx_isr);
    uart_enable();

    gpio_set_mode(LED_PIN, Output);
    gpio_set(LED_PIN, 0);

    gpio_set_mode(TOUCH_PIN, PullUp);
    gpio_set_trigger(TOUCH_PIN, Rising);
    gpio_set_callback(TOUCH_PIN, touch_callback);

    timer_init(1000000 * read_interval); // Initialize timer with read_interval in microseconds
    timer_set_callback(timer_callback);
    timer_enable();

    __enable_irq();  // Enable global interrupts

    // Block here until user authenticates successfully
    request_password();

    // Main loop: process UART commands and wait for interrupts
    while (1) {
        uint8_t rx_char;

        if (queue_dequeue(&rx_queue, &rx_char)) {
            if (rx_char == 's') {
                // Check if full "status" command is entered
                char cmd_buff[6];
                cmd_buff[0] = rx_char;
                for (int i = 1; i < 6; i++) {
                    while (!queue_dequeue(&rx_queue, &rx_char));
                    cmd_buff[i] = rx_char;
                }

                if (strncmp(cmd_buff, "status", 6) == 0) {
                    print_status();
                }
            } else {
                process_command(rx_char);
            }
        }

        __WFI();  // Low-power wait until interrupt (efficient idle)
    }

    return 0;
}
