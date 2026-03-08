#include "platform.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "uart.h"
#include "queue.h"
#include "gpio.h"
#include "timer.h"
#include "delay.h"
#include "dht11_2.h"

// === Definitions ===
#define BUFF_SIZE 128
#define LED_PIN PA_5
#define TOUCH_PIN PC_13
#define DHT11_PIN PB_10
#define PASSWORD "1234"

// === Variables ===
char aem_str[20];
uint8_t aem_digit1 = 0, aem_digit2 = 0;

typedef enum { SHOW_TEMP, SHOW_HUM, SHOW_BOTH } DisplayMode;
typedef enum { MODE_A, MODE_B } SystemMode;

DisplayMode display_mode = SHOW_BOTH;
SystemMode current_mode = MODE_A;

uint8_t read_interval = 5;
uint8_t profile_changes = 0;
uint8_t last_temp = 0;
uint8_t last_hum = 0;
uint8_t touch_count = 0;
uint8_t panic_count = 0;
uint8_t safe_count = 0;

Queue rx_queue;
char input_buffer[BUFF_SIZE];
volatile uint8_t command_ready = 0;

// LED blinking for Alert Mode
volatile uint8_t led_blink_active = 0;
volatile uint8_t led_state = 0;
volatile uint16_t led_counter = 0;

// === Prototypes ===
void timer_callback(void);
void print_status(void);
void process_command(char cmd);
void request_password(void);
void read_sensor(void);
void touch_callback(int status);
void read_line(char *buffer, int max_len);
void handle_led_blinking(void);

void uart_rx_isr(uint8_t rx) {
    queue_enqueue(&rx_queue, rx);
    if (rx == '\r')
        command_ready = 1;
}

void read_line(char *buffer, int max_len) {
    int index = 0;
    uint8_t ch;
    while (1) {
        while (!queue_dequeue(&rx_queue, &ch));
        if (ch == '\r' || ch == '\n') {
            uart_print("\r\n");
            break;
        }
        if (index < max_len - 1) {
            buffer[index++] = ch;
            char buf[2] = {ch, '\0'};
            uart_print(buf);
        }
    }
    buffer[index] = '\0';
}

void print_status() {
    char msg[256];
    sprintf(msg,
        "Current state: %s\r\n"
        "Temperature: %d C\r\n"
        "Humidity: %d %%\r\n"
        "Profile changes: %d\r\n",
        (current_mode == MODE_A) ? "Mode A (Normal)" : "Mode B (Alert)",
        last_temp, last_hum, profile_changes
    );
    uart_print(msg);
}

void process_command(char cmd) {
    switch (cmd) {
        case 'a':
            if (read_interval > 2) {
                read_interval--;
                char msg[64];
                sprintf(msg, "Frequency increased: %d seconds\r\n", read_interval);
                uart_print(msg);
            } else {
                uart_print("Minimum frequency: 2 seconds\r\n");
            }
            break;
        case 'b':
            if (read_interval < 10) {
                read_interval++;
                char msg[64];
                sprintf(msg, "Frequency decreased: %d seconds\r\n", read_interval);
                uart_print(msg);
            } else {
                uart_print("Maximum frequency: 10 seconds\r\n");
            }
            break;
        case 'c':
            display_mode = (display_mode + 1) % 3;
            switch(display_mode) {
                case SHOW_TEMP:
                    uart_print("Display: Temperature only\r\n");
                    break;
                case SHOW_HUM:
                    uart_print("Display: Humidity only\r\n");
                    break;
                case SHOW_BOTH:
                    uart_print("Display: Temperature and Humidity\r\n");
                    break;
            }
            break;
        case 'd':
            print_status();
            break;
        default:
            uart_print("Unknown command\r\n");
            break;
    }

    // Restart timer with new interval
    timer_disable();
    timer_init(1000000 * read_interval);
    timer_set_callback(timer_callback);
    timer_enable();
}

void request_password() {

    char input[BUFF_SIZE];
    
    while (1) {
        uart_print("Enter password: ");
        read_line(input, BUFF_SIZE);
        if (strcmp(input, PASSWORD) == 0) {
            uart_print("Correct password!\r\n");
            break;
        }
        uart_print("Wrong password. Try again.\r\n");
    }
    
    uart_print("Enter your AEM: ");
    read_line(aem_str, sizeof(aem_str));

    size_t len = strlen(aem_str);
    if (len >= 2) {
        aem_digit1 = aem_str[len - 2] - '0';
        aem_digit2 = aem_str[len - 1] - '0';
        
        // Validate digits
        if (aem_digit1 < 0 || aem_digit1 > 9) aem_digit1 = 0;
        if (aem_digit2 < 0 || aem_digit2 > 9) aem_digit2 = 0;
    }

    uart_print(
        "=== Environmental Monitoring System ===\r\n"
        "Options:\r\n"
        "a: Increase reading and printing frequency by 1s (minimum 2s)\r\n"
        "b: Decrease reading and printing frequency by 1s (maximum 10s)\r\n"
        "c: Toggle between Temperature / Humidity / Both display\r\n"
        "d: Print last values and system status\r\n"
        "Type 'status' for complete system status\r\n"
    );
}

