
% start of func "CreateA"
funcTag0 sw -32(r14), r15
addi r1, r14, 0
sw -8(r14), r1

addi r1, r0, 3
sw -12(r14), r1

%assignment
addi r1, r0, 3
sw 0(r14), r1
addi r1, r14, -4
sw -16(r14), r1

addi r1, r0, 7
sw -20(r14), r1

%assignment
addi r1, r0, 7
sw -4(r14), r1
lw r1, 0(r14)
sw -24(r14), r1
lw r1, -4(r14)
sw -28(r14), r1
lw r1, -24(r14)
sw -36(r14), r1
lw r1, -28(r14)
sw -40(r14), r1
addi r13, r14, -36
lw r15, -32(r14)
jr r15
% end of func "CreateA"

entry
addi r14,r0,topaddr
lw r1, -8(r14)
sw -16(r14), r1
lw r1, -12(r14)
sw -20(r14), r1

%assignment

addi r14, r14, -32
jl r15, funcTag0
subi r14, r14, -32

% handle return value

% copy data at reference
lw r1, 0(r13)
sw -8(r14), r1
lw r1, 4(r13)
sw -12(r14), r1

lw r1, -8(r14)
sw 0(r14), r1
lw r1, -12(r14)
sw -4(r14), r1
addi r1, r14, -4
sw -24(r14), r1

lw r2, -24(r14)
lw r1, 0(r2)
sw -28(r14), r1

%write stat
lw r1, -28(r14)

addi r14, r14, -32
sw -8(r14), r1
addi r1, r0, buff
sw -12(r14), r1
jl r15, intstr
sw -8(r14), r13
jl r15, putstr

subi r14, r14, -32
hlt
buff res 20
