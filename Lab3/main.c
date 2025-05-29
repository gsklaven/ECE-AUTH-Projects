#include "platform.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "uart.h"
#include "queue.h"
#include "gpio.h"
#include "timer.h"
#include "delay.h"
#include "dht11.h"

// === Definitions ===
#define BUFF_SIZE 128
#define LED_PIN PA_5
#define TOUCH_PIN PC_13
#define DHT11_PIN PB_10
#define PASSWORD "1234"

// === Variables ===
char aem_str[10];
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

// === Prototypes ===
void timer_callback(void);
void print_status(void);
void process_command(char cmd);
void request_password(void);
void read_sensor(void);
void touch_callback(int status);
void read_line(char *buffer, int max_len);

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
    char msg[128];
    sprintf(msg,
        "Mode: %s\r\nTemperature: %d C\r\nHumidity: %d %%\r\nProfile changes: %d\r\n",
        (current_mode == MODE_A) ? "Normal" : "Alert",
        last_temp, last_hum, profile_changes
    );
    uart_print(msg);
}

void process_command(char cmd) {
    switch (cmd) {
        case 'a':
            if (read_interval > 2) read_interval--;
            break;
        case 'b':
            if (read_interval < 10) read_interval++;
            break;
        case 'c':
            display_mode = (display_mode + 1) % 3;
            break;
        case 'd':
            print_status();
            break;
    }

    timer_disable();
    timer_init(1000000 * read_interval);
    timer_set_callback(timer_callback);
    timer_enable();
}

void request_password() {
    char input[BUFF_SIZE];
    timer_disable();
    while (1) {
        uart_print("Enter password: ");
        read_line(input, BUFF_SIZE);
        if (strcmp(input, PASSWORD) == 0)
            break;
        uart_print("Wrong password. Try again.\r\n");
    }
    uart_print("Access granted.\r\n");
    uart_print("Enter your AEM: ");
    read_line(aem_str, BUFF_SIZE);

    size_t len = strlen(aem_str);
    if (len >= 2) {
        aem_digit1 = aem_str[len - 2] - '0';
        aem_digit2 = aem_str[len - 1] - '0';
    }

    uart_print(
        "=== Environmental Monitoring System ===\r\n"
        "Available commands:\r\n"
        "a: Increase sampling frequency (-1s)\r\n"
        "b: Decrease sampling frequency (+1s)\r\n"
        "c: Change display mode\r\n"
        "d: Show system status\r\n"
    );
    timer_init(1000000 * read_interval);
    timer_set_callback(timer_callback);
    timer_enable();
}

void touch_callback(int status) {
    (void)status;
    touch_count++;
    profile_changes++;

    current_mode = (current_mode == MODE_A) ? MODE_B : MODE_A;

    if (touch_count % 3 == 0) {
        read_interval = aem_digit1 + aem_digit2;
        if (read_interval < 2) read_interval = 2;
        if (read_interval > 10) read_interval = 10;

        timer_disable();
        timer_init(1000000 * read_interval);
        timer_set_callback(timer_callback);
        timer_enable();
    }
}

void read_sensor() {
    float* measurements = getData();
    if(measurements != NULL){
        last_hum  = (uint8_t)measurements[0];
        last_temp = (uint8_t)measurements[1];
    }
    else {
        uart_print("DHT11 read error\r\n");
    }
}

void timer_callback(void) {
    read_sensor();

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

    if (last_temp > 35 || last_hum > 80) {
        panic_count++;
        if (panic_count >= 3) {
            uart_print("System reset: High readings detected!\r\n");
            NVIC_SystemReset();
        }
    } else {
        panic_count = 0;
    }

    if (current_mode == MODE_B) {
        if (last_temp > 25 || last_hum > 60) {
            gpio_toggle(LED_PIN);
            safe_count = 0;
        } else {
            safe_count++;
            if (safe_count >= 5) {
                gpio_set(LED_PIN, 0);
            }
        }
    }
}

int main() {
    queue_init(&rx_queue, BUFF_SIZE);
    uart_init(115200);
    uart_set_rx_callback(uart_rx_isr);
    uart_enable();
	
    timer_init(1000000);
    timer_set_callback(timer_callback);
    timer_enable();

    gpio_set_mode(LED_PIN, Output);
    //gpio_set(LED_PIN, 0);

    gpio_set_mode(TOUCH_PIN, PullDown);
    gpio_set_trigger(TOUCH_PIN, Rising);
    gpio_set_callback(TOUCH_PIN, touch_callback);

    __enable_irq();

    request_password();

    while (1) {
        if (command_ready) {
            command_ready = 0;
            read_line(input_buffer, BUFF_SIZE);

            if (strcmp(input_buffer, "status") == 0) {
                print_status();
            } else if (strlen(input_buffer) == 1) {
                process_command(input_buffer[0]);
            }
        }

        __WFI(); // Wait for interrupt - low power mode
    }

    return 0;
}