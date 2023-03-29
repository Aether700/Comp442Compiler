
% start of func "g"
funcTag0 sw -16(r14), r15

addi r1, r0, 9
sw -4(r14), r1

%assignment
addi r1, r0, 9
sw 0(r14), r1

lw r1, 0(r14)
addi r2, r0, 3
div r3, r1, r2
sw -8(r14), r3

lw r1, -8(r14)
sw -12(r14), r1

%write stat
lw r1, -12(r14)

addi r14, r14, -20
sw -8(r14), r1
addi r1, r0, buff
sw -12(r14), r1
jl r15, intstr
sw -8(r14), r13
jl r15, putstr

subi r14, r14, -20
lw r15, -16(r14)
jr r15
% end of func "g"

% start of func "f"
funcTag1 sw -16(r14), r15

addi r1, r0, 4
sw -4(r14), r1

%assignment
addi r1, r0, 4
sw 0(r14), r1

addi r14, r14, -20
jl r15, funcTag0
subi r14, r14, -20

lw r1, 0(r14)
addi r2, r0, 2
mul r3, r1, r2
sw -8(r14), r3

lw r1, -8(r14)
sw -12(r14), r1

%write stat
lw r1, -12(r14)

addi r14, r14, -20
sw -8(r14), r1
addi r1, r0, buff
sw -12(r14), r1
jl r15, intstr
sw -8(r14), r13
jl r15, putstr

subi r14, r14, -20
lw r15, -16(r14)
jr r15
% end of func "f"

entry
addi r14,r0,topaddr

addi r14, r14, -0
jl r15, funcTag1
subi r14, r14, -0
hlt
buff res 20
