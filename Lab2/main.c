#include "platform.h"
#include <stdio.h>
#include <stdint.h>
#include "uart.h"
#include <string.h>
#include "queue.h"
#include "gpio.h"
#include "timer.h"

#define BUFF_SIZE 128 // Read buffer length
#define P_LED PA_5
#define BUTTON_PIN PC_13 //Button input pin
Queue rx_queue; // Queue for storing received characters

volatile uint32_t current_digit_index = 0; // Index of the current digit being processed
volatile uint8_t process_digits = 0;         // Flag to start/stop digit processing
volatile uint8_t repeat_analysis = 0;          // Flag to repeat analysis when number ends with '-'
volatile uint8_t new_input_detected = 0;       // Flag to indicate a new input has been detected

char buff[BUFF_SIZE]; // Buffer to store user input
volatile uint8_t led_locked = 0;             // Lock flag for LED logic
volatile uint32_t button_press_count = 0;    // Counts button presses
//  Button interrupt callback
void button_callback(int status) {
    if (status & (1 << GET_PIN_INDEX(BUTTON_PIN))) {  // Check if button was pressed
        button_press_count++;                             // Toggle lock state
        led_locked ^= 1;                             // Toggle lock state

        uart_print("Interrupt: Button pressed. LED ");  // Report current state
        if (led_locked)
            uart_print("locked. Count = ");
        else
            uart_print("unlocked. Count = ");

        char count_str[10];
        sprintf(count_str, "%u\r\n", button_press_count);  // Print count
        uart_print(count_str);
    }
}                                                       

// UART receive interrupt service routine
void uart_rx_isr(uint8_t rx) {
    // Process valid characters in one if statement
    if ((rx >= '0' && rx <= '9') || rx == '-' || rx == '\r') {
        // If processing is active and a digit or '-' arrives, abort current processing.
        if (process_digits && ((rx >= '0' && rx <= '9') || rx == '-')) {
            process_digits = 0;
            timer_disable();
            current_digit_index = 0;
            new_input_detected = 1;
        }

        // Enqueue the valid character.
        queue_enqueue(&rx_queue, rx);
        
        // Set the repeat flag if the character is '-'
        if (rx == '-') {
            repeat_analysis = 1;
        }
    }
    // All other characters are ignored
}

// Timer interrupt handler for digit analysis every 0.5 sec
void TIM_IRQHandler() {
    // Abort current processing if a new input is detected
    if (new_input_detected) {
        new_input_detected = 0;
        process_digits = 0;
        timer_disable();
        gpio_set(P_LED, 0);
        return;
    }

    if (process_digits) {
        if (current_digit_index < strlen(buff)) {
            uint8_t current_char = buff[current_digit_index];
            
            if (current_char == '-') {
                current_digit_index++;
                return;
            }
            
            // Process only if the character is a digit (ignore '-' for digit analysis)
            if (current_char >= '0' && current_char <= '9') {
                uint8_t digit = current_char - '0'; // Convert ASCII to integer

                // Print the action to UART
                uart_print("Digit ");
                uart_tx(current_char);
                uart_print(": ");
                    if (led_locked) {                                      // LED is locked
                        uart_print("LED action skipped due to lock\r\n"); 
                    } else {                                               
                    if (digit % 2 == 0) {
                        // Even digit: LED blink (200ms ON then 200ms OFF)
                        gpio_set(P_LED, 1); // LED ON
                        for (volatile int i = 0; i < 200000; ++i); // Delay ~200ms
                        gpio_set(P_LED, 0); // LED OFF
                        for (volatile int i = 0; i < 200000; ++i); // Delay ~200ms
                        uart_print("LED blink (200ms ON-OFF)\r\n");
                    } else {
                        // Odd digit: LED toggle and keep the new state
                        gpio_toggle(P_LED);
                        uart_print("LED toggled\r\n");
                    }// !!!!! 
                }
            }
            current_digit_index++;
        } else {
            // Completed processing all digits in the current input
            timer_disable();
            gpio_set(P_LED, 0);
            // If the input ends with '-', repeat analysis for the same input
            if (repeat_analysis) {
                current_digit_index = 0;
                timer_enable();
            } else {
                process_digits = 0;
            }
        }
    }
}

int main() {
    uint8_t rx_char = 0;
    uint32_t buff_index;

    // Initialize the queue, UART, and GPIO
    queue_init(&rx_queue, 128);
    uart_init(115200);
    uart_set_rx_callback(uart_rx_isr);
    uart_enable();
    gpio_set_mode(P_LED, Output);
    gpio_set(P_LED, 0); // LED OFF initially
    timer_init(500000); // Timer with 0.5 sec interval
    timer_set_callback(TIM_IRQHandler);
    gpio_set_mode(BUTTON_PIN, Input);                     //  Configure button pin as input
    gpio_set_trigger(BUTTON_PIN, Falling);                // Trigger on falling edge (button press)
    gpio_set_callback(BUTTON_PIN, button_callback);       // Register callback
    __enable_irq(); // Enable global interrupts

    uart_print("\r\n");

    while (1) {
        // Prompt user for number input
        uart_print("Enter a number: ");
        buff_index = 0;

        // Read characters until carriage return is encountered
        do {
            while (!queue_dequeue(&rx_queue, &rx_char))
                __WFI(); // Wait for interrupt

            if (rx_char != '\r') {
                if (buff_index < BUFF_SIZE - 1) {
                    buff[buff_index++] = (char)rx_char;
                    uart_tx(rx_char);
                } else {
                    uart_print("\r\nInput too long, try again.\r\n");
                    buff_index = 0;
                    break;
                }
            }
        } while (rx_char != '\r');

        buff[buff_index] = '\0'; // Null-terminate the string
        uart_print("\r\n");

        // If no input was provided, restart the loop for new input
        if (buff_index == 0)
            continue;

        // Set repeat_analysis flag based on the last character in the input
        if (buff[buff_index - 1] == '-') {
            repeat_analysis = 1;
        } else {
            repeat_analysis = 0;
        }

        // Start processing the input number
        process_digits = 1;
        current_digit_index = 0;
        new_input_detected = 0; // Reset new input flag
        timer_enable();

        // Wait until processing is completed or aborted due to new input
        while (process_digits) {
            __WFI();
        }
    }

    return 0;
}
