
entry
addi r14,r0,topaddr

addi r1, r0, 1
sw -16(r14), r1

%assignment
addi r1, r0, 1
sw 0(r14), r1

addi r1, r0, 3
sw -24(r14), r1

%assignment
addi r1, r0, 3
sw -4(r14), r1

addi r1, r0, 2
sw -32(r14), r1

%assignment
addi r1, r0, 2
sw -8(r14), r1

addi r2, r14, 0
lw r1, 0(r2)
addi r3, r14, 0
lw r2, -4(r3)
add r3, r1, r2
sw -44(r14), r3

lw r1, -44(r14)
addi r3, r14, 0
lw r2, -8(r3)
add r3, r1, r2
sw -52(r14), r3


lw r1, -44(r14)
addi r3, r14, 0
lw r2, -8(r3)
add r3, r1, r2
sw -52(r14), r3
lw r1, -52(r14)
sw -56(r14), r1

%write stat

lw r1, -44(r14)
addi r3, r14, 0
lw r2, -8(r3)
add r3, r1, r2
sw -52(r14), r3
lw r1, -52(r14)

addi r14, r14, -60
sw -8(r14), r1
addi r1, r0, buff
sw -12(r14), r1
jl r15, intstr
sw -8(r14), r13
jl r15, putstr

subi r14, r14, -60
hlt
buff res 20
