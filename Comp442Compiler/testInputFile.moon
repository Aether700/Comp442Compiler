
entry
addi r14,r0,topaddr

addi r1, r0, 1
addi r2, r0, 2
add r3, r1, r2
sw -8(r14), r3

addi r1, r0, 3
addi r2, r0, 2
mul r3, r1, r2
sw -12(r14), r3

lw r1, -8(r14)
lw r2, -12(r14)
sub r3, r1, r2
sw -16(r14), r3

addi r1, r0, 8
addi r2, r0, 2
div r3, r1, r2
sw -20(r14), r3

lw r1, -16(r14)
lw r2, -20(r14)
add r3, r1, r2
sw -24(r14), r3

lw r1, -24(r14)
sw -28(r14), r1

%write stat
lw r1, -28(r14)

addi r14, r14, -24
sw -8(r14), r1
addi r1, r0, buff
sw -12(r14), r1
jl r15, intstr
sw -8(r14), r13
jl r15, putstr

subi r14, r14, -24

%equals
addi r1, r0, 1
addi r2, r0, 1
ceq r3, r1, r2
sw -32(r14), r3

lw r1, -32(r14)
sw -36(r14), r1

%write stat
lw r1, -36(r14)

addi r14, r14, -24
sw -8(r14), r1
addi r1, r0, buff
sw -12(r14), r1
jl r15, intstr
sw -8(r14), r13
jl r15, putstr

subi r14, r14, -24

%not equals
addi r1, r0, 1
addi r2, r0, 0
cne r3, r1, r2
sw -40(r14), r3

lw r1, -40(r14)
sw -44(r14), r1

%write stat
lw r1, -44(r14)

addi r14, r14, -24
sw -8(r14), r1
addi r1, r0, buff
sw -12(r14), r1
jl r15, intstr
sw -8(r14), r13
jl r15, putstr

subi r14, r14, -24

%greater than
addi r1, r0, 1
addi r2, r0, 0
cgt r3, r1, r2
sw -48(r14), r3

lw r1, -48(r14)
sw -52(r14), r1

%write stat
lw r1, -52(r14)

addi r14, r14, -24
sw -8(r14), r1
addi r1, r0, buff
sw -12(r14), r1
jl r15, intstr
sw -8(r14), r13
jl r15, putstr

subi r14, r14, -24

%less than
addi r1, r0, 1
addi r2, r0, 0
clt r3, r1, r2
sw -56(r14), r3

lw r1, -56(r14)
sw -60(r14), r1

%assignment

%less than
addi r1, r0, 1
addi r2, r0, 0
clt r3, r1, r2
sw -56(r14), r3
lw r1, -56(r14)
sw 0(r14), r1

% not
lw r1, -64(r14)
bz r1, memAddress0
addi r2, r0, 0
j memAddress1
memAddress0 addi r2, r0, 1
memAddress1 sw -64(r14), r2

lw r1, -64(r14)
sw -68(r14), r1

%write stat
lw r1, -68(r14)

addi r14, r14, -24
sw -8(r14), r1
addi r1, r0, buff
sw -12(r14), r1
jl r15, intstr
sw -8(r14), r13
jl r15, putstr

subi r14, r14, -24

%less than
addi r1, r0, 1
addi r2, r0, 2
clt r3, r1, r2
sw -72(r14), r3

lw r1, -72(r14)
sw -76(r14), r1

%if statement

%less than
addi r1, r0, 1
addi r2, r0, 2
clt r3, r1, r2
sw -72(r14), r3
lw r1, -72(r14)
bz r1, memAddress2
%if block

addi r1, r0, 1
sw -80(r14), r1

%write stat
lw r1, -80(r14)

addi r14, r14, -24
sw -8(r14), r1
addi r1, r0, buff
sw -12(r14), r1
jl r15, intstr
sw -8(r14), r13
jl r15, putstr

subi r14, r14, -24
%end if block
j memAddress3
%else block
memAddress2 
addi r1, r0, 0
sw -84(r14), r1

%write stat
lw r1, -84(r14)

addi r14, r14, -24
sw -8(r14), r1
addi r1, r0, buff
sw -12(r14), r1
jl r15, intstr
sw -8(r14), r13
jl r15, putstr

subi r14, r14, -24
%end else block
memAddress3 
%greater than
addi r2, r0, 2
addi r3, r0, 3
cgt r4, r2, r3
sw -88(r14), r4

lw r2, -88(r14)
sw -92(r14), r2

%if statement

