
% start of func "g()"
funcTag0 sw -16(r14), r15

% storing temporary value of expression

addi r1, r0, 9
sw -4(r14), r1
% finished storing temporary value of expression

%assignment
addi r1, r0, 9
sw 0(r14), r1
% assignment statement finished


% computing binary operator value

lw r1, 0(r14)
addi r2, r0, 3
div r3, r1, r2
sw -8(r14), r3
% finished computing binary operator value

% storing temporary value of expression


lw r1, 0(r14)
addi r2, r0, 3
div r3, r1, r2
sw -8(r14), r3
lw r1, -8(r14)
sw -12(r14), r1
% finished storing temporary value of expression

%write stat

lw r1, 0(r14)
addi r2, r0, 3
div r3, r1, r2
sw -8(r14), r3
lw r1, -8(r14)

addi r14, r14, -20
sw -8(r14), r1
addi r1, r0, buff
sw -12(r14), r1
jl r15, intstr
sw -8(r14), r13
jl r15, putstr

subi r14, r14, -20
% write statement finished

lw r15, -16(r14)
jr r15

% end of func "g()"

% start of func "f()"
funcTag1 sw -16(r14), r15

% storing temporary value of expression

addi r1, r0, 4
sw -4(r14), r1
% finished storing temporary value of expression

%assignment
addi r1, r0, 4
sw 0(r14), r1
% assignment statement finished


% calling function "g()"
add r1, r14, r0

addi r14, r14, -20
jl r15, funcTag0

subi r14, r14, -20
% finished calling function "g()"


% computing binary operator value

lw r1, 0(r14)
addi r2, r0, 2
mul r3, r1, r2
sw -8(r14), r3
% finished computing binary operator value

% storing temporary value of expression


lw r1, 0(r14)
addi r2, r0, 2
mul r3, r1, r2
sw -8(r14), r3
lw r1, -8(r14)
sw -12(r14), r1
% finished storing temporary value of expression

%write stat

lw r1, 0(r14)
addi r2, r0, 2
mul r3, r1, r2
sw -8(r14), r3
lw r1, -8(r14)

addi r14, r14, -20
sw -8(r14), r1
addi r1, r0, buff
sw -12(r14), r1
jl r15, intstr
sw -8(r14), r13
jl r15, putstr

subi r14, r14, -20
% write statement finished

lw r15, -16(r14)
jr r15

% end of func "f()"

entry
addi r14,r0,topaddr

% calling function "f()"
add r1, r14, r0

addi r14, r14, -0
jl r15, funcTag1

subi r14, r14, -0
% finished calling function "f()"

hlt
buff res 20
