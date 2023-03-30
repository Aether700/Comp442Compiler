
% start of func "f(A)"
funcTag0 sw -12(r14), r15
addi r1, r14, 0
sw -4(r14), r1

lw r2, 0(r14)
lw r1, 0(r2)
sw -8(r14), r1

%write stat
lw r2, 0(r14)
lw r1, 0(r2)

addi r14, r14, -16
sw -8(r14), r1
addi r1, r0, buff
sw -12(r14), r1
jl r15, intstr
sw -8(r14), r13
jl r15, putstr

subi r14, r14, -16
lw r15, -12(r14)
jr r15

% end of func "f(A)"

entry
addi r14,r0,topaddr
addi r1, r14, 0
sw -64(r14), r1

addi r1, r0, 5
sw -68(r14), r1

%assignment
addi r1, r0, 5
sw 0(r14), r1
addi r1, r14, -4
sw -72(r14), r1

addi r1, r0, 7
sw -76(r14), r1

%assignment
addi r1, r0, 7
sw -4(r14), r1
addi r1, r14, -8
sw -80(r14), r1

addi r1, r0, 12
sw -84(r14), r1

%assignment
addi r1, r0, 12
sw -8(r14), r1
addi r1, r14, 0
sw -88(r14), r1

addi r1, r0, 12
sw -92(r14), r1

%assignment
addi r1, r0, 12
sw 0(r14), r1
lw r1, 0(r14)
sw -96(r14), r1
lw r1, -4(r14)
sw -100(r14), r1
lw r1, -8(r14)
sw -104(r14), r1
lw r1, -12(r14)
sw -108(r14), r1

% calling function "f(A)"
add r1, r14, r0

addi r14, r14, -112

% sending parameters to function

add r2, r0, r1
sw 0(r14), r2

jl r15, funcTag0
subi r14, r14, -112
% finished calling function "f(A)"

hlt
buff res 20
