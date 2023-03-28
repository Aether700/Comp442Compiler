
entry
addi r14,r0,topaddr
addi r1, r0, 05
sw 0(r14), r1
addi r1, r0, 0
sw -4(r14), r1
addi r1, r0, 056
sw -8(r14), r1
addi r1, r0, 0
sw -12(r14), r1

% subtracting floats

% find smallest exponent
lw r1, -4(r14)
lw r2, -12(r14)
clt r3, r1, r2
bz r3, memAddress0
sub r4, r1, r2
lw r6, 0(r14)
lw r5, -8(r14)
sw -16(r14), r0
j memAddress1
memAddress0 sub r4, r2, r1
lw r6, -8(r14)
lw r5, 0(r14)
addi r10, r0, 1
sw -16(r14), r10

% adjust mantissa
memAddress1 addi r7, r0, 0
memAddress2 clt r8, r7, r4
bz r8, memAddress3
divi r6, r6, 10
addi r7, r7, 1
j memAddress2
memAddress3 
% adjust mantissa position

% count num digits
add r2, r0, r5
addi r1, r0, 1
divi r2, r2, 10
memAddress4 bz r2, memAddress5
divi r2, r2, 10
addi r1, r1, 1
j memAddress4

memAddress5 
% count num digits
add r10, r0, r6
addi r2, r0, 1
divi r10, r10, 10
memAddress6 bz r10, memAddress7
divi r10, r10, 10
addi r2, r2, 1
j memAddress6

memAddress7 sub r4, r2, r1
sw -20(r14), r2
clt r3, r4, r0
bz r3, memAddress8
muli r4, r4, -1
sw -20(r14), r1
memAddress8 
% sort what mantissa to use for operation
clt r3, r2, r1
bz r3, memAddress9
add r1, r0, r6
add r2, r0, r5
lw r10, -16(r14)
not r10, r10
sw -16(r14), r10
j memAddress10
memAddress9 add r1, r0, r6
add r2, r0, r5

% multiply shortest mantissa to adjust it's position
memAddress10 addi r7, r0, 0
memAddress11 clt r8, r7, r4
bz r8, memAddress12
muli r1, r1, 10
addi r7, r7, 1
j memAddress11
memAddress12 lw r10, -16(r14)
bz r10, memAddress13
add r10, r0, r1
add r1, r0, r2
add r2, r0, r10
memAddress13 
% perform operation
muli r1, r1, 100
muli r2, r2, 100
sub r9, r1, r2

% compact result
addi r1, r0, 2
memAddress14 modi r10, r9, 10
bnz r10, memAddress15
bz r9, memAddress15
divi r9, r9, 10
subi r1, r1, 1
j memAddress14

% compact exponent if mantissa is 0
memAddress15 bnz r9, memAddress16
lw r1, -16(r14)
muli r1, r1, -1

% store the result
memAddress16 sw -16(r14), r9
lw r9, -20(r14)
add r9, r9, r1
sw -20(r14), r9
lw r3, -16(r14)
sw -24(r14), r3
lw r3, -20(r14)
sw -28(r14), r3

%write stat
lw r3, -24(r14)

addi r14, r14, -32
sw -8(r14), r3
addi r3, r0, buff
sw -12(r14), r3
jl r15, intstr
sw -8(r14), r13
jl r15, putstr

subi r14, r14, -32
addi r3, r0, 42
putc r3
addi r3, r0, 49
putc r3
addi r3, r0, 48
putc r3
addi r3, r0, 94
putc r3
lw r3, -28(r14)

addi r14, r14, -32
sw -8(r14), r3
addi r3, r0, buff
sw -12(r14), r3
jl r15, intstr
sw -8(r14), r13
jl r15, putstr

subi r14, r14, -32
hlt
buff res 20
