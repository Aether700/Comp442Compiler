
entry
addi r14,r0,topaddr

addi r1, r0, 1
sw -12(r14), r1

%assignment
addi r1, r0, 1

% computing index at runtime
addi r2, r0, 0
addi r3, r0, 0
muli r3, r3, -4
add r2, r2, r3
addi r2, r2, 0
% finished computing index at runtime

add r2, r2, r14
sw 0(r2), r1

addi r1, r0, 2
addi r2, r0, 3
mul r3, r1, r2
sw -16(r14), r3

lw r1, -16(r14)
addi r2, r0, 5
sub r3, r1, r2
sw -20(r14), r3

addi r1, r0, 2
sw -24(r14), r1

%assignment
addi r1, r0, 2

% computing index at runtime
addi r2, r0, 0

lw r3, -16(r14)
addi r4, r0, 5
sub r5, r3, r4
sw -20(r14), r5
lw r3, -20(r14)
muli r3, r3, -4
add r2, r2, r3
addi r2, r2, 0
% finished computing index at runtime

add r2, r2, r14
sw 0(r2), r1

addi r1, r0, 3
sw -28(r14), r1

%assignment
addi r1, r0, 3

% computing index at runtime
addi r2, r0, 0
addi r3, r0, 2
muli r3, r3, -4
add r2, r2, r3
addi r2, r2, 0
% finished computing index at runtime

add r2, r2, r14
sw 0(r2), r1


% computing index at runtime
addi r2, r0, 0
addi r3, r0, 0
muli r3, r3, -4
add r2, r2, r3
addi r2, r2, 0
% finished computing index at runtime

add r2, r2, r14
lw r1, 0(r2)
sw -32(r14), r1

%write stat

% computing index at runtime
addi r2, r0, 0
addi r3, r0, 0
muli r3, r3, -4
add r2, r2, r3
addi r2, r2, 0
% finished computing index at runtime

add r2, r2, r14
lw r1, 0(r2)

addi r14, r14, -52
sw -8(r14), r1
addi r1, r0, buff
sw -12(r14), r1
jl r15, intstr
sw -8(r14), r13
jl r15, putstr

subi r14, r14, -52

addi r1, r0, 6
addi r2, r0, 2
div r3, r1, r2
sw -36(r14), r3

lw r1, -36(r14)
addi r2, r0, 2
sub r3, r1, r2
sw -40(r14), r3


% computing index at runtime
addi r2, r0, 0

lw r3, -36(r14)
addi r4, r0, 2
sub r5, r3, r4
sw -40(r14), r5
lw r3, -40(r14)
muli r3, r3, -4
add r2, r2, r3
addi r2, r2, 0
% finished computing index at runtime

add r2, r2, r14
lw r1, 0(r2)
sw -44(r14), r1

%write stat

% computing index at runtime
addi r2, r0, 0

lw r3, -36(r14)
addi r4, r0, 2
sub r5, r3, r4
sw -40(r14), r5
lw r3, -40(r14)
muli r3, r3, -4
add r2, r2, r3
addi r2, r2, 0
% finished computing index at runtime

add r2, r2, r14
lw r1, 0(r2)

addi r14, r14, -52
sw -8(r14), r1
addi r1, r0, buff
sw -12(r14), r1
jl r15, intstr
sw -8(r14), r13
jl r15, putstr

subi r14, r14, -52


% computing index at runtime
addi r2, r0, 0
addi r3, r0, 2
muli r3, r3, -4
add r2, r2, r3
addi r2, r2, 0
% finished computing index at runtime

add r2, r2, r14
lw r1, 0(r2)
sw -48(r14), r1

%write stat

% computing index at runtime
addi r2, r0, 0
addi r3, r0, 2
muli r3, r3, -4
add r2, r2, r3
addi r2, r2, 0
% finished computing index at runtime

add r2, r2, r14
lw r1, 0(r2)

addi r14, r14, -52
sw -8(r14), r1
addi r1, r0, buff
sw -12(r14), r1
jl r15, intstr
sw -8(r14), r13
jl r15, putstr

subi r14, r14, -52
hlt
buff res 20
