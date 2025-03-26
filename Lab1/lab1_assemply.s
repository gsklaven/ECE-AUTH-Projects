.global hashing
.p2align 2

.type hashing,%function
hashing:
    .fnstart
    mov r1, #0       // r1 = 0, counter (string length)
	mov r3, r0
loop0:
    ldrb r2, [r0], #1 // Load byte from r0, increment r0 by 1
    cmp r2, #0        // Check if null terminator
    beq hashing_start // If null, go to hashing_start
    add r1, r1, #1    // Increase counter (string length)
    b loop0            // Repeat loop
hashing_start:
    mov r4, r1        // Move hash value to return global variable (r4)
	bx lr             // Return
	.fnend
	
	
.global asciicompare
.p2align 2

.type asciicompare,%function
asciicompare:
    .fnstart
	mov r0, #'A'
loop1:
	cmp r3, r0
	beq asciiuppercase
	add r0, r0, #1
	b loop1
	
asciiuppercase:
	mov r1, #2
	mul r0, r0, r1
	bl hashing //Call function
	add r4, r4, r0
	bx lr
	.fnend
	