%greater than
addi r2, r0, 2
addi r3, r0, 3
cgt r4, r2, r3
sw -88(r14), r4
lw r2, -88(r14)
bz r2, memAddress4
%if block

addi r2, r0, 0
sw -96(r14), r2

%write stat
lw r2, -96(r14)

addi r14, r14, -24
sw -8(r14), r2
addi r2, r0, buff
sw -12(r14), r2
jl r15, intstr
sw -8(r14), r13
jl r15, putstr

subi r14, r14, -24
%end if block
j memAddress5
%else block
memAddress4 
addi r2, r0, 1
sw -100(r14), r2

%write stat
lw r2, -100(r14)

addi r14, r14, -24
sw -8(r14), r2
addi r2, r0, buff
sw -12(r14), r2
jl r15, intstr
sw -8(r14), r13
jl r15, putstr

subi r14, r14, -24
%end else block
memAddress5 
addi r3, r0, 3
sw -104(r14), r3

%write stat
lw r3, -104(r14)

addi r14, r14, -24
sw -8(r14), r3
addi r3, r0, buff
sw -12(r14), r3
jl r15, intstr
sw -8(r14), r13
jl r15, putstr

subi r14, r14, -24

addi r3, r0, 0
sw -108(r14), r3

%assignment
addi r3, r0, 0
sw -4(r14), r3

%less than
lw r3, -4(r14)
addi r4, r0, 3
clt r5, r3, r4
sw -112(r14), r5

lw r3, -112(r14)
sw -116(r14), r3

%while statement
memAddress6 
%less than
lw r3, -4(r14)
addi r4, r0, 3
clt r5, r3, r4
sw -112(r14), r5
lw r3, -112(r14)
bz r3, memAddress7

addi r3, r0, 1
sw -120(r14), r3

%write stat
lw r3, -120(r14)

addi r14, r14, -24
sw -8(r14), r3
addi r3, r0, buff
sw -12(r14), r3
jl r15, intstr
sw -8(r14), r13
jl r15, putstr

subi r14, r14, -24

lw r3, -4(r14)
addi r4, r0, 1
add r5, r3, r4
sw -124(r14), r5

lw r3, -124(r14)
sw -128(r14), r3

%assignment

lw r3, -4(r14)
addi r4, r0, 1
add r5, r3, r4
sw -124(r14), r5
lw r3, -124(r14)
sw -4(r14), r3
j memAddress6
memAddress7 
addi r4, r0, 3
sw -132(r14), r4

%write stat
lw r4, -132(r14)

addi r14, r14, -24
sw -8(r14), r4
addi r4, r0, buff
sw -12(r14), r4
jl r15, intstr
sw -8(r14), r13
jl r15, putstr

subi r14, r14, -24

addi r4, r0, 3
sw -136(r14), r4

%write stat
lw r4, -136(r14)

addi r14, r14, -24
sw -8(r14), r4
addi r4, r0, buff
sw -12(r14), r4
jl r15, intstr
sw -8(r14), r13
jl r15, putstr

subi r14, r14, -24

addi r4, r0, 0
sw -140(r14), r4

%assignment
addi r4, r0, 0
sw -4(r14), r4

%greater than
lw r4, -4(r14)
addi r5, r0, 3
cgt r6, r4, r5
sw -144(r14), r6

lw r4, -144(r14)
sw -148(r14), r4

%while statement
memAddress8 
%greater than
lw r4, -4(r14)
addi r5, r0, 3
cgt r6, r4, r5
sw -144(r14), r6
lw r4, -144(r14)
bz r4, memAddress9

addi r4, r0, 1
sw -152(r14), r4

%write stat
lw r4, -152(r14)

addi r14, r14, -24
sw -8(r14), r4
addi r4, r0, buff
sw -12(r14), r4
jl r15, intstr
sw -8(r14), r13
jl r15, putstr

subi r14, r14, -24

lw r4, -4(r14)
addi r5, r0, 1
add r6, r4, r5
sw -156(r14), r6

lw r4, -156(r14)
sw -160(r14), r4

%assignment

lw r4, -4(r14)
addi r5, r0, 1
add r6, r4, r5
sw -156(r14), r6
lw r4, -156(r14)
sw -4(r14), r4
j memAddress8
memAddress9 
addi r5, r0, 3
sw -164(r14), r5

%write stat
lw r5, -164(r14)

addi r14, r14, -24
sw -8(r14), r5
addi r5, r0, buff
sw -12(r14), r5
jl r15, intstr
sw -8(r14), r13
jl r15, putstr

subi r14, r14, -24
hlt
buff res 20
