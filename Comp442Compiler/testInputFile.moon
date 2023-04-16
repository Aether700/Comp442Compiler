
% start of func "f()"
funcTag0 sw 0(r14), r15

% storing temporary value of expression

addi r1, r0, 5
sw -12(r14), r1
% finished storing temporary value of expression

%assignment
addi r1, r0, 5
lw r2, -4(r14)
sw 0(r2), r1
% assignment statement finished

lw r15, 0(r14)
jr r15

% end of func "f()"

entry
addi r14,r0,topaddr

% calling member function
addi r1, r14, 0

% calling function "A::f()"
addi r2, r14, 0
add r3, r14, r0

addi r14, r14, -8
sw -4(r14), r2
jl r15, funcTag0

subi r14, r14, -8
% finished calling function "A::f()"

% finished calling member function

hlt
buff res 20
