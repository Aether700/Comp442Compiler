
entry
addi r14,r0,topaddr

% storing temporary value of expression

addi r1, r0, 1
sw -16(r14), r1
% finished storing temporary value of expression

%assignment
addi r1, r0, 1

% computing offset at runtime of dot expression
addi r2, r0, 0
addi r3, r14, 0

% computing index at runtime
addi r4, r0, 0
addi r5, r0, 0
muli r5, r5, -4
add r4, r4, r5
addi r4, r4, 0
% finished computing index at runtime

add r2, r2, r4
addi r2, r2, 0
addi r3, r2, 0
% finished computing offset at runtime of dot expression

add r3, r3, r14
sw 0(r3), r1
% assignment statement finished


% computing binary operator value

addi r1, r0, 2
addi r3, r0, 3
mul r2, r1, r3
sw -20(r14), r2
% finished computing binary operator value

% computing binary operator value

lw r1, -20(r14)
addi r3, r0, 5
sub r2, r1, r3
sw -24(r14), r2
% finished computing binary operator value

% storing temporary value of expression

addi r1, r0, 2
sw -32(r14), r1
% finished storing temporary value of expression

%assignment
addi r1, r0, 2

% computing offset at runtime of dot expression
addi r3, r0, 0
addi r2, r14, 0

% computing index at runtime
addi r4, r0, 0

lw r5, -20(r14)
addi r6, r0, 5
sub r7, r5, r6
sw -24(r14), r7
lw r5, -24(r14)
muli r5, r5, -4
add r4, r4, r5
addi r4, r4, 0
% finished computing index at runtime

add r3, r3, r4
addi r3, r3, 0
addi r2, r3, 0
% finished computing offset at runtime of dot expression

add r2, r2, r14
sw 0(r2), r1
% assignment statement finished


% storing temporary value of expression

addi r1, r0, 3
sw -40(r14), r1
% finished storing temporary value of expression

%assignment
addi r1, r0, 3

% computing offset at runtime of dot expression
addi r2, r0, 0
addi r3, r14, 0

% computing index at runtime
addi r4, r0, 0
addi r5, r0, 2
muli r5, r5, -4
add r4, r4, r5
addi r4, r4, 0
% finished computing index at runtime

add r2, r2, r4
addi r2, r2, 0
addi r3, r2, 0
% finished computing offset at runtime of dot expression

add r3, r3, r14
sw 0(r3), r1
% assignment statement finished


% storing temporary value of expression


% computing offset at runtime of dot expression
addi r3, r0, 0
addi r2, r14, 0

% computing index at runtime
addi r4, r0, 0
addi r5, r0, 0
muli r5, r5, -4
add r4, r4, r5
addi r4, r4, 0
% finished computing index at runtime

add r3, r3, r4
addi r3, r3, 0
addi r2, r3, 0
% finished computing offset at runtime of dot expression

add r2, r2, r14
lw r1, 0(r2)
sw -48(r14), r1
% finished storing temporary value of expression

%write stat

% computing offset at runtime of dot expression
addi r2, r0, 0
addi r3, r14, 0

% computing index at runtime
addi r4, r0, 0
addi r5, r0, 0
muli r5, r5, -4
add r4, r4, r5
addi r4, r4, 0
% finished computing index at runtime

add r2, r2, r4
addi r2, r2, 0
addi r3, r2, 0
% finished computing offset at runtime of dot expression

add r3, r3, r14
lw r1, 0(r3)

addi r14, r14, -76
sw -8(r14), r1
addi r1, r0, buff
sw -12(r14), r1
jl r15, intstr
sw -8(r14), r13
jl r15, putstr

subi r14, r14, -76
% write statement finished


% computing binary operator value

addi r1, r0, 6
addi r3, r0, 2
div r2, r1, r3
sw -52(r14), r2
% finished computing binary operator value

% computing binary operator value

lw r1, -52(r14)
addi r3, r0, 2
sub r2, r1, r3
sw -56(r14), r2
% finished computing binary operator value

% storing temporary value of expression


% computing offset at runtime of dot expression
addi r3, r0, 0
addi r2, r14, 0

% computing index at runtime
addi r4, r0, 0

lw r5, -52(r14)
addi r6, r0, 2
sub r7, r5, r6
sw -56(r14), r7
lw r5, -56(r14)
muli r5, r5, -4
add r4, r4, r5
addi r4, r4, 0
% finished computing index at runtime

add r3, r3, r4
addi r3, r3, 0
addi r2, r3, 0
% finished computing offset at runtime of dot expression

add r2, r2, r14
lw r1, 0(r2)
sw -64(r14), r1
% finished storing temporary value of expression

%write stat

% computing offset at runtime of dot expression
addi r2, r0, 0
addi r3, r14, 0

% computing index at runtime
addi r4, r0, 0

lw r5, -52(r14)
addi r6, r0, 2
sub r7, r5, r6
sw -56(r14), r7
lw r5, -56(r14)
muli r5, r5, -4
add r4, r4, r5
addi r4, r4, 0
% finished computing index at runtime

add r2, r2, r4
addi r2, r2, 0
addi r3, r2, 0
% finished computing offset at runtime of dot expression

add r3, r3, r14
lw r1, 0(r3)

addi r14, r14, -76
sw -8(r14), r1
addi r1, r0, buff
sw -12(r14), r1
jl r15, intstr
sw -8(r14), r13
jl r15, putstr

subi r14, r14, -76
% write statement finished


% storing temporary value of expression


% computing offset at runtime of dot expression
addi r3, r0, 0
addi r2, r14, 0

% computing index at runtime
addi r4, r0, 0
addi r5, r0, 2
muli r5, r5, -4
add r4, r4, r5
addi r4, r4, 0
% finished computing index at runtime

add r3, r3, r4
addi r3, r3, 0
addi r2, r3, 0
% finished computing offset at runtime of dot expression

add r2, r2, r14
lw r1, 0(r2)
sw -72(r14), r1
% finished storing temporary value of expression

%write stat

% computing offset at runtime of dot expression
addi r2, r0, 0
addi r3, r14, 0

% computing index at runtime
addi r4, r0, 0
addi r5, r0, 2
muli r5, r5, -4
add r4, r4, r5
addi r4, r4, 0
% finished computing index at runtime

add r2, r2, r4
addi r2, r2, 0
addi r3, r2, 0
% finished computing offset at runtime of dot expression

add r3, r3, r14
lw r1, 0(r3)

addi r14, r14, -76
sw -8(r14), r1
addi r1, r0, buff
sw -12(r14), r1
jl r15, intstr
sw -8(r14), r13
jl r15, putstr

subi r14, r14, -76
% write statement finished

hlt
buff res 20
