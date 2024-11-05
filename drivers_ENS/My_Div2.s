    .syntax unified
    .thumb
    .global My_Div2

My_Div2:
    movs r2, r0
    movs r3, r1
    movs r0, #0

loop_start:
    cmp r2, r3
    blt loop_end
    subs r2, r2, r3
    adds r0, r0, #1
    b loop_start

loop_end:
    bx lr