void touch_callback(int status) {
    (void)status;
    touch_count++;
    profile_changes++;

    // Switch between modes
    current_mode = (current_mode == MODE_A) ? MODE_B : MODE_A;
    
    char msg[64];
    sprintf(msg, "Switched to %s\r\n", 
        (current_mode == MODE_A) ? "Mode A (Normal)" : "Mode B (Alert)");
    uart_print(msg);

    // Every 3rd touch, change reading interval based on AEM digits
    if (touch_count % 3 == 0) {
        read_interval = aem_digit1 + aem_digit2;
        if (read_interval < 2) read_interval = 2;
        if (read_interval > 10) read_interval = 10;

        sprintf(msg, "New frequency: %d seconds (based on AEM: %d+%d)\r\n", 
            read_interval, aem_digit1, aem_digit2);
        uart_print(msg);
    }
}

void read_sensor() {
    float* measurements = getData();
    if(measurements != NULL){
        last_hum  = (uint8_t)measurements[0];
        last_temp = (uint8_t)measurements[1];
        char msg[64];
        sprintf(msg, "H: %.1f, T: %.1f\r\n", measurements[0], measurements[1]);
        uart_print(msg);
    } else {
        uart_print("DHT11 read error\r\n");
    }
}

void handle_led_blinking(void) {
    if (current_mode == MODE_B) {
        if (last_temp > 25 || last_hum > 60) {
            // Should blink - activate blinking
            led_blink_active = 1;
            safe_count = 0;
        } else {
            // Values are safe
            safe_count++;
            if (safe_count >= 5) {
                // Turn off LED after 5 consecutive safe readings
                led_blink_active = 0;
                gpio_set(LED_PIN, 0);
            }
        }
    } else {
        // Mode A - LED should be off
        led_blink_active = 0;
        gpio_set(LED_PIN, 0);
    }
}

void timer_callback(void) {
    // Read sensor data
		uart_print("Timer tick\r\n");
    
		float* measurements = getData();
    if (measurements == NULL) {
        uart_print("ERROR: getData() returned NULL\r\n");
    } else {
        last_hum  = (uint8_t)measurements[0];
        last_temp = (uint8_t)measurements[1];
        char msg[64];
        sprintf(msg, "DBG: raw H=%.1f  T=%.1f\r\n", measurements[0], measurements[1]);
        uart_print(msg);
    }

    // Display based on current mode
    if (display_mode == SHOW_TEMP || display_mode == SHOW_BOTH) {
        char msg[32];
        sprintf(msg, "Temperature: %d C\r\n", last_temp);
        uart_print(msg);
    }

    if (display_mode == SHOW_HUM || display_mode == SHOW_BOTH) {
        char msg[32];
        sprintf(msg, "Humidity: %d %%\r\n", last_hum);
        uart_print(msg);
    }

    // Check for panic conditions
    if (last_temp > 35 || last_hum > 80) {
        panic_count++;
        if (panic_count >= 3) {
            uart_print("System reset: High readings detected!\r\n");
            delay_ms(100); // Give time for UART to send
            NVIC_SystemReset();
        }
    } else {
        panic_count = 0; // Reset panic counter when values are normal
    }

    // Handle LED blinking logic
    handle_led_blinking();
    
    // LED blinking counter (for 1 second blink in Alert mode)
    static uint16_t blink_counter = 0;
    if (led_blink_active) {
        blink_counter++;
        // Assuming timer runs every read_interval seconds, we need to blink every second
        // So we toggle LED every (1000ms / (read_interval * 1000ms)) calls
        if (blink_counter >= (1000 / (read_interval * 1000))) {
            gpio_toggle(LED_PIN);
            blink_counter = 0;
        }
    }
}

int main() {
    // Initialize peripherals
    queue_init(&rx_queue, BUFF_SIZE);
    uart_init(115200);
    uart_set_rx_callback(uart_rx_isr);
    uart_enable();
	
    // Initialize timer
		timer_init(1000000 * read_interval);
		timer_set_callback(timer_callback);
		timer_enable();

    // Initialize GPIO
    gpio_set_mode(LED_PIN, Output);
    gpio_set(LED_PIN, 0); // Start with LED off

    gpio_set_mode(TOUCH_PIN, PullDown);
    gpio_set_trigger(TOUCH_PIN, Rising);
    gpio_set_callback(TOUCH_PIN, touch_callback);

    // Enable interrupts
    __enable_irq();

    // Request password and show menu
    request_password();

    // Main loop
    while (1) {
        if (command_ready) {
            command_ready = 0;
            read_line(input_buffer, BUFF_SIZE);

            if (strcmp(input_buffer, "status") == 0) {
                print_status();
            } else if (strlen(input_buffer) == 1) {
                process_command(input_buffer[0]);
            } else if (strlen(input_buffer) > 0) {
                uart_print("Unknown command. Use a, b, c, d or 'status'\r\n");
            }
        }

        // Wait for interrupt - low power mode
        __WFI();
    }

    return 0;
}