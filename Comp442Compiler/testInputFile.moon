
% start of func "CreateA()"
funcTag0 sw -48(r14), r15
addi r1, r14, 0
sw -12(r14), r1

addi r1, r0, 3
sw -16(r14), r1

%assignment
addi r1, r0, 3
sw 0(r14), r1
addi r1, r14, -4
sw -20(r14), r1

addi r1, r0, 7
sw -24(r14), r1

%assignment
addi r1, r0, 7
sw -4(r14), r1
addi r1, r14, -8
sw -28(r14), r1

addi r1, r0, 36
sw -32(r14), r1

%assignment
addi r1, r0, 36
sw -8(r14), r1
lw r1, 0(r14)
sw -36(r14), r1
lw r1, -4(r14)
sw -40(r14), r1
lw r1, -8(r14)
sw -44(r14), r1
lw r1, -36(r14)
sw -52(r14), r1
lw r1, -40(r14)
sw -56(r14), r1
lw r1, -44(r14)
sw -60(r14), r1
addi r13, r14, -52
lw r15, -48(r14)
jr r15

% end of func "CreateA()"

% start of func "f(integer)"
funcTag1 sw -8(r14), r15

lw r1, 0(r14)
sw -4(r14), r1

%write stat
lw r1, -4(r14)

addi r14, r14, -12
sw -8(r14), r1
addi r1, r0, buff
sw -12(r14), r1
jl r15, intstr
sw -8(r14), r13
jl r15, putstr

subi r14, r14, -12
lw r15, -8(r14)
jr r15

% end of func "f(integer)"

entry
addi r14,r0,topaddr
lw r1, -12(r14)
sw -24(r14), r1
lw r1, -16(r14)
sw -28(r14), r1
lw r1, -20(r14)
sw -32(r14), r1

%assignment

% calling function "CreateA()"

addi r14, r14, -44
jl r15, funcTag0
subi r14, r14, -44

% handle return value

% copy data at reference
lw r1, -0(r13)
sw -12(r14), r1
lw r1, -4(r13)
sw -16(r14), r1
lw r1, -8(r13)
sw -20(r14), r1
% finished calling function "CreateA()"

lw r1, -12(r14)
sw 0(r14), r1
lw r1, -16(r14)
sw -4(r14), r1
lw r1, -20(r14)
sw -8(r14), r1
lw r1, -24(r14)
sw -12(r14), r1
lw r1, -28(r14)
sw -16(r14), r1
lw r1, -32(r14)
sw -20(r14), r1
lw r1, -36(r14)
sw -24(r14), r1
lw r1, -40(r14)
sw -28(r14), r1
lw r1, -44(r14)
sw -32(r14), r1
lw r1, -48(r14)
sw -36(r14), r1
lw r1, -52(r14)
sw -40(r14), r1
addi r1, r14, 0
sw -36(r14), r1

lw r2, -36(r14)
lw r1, 0(r2)
sw -40(r14), r1

% calling function "f(integer)"

addi r14, r14, -44

% sending parameters to function

lw r2, -36(r14)
lw r1, 0(r2)
sw 0(r14), r1

jl r15, funcTag1
subi r14, r14, -44
% finished calling function "f(integer)"

hlt
buff res 20
