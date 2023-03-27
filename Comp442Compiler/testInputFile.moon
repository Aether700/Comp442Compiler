
entry
addi r14,r0,topaddr
lw r1, -16(r14)
sw -24(r14), r1
lw r1, -20(r14)
sw -28(r14), r1

%assignment
lw r1, -24(r14)
sw 0(r14), r1
lw r1, -28(r14)
sw -4(r14), r1

% minus expr
addi r2, r0, 05
sw -40(r14), r2
addi r2, r0, 1
sw -44(r14), r2
lw r1, -40(r14)
muli r1, r1, -1
sw -40(r14), r1
lw r2, -40(r14)
sw -48(r14), r2
lw r2, -44(r14)
sw -52(r14), r2

%assignment
lw r2, -48(r14)
sw -8(r14), r2
lw r2, -52(r14)
sw -12(r14), r2

% subtracting floats

% find smallest exponent
lw r2, -4(r14)
lw r3, -12(r14)
% clt r4, r2, 3
bz r4, memAddress0
sub r5, r2, r3
lw r7, 0(r14)
lw r6, -8(r14)
sw -56(r14), r0
j memAddress1
memAddress0 sub r5, r3, r2
lw r7, -8(r14)
lw r6, 0(r14)
addi r11, r0, 1
sw -56(r14), r11

% adjust mantissa
memAddress1 addi r8, r0, 0
memAddress2 clt r9, r8, r5
bz r9, memAddress3
divi r7, r7, 10
addi r8, r8, 1
j memAddress2
memAddress3 
% adjust mantissa position

% count num digits
add r3, r0, r6
addi r2, r0, 1
divi r3, r3, 10
memAddress4 bz r3, memAddress5
divi r3, r3, 10
addi r2, r2, 1
j memAddress4

memAddress5 
% count num digits
add r11, r0, r7
addi r3, r0, 1
divi r11, r11, 10
memAddress6 bz r11, memAddress7
divi r11, r11, 10
addi r3, r3, 1
j memAddress6

memAddress7 sub r5, r3, r2
sw -60(r14), r3
clt r4, r5, r0
bz r4, memAddress8
muli r5, r5, -1
memAddress8 sw -60(r14), r2

% sort what mantissa to use for operation
clt r4, r3, r2
bz r4, memAddress9
add r2, r0, r7
add r3, r0, r6
lw r11, -56(r14)
not r11, r11
sw -56(r14), r11
j memAddress10
memAddress9 add r2, r0, r7
add r3, r0, r6

% multiply shortest mantissa to adjust it's position
memAddress10 addi r8, r0, 0
memAddress11 clt r9, r8, r5
bz r9, memAddress12
muli r2, r2, 10
addi r8, r8, 1
j memAddress11
memAddress12 lw r11, -56(r14)
bz r11, memAddress13
add r11, r0, r2
add r2, r0, r3
add r3, r0, r11
memAddress13 
% perform operation
muli r2, r2, 100
muli r3, r3, 100
sub r10, r2, r3

% compact result
addi r2, r0, 2
memAddress14 modi r11, r10, 10
bnz r11, memAddress15
bz r10, memAddress15
divi r10, r10, 10
subi r2, r2, 1
j memAddress14

% compact exponent if mantissa is 0
memAddress15 bnz r10, memAddress16
lw r2, -56(r14)
muli r2, r2, -1

% store the result
memAddress16 sw -56(r14), r10
lw r10, -60(r14)
add r10, r10, r2
sw -60(r14), r10
lw r4, -56(r14)
sw -64(r14), r4
lw r4, -60(r14)
sw -68(r14), r4

%write stat
lw r4, -64(r14)

addi r14, r14, -72
sw -8(r14), r4
addi r4, r0, buff
sw -12(r14), r4
jl r15, intstr
sw -8(r14), r13
jl r15, putstr

subi r14, r14, -72
addi r4, r0, 42
putc r4
addi r4, r0, 49
putc r4
addi r4, r0, 48
putc r4
addi r4, r0, 94
putc r4
lw r4, -68(r14)

addi r14, r14, -72
sw -8(r14), r4
addi r4, r0, buff
sw -12(r14), r4
jl r15, intstr
sw -8(r14), r13
jl r15, putstr

subi r14, r14, -72
hlt
buff res 20
