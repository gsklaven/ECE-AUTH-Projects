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
	
	
.global asciicompare
.p2align 2

.type asciicompare,%function
asciicompare:
    .fnstart
	push {lr} //
	push {r0, r1} //push the values before hashing changes them
	bl hashing //Call the function
	mov r4, r0  //Move hashing output to r4
	pop {r0, r1} //restore r0, r1
	mov r3, r0 //Move string address to r3
loop1:
    ldrb r2, [r3], #1  // Load next character
    cmp r2, #0       
    beq end_compare   // If null terminator, exit

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
    mov r0, r4       
    pop {lr}         
    bx lr            
    .fnend
	
	