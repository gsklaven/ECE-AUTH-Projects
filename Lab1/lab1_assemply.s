.global hashing
.p2align 2

.type hashing,%function
hashing:
    .fnstart
    mov r1, #0       // r1 = 0, counter (string length)
loop:
    ldrb r3, [r0], #1 // Load byte from r0, increment r0 by 1
    cmp r3, #0        // Check if null terminator
    beq hashing_start // If null, go to hashing_start
    add r1, r1, #1    // Increase counter (string length)
    b loop            // Repeat loop
hashing_start:
    mov r0, r1        // Move hash value to return register (r0)
	bx lr             // Return
	.fnend
	
	
.global asciicompare
.p2align 2

.type asciicompare,%function
asciicompare:
    .fnstart
	