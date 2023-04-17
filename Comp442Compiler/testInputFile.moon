
% start of func "A::constructor(integer)"
funcTag5 sw -4(r14), r15

% storing temporary value of expression

lw r1, 0(r14)
sw -20(r14), r1
% finished storing temporary value of expression

%assignment
lw r1, 0(r14)
sw -32(r14), r1
% assignment statement finished


% storing temporary value of expression

lw r1, 0(r14)
sw -28(r14), r1
% finished storing temporary value of expression

%assignment
lw r1, 0(r14)
sw -36(r14), r1
% assignment statement finished

addi r13, r14, -8
lw r15, -4(r14)
jr r15

% end of func "A::constructor(integer)"

% start of func "A::constructor(integer, integer)"
funcTag3 sw -8(r14), r15

% storing temporary value of expression

lw r1, 0(r14)
sw -24(r14), r1
% finished storing temporary value of expression

%assignment
lw r1, 0(r14)
sw -36(r14), r1
% assignment statement finished


% storing temporary value of expression

lw r1, -4(r14)
sw -32(r14), r1
% finished storing temporary value of expression

%assignment
lw r1, -4(r14)
sw -40(r14), r1
% assignment statement finished

addi r13, r14, -12
lw r15, -8(r14)
jr r15

% end of func "A::constructor(integer, integer)"

entry
addi r14,r0,topaddr

% storing temporary value of expression

addi r1, r0, 15
sw -16(r14), r1
% finished storing temporary value of expression


% calling constructor "A::constructor(integer)"
add r1, r14, r0

addi r14, r14, -44

% sending parameters to function

addi r2, r0, 15
sw 0(r14), r2

jl r15, funcTag5

% copying data
lw r1, -32(r14)
sw -8(r14), r1
lw r1, -36(r14)
sw -12(r14), r1
% finished copying data

subi r14, r14, -44

% copy data at reference
lw r1, -0(r13)
sw 0(r14), r1
lw r1, -4(r13)
sw -4(r14), r1
% finished calling constructor "A::constructor(integer)"


% storing temporary value of expression

addi r1, r0, 5
sw -20(r14), r1
% finished storing temporary value of expression

% storing temporary value of expression

addi r1, r0, 3
sw -24(r14), r1
% finished storing temporary value of expression


% calling constructor "A::constructor(integer, integer)"
add r1, r14, r0

addi r14, r14, -44

% sending parameters to function

addi r2, r0, 5
sw 0(r14), r2
addi r2, r0, 3
sw -4(r14), r2

jl r15, funcTag3

% copying data
lw r1, -36(r14)
sw -12(r14), r1
lw r1, -40(r14)
sw -16(r14), r1
% finished copying data

subi r14, r14, -44

% copy data at reference
lw r1, -0(r13)
sw -8(r14), r1
lw r1, -4(r13)
sw -12(r14), r1
% finished calling constructor "A::constructor(integer, integer)"


% storing temporary value of expression

addi r2, r14, 0
lw r1, -4(r2)
sw -32(r14), r1
% finished storing temporary value of expression

%write stat
addi r2, r14, 0
lw r1, -4(r2)

addi r14, r14, -44
sw -8(r14), r1
addi r1, r0, buff
sw -12(r14), r1
jl r15, intstr
sw -8(r14), r13
jl r15, putstr

subi r14, r14, -44
% write statement finished


% storing temporary value of expression

addi r2, r14, 0
lw r1, -12(r2)
sw -40(r14), r1
% finished storing temporary value of expression

%write stat
addi r2, r14, 0
lw r1, -12(r2)

addi r14, r14, -44
sw -8(r14), r1
addi r1, r0, buff
sw -12(r14), r1
jl r15, intstr
sw -8(r14), r13
jl r15, putstr

subi r14, r14, -44
% write statement finished

hlt
buff res 20
