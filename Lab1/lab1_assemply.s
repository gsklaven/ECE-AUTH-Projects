.section .text
.global hashing
.p2align 2

.type hashing,%function
hashing:
    .fnstart
    mov r1, #0       // r1 = 0, counter (string length)
    loop0:
        ldrb r3, [r0], #1 // Load byte from r0, increment r0 by 1
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
    push {lr} //
    push {r0, r1} // Push the values before hashing changes them
    bl hashing // Call the hashing function
    mov r4, r0  // Move hashing output to r4
    pop {r0, r1} // Restore r0, r1
    mov r3, r0 // Move string address to r3
    loop1:
        ldrb r2, [r3], #1  // Load next character
        cmp r2, #0       
        beq end_compare   // If null terminator, exit

        cmp r2, #'0'                                                    //allagh
		blt next_char
		cmp r2, #'0'
		IT le
		addle r4,#5
		cmp r2, #'0'
		ble next_char
		cmp r2, #'1'
		IT le
		addle r4,#12
		cmp r2, #'1'
		ble next_char
		cmp r2, #'2'
		IT le
		addle r4,#7
		cmp r2, #'2'
		ble next_char
		cmp r2, #'3'
		IT le
		addle r4,#6
		cmp r2, #'3'
		ble next_char
		cmp r2, #'4'
		IT le
		addle r4,#4
		cmp r2, #'4'
		ble next_char
		cmp r2, #'5'
		IT le
		addle r4,#11
		cmp r2, #'5'
		ble next_char
		cmp r2, #'6'
		IT le
		addle r4,#6
		cmp r2, #'6'
		ble next_char
		cmp r2, #'7'
		IT le
		addle r4,#3
		cmp r2, #'7'
		ble next_char
		cmp r2, #'8'
		IT le
		addle r4,#10
		cmp r2, #'8'
		ble next_char
		cmp r2, #'9'
		IT le
		addle r4,#23
		cmp r2, #'9'
		ble next_char                                                           //allagh

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
        add r0, r2, r2    
        add r4, r4, r0    
        b loop1           // Continue processing next character

    lowercase:
        sub r0, r2, #'a'  // r0 = ASCII - 97
        mul r0, r0, r0    // r0 = (ASCII - 97)^2
        add r4, r4, r0    
        b loop1           // Continue processing next character

    end_compare:
        mov r7, r4                                                            //allagh
        mov r0, r4       
        pop {lr}         
        bx lr            
    .fnend

.section .text
.global modulowith7
.p2align 2

.type modulowith7,%function
modulowith7:
    .fnstart
    push {lr}          // Save return address
    bl asciicompare    // Call asciicompare to get hash
    mov r4, r0         // Store hash value in r4

reduce_loop:
    mov r5, r4         // Store the current value in r5
    mov r1, #7         // Load divisor (7)
    
    sdiv r2, r5, r1    // r2 = r5 / 7 (quotient)
    mul r2, r2, r1     // r2 = (r5 / 7) * 7
    sub r4, r5, r2     // r4 = r5 % 7

    cmp r4, #9         // If the result is still > 9, repeat
    bgt reduce_loop    

    mov r0, r4         // Return final result
    pop {lr}           // Restore return address
    bx lr
    .fnend

.section .data
.global result
result: .word 4

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
    ldr r3, =result    // Load address of result
    str r1, [r3]       // Store the result in memory
    pop {lr}
    bx lr

fib_return0:
    mov r0, #0         // Return F(0)
    ldr r3, =result    // Load address of result
    str r0, [r3]       // Store the result in memory
    pop {lr}
    bx lr

fib_return1:
    mov r0, #1         // Return F(1)
    ldr r3, =result    // Load address of result
    str r0, [r3]       // Store the result in memory
    pop {lr}
    bx lr
    .fnend

.section .text                                                   //allahgh
.global addhash
.p2align 2

.type addhash,%function
	addhash:
	.fnstart
	push {lr}
	mov r0, r7
	pop {lr}
	bx lr
	.fnend                                                      //allagh
	
