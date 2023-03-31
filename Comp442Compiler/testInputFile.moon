
entry
addi r14,r0,topaddr
addi r1, r14, 0
sw -4(r14), r1

% read user input

addi r14, r14, -16
addi r1, r0, buff
sw -8(r14), r1
jl r15, getstr
addi r1, r0, buff
sw -8(r14), r1
jl r15, strint
add r1, r0, r13

subi r14, r14, -16
lw r1, -4(r14)

% copy data at reference
sw 0(r1), r13
addi r1, r14, 0
sw -8(r14), r1

lw r1, 0(r14)
sw -12(r14), r1

%write stat
lw r1, 0(r14)

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
