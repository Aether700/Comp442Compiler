
entry
addi r14,r0,topaddr
addi r1, r0, 051
sw -16(r14), r1
addi r1, r0, 1
sw -20(r14), r1

%assignment
addi r1, r0, 051
sw 0(r14), r1
addi r1, r0, 1
sw -4(r14), r1
addi r1, r0, 05
sw -24(r14), r1
addi r1, r0, 1
sw -28(r14), r1

%assignment
addi r1, r0, 05
sw -8(r14), r1
addi r1, r0, 1
sw -12(r14), r1

% adding floats

% find smallest exponent
lw r1, -4(r14)
lw r2, -12(r14)
% clt r3, r1, 2
bz r3, memAddress0
sub r4, r1, r2
lw r6, 0(r14)
lw r5, -8(r14)
sw -36(r14), r2
j memAddress1
memAddress0 sub r4, r2, r1
lw r6, -8(r14)
lw r5, 0(r14)
sw -36(r14), r1

% adjust mantissa
memAddress1 addi r7, r0, 0
memAddress2 clt r8, r7, r4
bz r8, memAddress3
divi r6, r6, 10
j memAddress2
memAddress3 
% perform operation
add r9, r5, r6

% compact result
memAddress4 modi r10, r9, 10
bnz r10, memAddress5
divi r9, r9, 10
j memAddress4
memAddress5 sw -32(r14), r9
lw r1, -32(r14)
sw -40(r14), r1
lw r1, -36(r14)
sw -44(r14), r1

%write stat
lw r1, -40(r14)

addi r14, r14, -48
sw -8(r14), r1
addi r1, r0, buff
sw -12(r14), r1
jl r15, intstr
sw -8(r14), r13
jl r15, putstr

subi r14, r14, -48
addi r1, r0, 42
putc r1
addi r1, r0, 49
putc r1
addi r1, r0, 48
putc r1
addi r1, r0, 94
putc r1
lw r1, -44(r14)

addi r14, r14, -48
sw -8(r14), r1
addi r1, r0, buff
sw -12(r14), r1
jl r15, intstr
sw -8(r14), r13
jl r15, putstr

subi r14, r14, -48
hlt
buff res 20
