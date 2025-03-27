.global hashing
.p2align 2

.type hashing,%function
hashing:
    .fnstart
    mov r1, #0       // r1 = 0, counter (string length)
	mov r2, r0
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
	ldrb r2, [r3], #1 
    cmp r2, #0       
    beq end_compare

    cmp r2, #'A'     
    blt loop1         

    cmp r2, #'Z'      
    bgt loop1         

    add r0, r2, r2    
    add r4, r4, r0    
    b loop1        
end_compare:
    mov r0, r4       
    pop {lr}   
    bx lr  
    .fnend
	
	