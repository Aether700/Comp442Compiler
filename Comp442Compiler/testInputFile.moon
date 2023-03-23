
entry
addi r14,r0,topaddr

%equals
addi r1, r0, 1
addi r2, r0, 1
ceq r3, r1, r2
sw 0(r14), r3

%if statement
lw r1, 0(r14)
bz r1, memAddress0

%equals
addi r1, r0, 1
addi r2, r0, 1
ceq r3, r1, r2
sw 0(r14), r3

addi r1, r0, 1

addi r14, r14, -24
sw -8(r14), r1
addi r1, r0, buff
sw -12(r14), r1
jl r15, intstr
sw -8(r14), r13
jl r15, putstr

subi r14, r14, -24
j memAddress1
memAddress0 
addi r1, r0, 0

addi r14, r14, -24
sw -8(r14), r1
addi r1, r0, buff
sw -12(r14), r1
jl r15, intstr
sw -8(r14), r13
jl r15, putstr

subi r14, r14, -24
memAddress1 
addi r2, r0, 2

addi r14, r14, -24
sw -8(r14), r2
addi r2, r0, buff
sw -12(r14), r2
jl r15, intstr
sw -8(r14), r13
jl r15, putstr

subi r14, r14, -24
hlt
buff res 20
