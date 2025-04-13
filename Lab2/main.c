#include "platform.h"
#include <stdio.h>
#include <stdint.h>
#include "uart.h"
#include <string.h>
#include "queue.h"
#include "gpio.h"
#include "leds.h"
#include "timer.h"


#define BUFF_SIZE 10 //read buffer length
#define TIME_INTERVAL 500 
Queue rx_queue; // Queue for storing received characters
volatile uint8_t led_state = gpio_get(LED_PIN, 0);


// Interrupt Service Routine for UART receive
void uart_rx_isr(uint8_t rx) {
	// Check if the received character is a desired number
	if (rx >= '0' && rx <= '9' ) {
		// Store the received character
		queue_enqueue(&rx_queue, rx);
	}
	else if (rx == '\r'){
		queue_enqueue(&rx_queue, rx);
	}
}

void toggle_led(){
	led_state = !led_state;
	gpio_set(LED_PIN, led_state);
}

int main() {
		
	// Variables to help with UART read
	uint8_t rx_char = 0;
	char buff[BUFF_SIZE]; // The UART read string will be stored here
	uint32_t buff_index;
	
	// Initialize the receive queue and UART
	queue_init(&rx_queue, 128);
	uart_init(115200);
	uart_set_rx_callback(uart_rx_isr); // Set the UART receive callback function
	uart_enable(); // Enable UART module
	
	 timer_init(TIME_INTERVAL);
   gpio_set_mode(LED_PIN, Output); 
	
	__enable_irq(); // Enable interrupts
	uart_print("Enter an integer number:\r\n");// Print newline
	
	while(1) {

		// Prompt the user to enter their full name
		buff_index = 0; // Reset buffer index
		
		do {
			// Wait until a character is received in the queue
			while (!queue_dequeue(&rx_queue, &rx_char))
				__WFI(); // Wait for Interrupt

			if (rx_char == 0x7F) { // Handle backspace character
				if (buff_index > 0) {
					buff_index--; // Move buffer index back
					uart_tx(rx_char); // Send backspace character to erase on terminal
				}
			} else {
				// Store and echo the received character back
				buff[buff_index++] = (char)rx_char; // Store character in buffer
				uart_tx(rx_char); // Echo character back to terminal
			}
		} while (rx_char != '\r' && buff_index < BUFF_SIZE); // Continue until Enter key or buffer full
		
		// Replace the last character with null terminator to make it a valid C string
		buff[buff_index - 1] = '\0';
		uart_print("\r\n"); // Print newline
		
		// Check if buffer overflow occurred
		if (buff_index >= BUFF_SIZE) {
			uart_print("Stop trying to overflow my buffer! I resent that!\r\n");
		}
	}
}
