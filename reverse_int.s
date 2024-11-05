reverse_int:

    //Divide los bits entre par e impar y los mueve
    mov    r1, #0xaaaaaaaa
    and    r2, r0, r1
    lsr    r2, r2, #1
    mov    r1, #0x55555555
    and    r3, r0, r1
    lsl    r3, r3, #1
    orr    r0, r2, r3

    //Divide los bits en pares y los mueve
    mov    r1, #0xcccccccc
    and    r2, r0, r1
    lsr    r2, r2, #2
    mov    r1, #0x33333333
    and    r3, r0, r1
    lsl    r3, r3, #2
    orr    r0, r2, r3

    //Divide los bits en 4 bits y los mueve
    mov    r1, #0xf0f0f0f0
    and    r2, r0, r1
    lsr    r2, r2, #4
    mov    r1, #0x0f0f0f0f
    and    r3, r0, r1
    lsl    r3, r3, #4
    orr    r0, r2, r3

    //Divide los bits en 8 bits y los mueve
    mov    r1, #0xff00ff00
    and    r2, r0, r1
    lsr    r2, r2, #8
    mov    r1, #0x00ff00ff
    and    r3, r0, r1
    lsl    r3, r3, #8
    orr    r0, r2, r3

    //Divide los bits en 16 bits y los mueve
    mov    r2, r0, lsr #16
    lsl    r0, r0, #16
    orr    r0, r0, r2

    bx     lr
