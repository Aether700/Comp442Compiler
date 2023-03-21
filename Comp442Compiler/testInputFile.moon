entry
addi r14,r0,topaddr

addi r1, r0, 0
sw -4(r14), r1

addi r1, r0, 1
sw 0(r14), r1

lw r1, -4(r14)
lw r2, 0(r14)
add r3, r1, r2
bz r3, 4
addi r4, r0, 1
j 3
addi r4, r0, 0
lw r4, -8(r14)

lw r1, -8(r14)

addi r14, r14, -24
sw -8(r14), r1
addi r1, r0, buff
sw -12(r14), r1
jl r15, intstr
sw -8(r14), r13
jl r15, putstr

subi r14, r14, -24
hlt
buff res 20
