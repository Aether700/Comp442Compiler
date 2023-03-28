
entry
addi r14,r0,topaddr
addi r1, r14, -8
sw -16(r14), r1

addi r1, r0, 4
sw -20(r14), r1

%assignment
addi r1, r0, 4
sw -8(r14), r1
addi r1, r14, -12
sw -24(r14), r1

addi r1, r0, 5
sw -28(r14), r1

%assignment
addi r1, r0, 5
sw -12(r14), r1
addi r1, r14, 0
sw -32(r14), r1

addi r1, r0, 6
sw -36(r14), r1

%assignment
addi r1, r0, 6
sw 0(r14), r1
addi r1, r14, 0
sw -40(r14), r1

addi r1, r0, 7
sw -44(r14), r1

%assignment
addi r1, r0, 7
sw 0(r14), r1
addi r1, r14, 0
sw -48(r14), r1

lw r2, -48(r14)
lw r1, 0(r2)
sw -52(r14), r1

%write stat
lw r1, -52(r14)

addi r14, r14, -80
sw -8(r14), r1
addi r1, r0, buff
sw -12(r14), r1
jl r15, intstr
sw -8(r14), r13
jl r15, putstr

subi r14, r14, -80
addi r1, r14, 0
sw -56(r14), r1

lw r2, -56(r14)
lw r1, 0(r2)
sw -60(r14), r1

%write stat
lw r1, -60(r14)

addi r14, r14, -80
sw -8(r14), r1
addi r1, r0, buff
sw -12(r14), r1
jl r15, intstr
sw -8(r14), r13
jl r15, putstr

subi r14, r14, -80
addi r1, r14, 0
sw -64(r14), r1
addi r1, r14, -12
sw -68(r14), r1

lw r2, -64(r14)
lw r1, 0(r2)
lw r3, -68(r14)
lw r2, 0(r3)
sub r3, r1, r2
sw -72(r14), r3

lw r1, -72(r14)
sw -76(r14), r1

%write stat
lw r1, -76(r14)

addi r14, r14, -80
sw -8(r14), r1
addi r1, r0, buff
sw -12(r14), r1
jl r15, intstr
sw -8(r14), r13
jl r15, putstr

subi r14, r14, -80
hlt
buff res 20
