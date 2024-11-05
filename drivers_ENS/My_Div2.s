    .syntax unified                // Use unified syntax
    .thumb                         // Specify Thumb mode (needed for Cortex-M0+)
    .global My_Div2                // Make the function accessible from other files

My_Div2:
    movs r2, r0                    // Move dividend (a) from r0 to r2
    movs r3, r1                    // Move divisor (b) from r1 to r3
    movs r0, #0                    // Initialize quotient (c) to 0 in r0

loop_start:
    cmp r2, r3                     // Compare remaining dividend with divisor
    blt loop_end                   // If dividend < divisor, exit loop
    subs r2, r2, r3                // Subtract divisor from remaining dividend
    adds r0, r0, #1                // Increment quotient
    b loop_start                   // Repeat loop

loop_end:
    bx lr                          // Return to caller with quotient in r0