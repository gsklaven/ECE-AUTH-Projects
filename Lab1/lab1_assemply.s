.section .data
.global result0
result0: .word 4

.section .text
.global hashing
.p2align 2

.type hashing,%function
hashing:
    .fnstart
    mov r1, #0       // r1 = 0, counter (string length)
	mov r2, r0 //move r0 string address to r2 to save it
    loop0:
        ldrb r3, [r2], #1 // Load byte from r2, increment r2 by 1
        cmp r3, #0        // Check if null terminator
        beq hashing_start // If null, go to hashing_start
        add r1, r1, #1    // Increase counter (string length)
        b loop0            // Repeat loop
    hashing_start:
        mov r0, r1        // Move hash value to return register (r0)
        bx lr             // Return
    .fnend

.section .text
.global asciicompare
.p2align 2

.type asciicompare,%function
asciicompare:
    .fnstart
    push {r0, lr}    // Save r0 (string pointer) and lr (return address)
    bl hashing       // Call the hashing function
    mov r1, r0       // Store hashing output in r1
    pop {r0, lr}     // Restore r0 (original string pointer) and lr
    mov r3, r0       // Copy string address to r3
    mov r4, #0       // Initialize r4 (accumulator) to 0

loop1:
    ldrb r2, [r3], #1  // Load next character
    cmp r2, #0         // Check for null terminator
    beq end_compare    // If null terminator, exit

    cmp r2, #'A'       
    blt next_char      // If less than 'A', skip
    cmp r2, #'Z'       
    ble uppercase      // If between 'A' and 'Z', go to uppercase

    cmp r2, #'a'
    blt next_char      // If less than 'a', skip
    cmp r2, #'z'
    ble lowercase      // If between 'a' and 'z', go to lowercase

next_char:
    b loop1            // Continue loop for next character

uppercase:
    add r2, r2, r2     // Multiply ASCII value by 2 for uppercase letter
    add r4, r4, r2     // Add to accumulator
    b loop1            // Continue processing next character

lowercase:
    sub r2, r2, #'a'   // Convert 'a'-'z' range to 0-25
    mul r2, r2, r2     // Square the result
    add r4, r4, r2     // Add to accumulator
    b loop1            // Continue processing next character

end_compare:
    add r0, r1, r4     // Final result: hashing output (r1) + accumulated result (r4)
    bx lr              // Return
    .fnend


.section .data
.global digit_table

digit_table: 
    .word 5   // Value for '0'
    .word 12  // Value for '1'
    .word 7   // Value for '2'
    .word 6   // Value for '3'
    .word 4   // Value for '4'
    .word 11  // Value for '5'
    .word 6   // Value for '6'
    .word 3   // Value for '7'
    .word 10  // Value for '8'
    .word 23  // Value for '9'

.section .text
.global numberchange
.p2align 2

.type numberchange, %function
numberchange:
    .fnstart
    push {r0, lr}
    bl asciicompare        // r0 = ascii-based hash
    mov r4, r0             // Keep the ascii-based hash in r4
    pop {r0, lr}               // Restore the string pointer into r3
	mov r3, r0
number_loop:
    ldrb r2, [r3], #1      // Load next character from the *actual* string
    cmp r2, #0             // Check for null terminator
    beq end_numberchange

    cmp r2, #'0'
    blt skip_char
    cmp r2, #'9'
    bgt skip_char

	sub r2, r2, #'0' //Transform the the non-ASCII number by removing the ASCII 0
    ldr r5, =digit_table   
    ldr r6, [r5, r2, LSL #2] //Logical Shift Left for the next number in the table
    add r4, r4, r6         // Add the digit-table value

skip_char:
    b number_loop

end_numberchange:
    mov r0, r4
	ldr r3, =result0
	str r0, [r3]
    bx lr
    .fnend

.section .data
.global result1
result1: .word 4

.section .text                                                   
.global addhash
.p2align 2

.type addhash,%function
addhash:
    .fnstart
    push {r0, lr}
    bl numberchange      
    mov r3, r0       
    mov r4, #0           // r4 = 0, initialize digit sum
    pop {r0, lr}
digit_sum_once:
    mov r1, #10 
    sdiv r2, r3, r1      // r2 = r3 / 10 (quotient)
    mul r1, r2, r1       // r1 = r2 * 10 
    sub r1, r3, r1       // r1 = r3 - r1 (last digit of r3)
    add r4, r4, r1       // Add the last digit to the sum
    mov r3, r2           // Update r3 to be the quotient (remaining number)
    cmp r3, #0           // Check if we have any more digits
    bne digit_sum_once   // If r3 is not zero, repeat

    mov r0, r4           // Return the final sum
    bx lr                // Return
    .fnend               


.section .text
.global modulowith7
.p2align 2

.type modulowith7,%function
modulowith7:
    .fnstart
    push {r0, lr}         
    bl addhash 
    mov r4, r0         // Store hash value in r4
	pop {r0, lr}
reduce_loop:
    mov r5, r4         // Store the current value in r5
    mov r1, #7         // Load divisor (7)
    
    sdiv r2, r5, r1    // r2 = r5 / 7 (quotient)
    mul r2, r2, r1     // r2 = (r5 / 7) * 7
    sub r4, r5, r2     // r4 = r5 - 7

    cmp r4, #9         // If the result is still > 9, repeat
    bgt reduce_loop    

    mov r0, r4         // Return final result
	ldr r3, =result1
	str r0, [r3]       
    bx lr
    .fnend

.section .data
.global result2
result2: .word 4

.section .text
.global fibonacci
.p2align 2

.type fibonacci,%function
fibonacci:
    .fnstart
    push {lr}          // Save return address
    bl modulowith7     // Call modulowith7 to get hash value in r4
    mov r4, r0         // Store hash value in r4

    mov r0, #0         // Initialize Fibonacci base case F(0)
    mov r1, #1         // Initialize Fibonacci base case F(1)

    cmp r4, #0         // Check if input is 0
    beq fib_return0

    cmp r4, #1         // Check if input is 1
    beq fib_return1

    mov r2, #2         // Start at F(2)

fibonacci_loop:
    add r3, r0, r1     // F(n) = F(n-1) + F(n-2)
    mov r0, r1         // Update F(n-2) to F(n-1)
    mov r1, r3         // Update F(n-1) to F(n)
    cmp r2, r4         // Check if counter matches the input
    beq fib_return     // Break loop if reached target
    add r2, r2, #1     // Increment the counter
    b fibonacci_loop   // Continue loop

fib_return:
    mov r0, r1         // Result is in r1
    ldr r3, =result2    // Load address of result
    str r1, [r3]       // Store the result in memory
    pop {lr}
    bx lr

fib_return0:
    mov r0, #0         // Return F(0)
    ldr r3, =result2    // Load address of result
    str r0, [r3]       // Store the result in memory
    pop {lr}
    bx lr

fib_return1:
    mov r0, #1         // Return F(1)
    ldr r3, =result2    // Load address of result
    str r0, [r3]       // Store the result in memory
    pop {lr}
    bx lr
    .fnend