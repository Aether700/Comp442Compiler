entry
addi r14,r0,topaddr

%greater or equal
addi r1, r0, 7
addi r2, r0, 8
cge r3, r1, r2
sw 0(r14), r3

lw r1, 0(r14)

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
