
entry
addi r14,r0,topaddr
addi r1, r0, 1214
sw 0(r14), r1
addi r1, r0, 3
sw -4(r14), r1
lw r1, 0(r14)
sw -8(r14), r1
lw r1, -4(r14)
sw -12(r14), r1

%write stat
lw r1, -8(r14)

addi r14, r14, -16
sw -8(r14), r1
addi r1, r0, buff
sw -12(r14), r1
jl r15, intstr
sw -8(r14), r13
jl r15, putstr

subi r14, r14, -16
addi r1, r0, 42
putc r1
addi r1, r0, 49
putc r1
addi r1, r0, 48
putc r1
addi r1, r0, 94
putc r1
addi r1, r0, 0
putc r1
lw r1, -12(r14)

addi r14, r14, -16
sw -8(r14), r1
addi r1, r0, buff
sw -12(r14), r1
jl r15, intstr
sw -8(r14), r13
jl r15, putstr

subi r14, r14, -16
hlt
buff res 20
