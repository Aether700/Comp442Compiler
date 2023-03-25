
entry
addi r14,r0,topaddr

addi r1, r0, 1
addi r2, r0, 2
add r3, r1, r2
sw -80(r14), r3

addi r1, r0, 3
addi r2, r0, 2
mul r3, r1, r2
sw -84(r14), r3

lw r1, -80(r14)
lw r2, -84(r14)
sub r3, r1, r2
sw -88(r14), r3

addi r1, r0, 8
addi r2, r0, 2
div r3, r1, r2
sw -92(r14), r3

lw r1, -88(r14)
lw r2, -92(r14)
add r3, r1, r2
sw -96(r14), r3

lw r1, -96(r14)
sw -100(r14), r1

%write stat
lw r1, -100(r14)

addi r14, r14, -292
sw -8(r14), r1
addi r1, r0, buff
sw -12(r14), r1
jl r15, intstr
sw -8(r14), r13
jl r15, putstr

subi r14, r14, -292

%equals
addi r1, r0, 1
addi r2, r0, 1
ceq r3, r1, r2
sw -104(r14), r3

lw r1, -104(r14)
sw -108(r14), r1

%write stat
lw r1, -108(r14)

addi r14, r14, -292
sw -8(r14), r1
addi r1, r0, buff
sw -12(r14), r1
jl r15, intstr
sw -8(r14), r13
jl r15, putstr

subi r14, r14, -292

%not equals
addi r1, r0, 1
addi r2, r0, 0
cne r3, r1, r2
sw -112(r14), r3

lw r1, -112(r14)
sw -116(r14), r1

%write stat
lw r1, -116(r14)

addi r14, r14, -292
sw -8(r14), r1
addi r1, r0, buff
sw -12(r14), r1
jl r15, intstr
sw -8(r14), r13
jl r15, putstr

subi r14, r14, -292

%greater than
addi r1, r0, 1
addi r2, r0, 0
cgt r3, r1, r2
sw -120(r14), r3

lw r1, -120(r14)
sw -124(r14), r1

%write stat
lw r1, -124(r14)

addi r14, r14, -292
sw -8(r14), r1
addi r1, r0, buff
sw -12(r14), r1
jl r15, intstr
sw -8(r14), r13
jl r15, putstr

subi r14, r14, -292

%less than
addi r1, r0, 1
addi r2, r0, 0
clt r3, r1, r2
sw -128(r14), r3

lw r1, -128(r14)
sw -132(r14), r1

%assignment

%less than
addi r1, r0, 1
addi r2, r0, 0
clt r3, r1, r2
sw -128(r14), r3
lw r1, -128(r14)
sw 0(r14), r1

% not
lw r1, -136(r14)
bz r1, memAddress0
addi r2, r0, 0
j memAddress1
memAddress0 addi r2, r0, 1
memAddress1 sw -136(r14), r2

lw r1, -136(r14)
sw -140(r14), r1

%write stat
lw r1, -140(r14)

addi r14, r14, -292
sw -8(r14), r1
addi r1, r0, buff
sw -12(r14), r1
jl r15, intstr
sw -8(r14), r13
jl r15, putstr

subi r14, r14, -292

%less than
addi r1, r0, 1
addi r2, r0, 2
clt r3, r1, r2
sw -144(r14), r3

lw r1, -144(r14)
sw -148(r14), r1

%if statement

%less than
addi r1, r0, 1
addi r2, r0, 2
clt r3, r1, r2
sw -144(r14), r3
lw r1, -144(r14)
bz r1, memAddress2
%if block

addi r1, r0, 1
sw -152(r14), r1

%write stat
lw r1, -152(r14)

addi r14, r14, -292
sw -8(r14), r1
addi r1, r0, buff
sw -12(r14), r1
jl r15, intstr
sw -8(r14), r13
jl r15, putstr

subi r14, r14, -292
%end if block
j memAddress3
%else block
memAddress2 
addi r1, r0, 0
sw -156(r14), r1

%write stat
lw r1, -156(r14)

addi r14, r14, -292
sw -8(r14), r1
addi r1, r0, buff
sw -12(r14), r1
jl r15, intstr
sw -8(r14), r13
jl r15, putstr

subi r14, r14, -292
%end else block
memAddress3 
%greater than
addi r2, r0, 2
addi r3, r0, 3
cgt r4, r2, r3
sw -160(r14), r4

lw r2, -160(r14)
sw -164(r14), r2

%if statement

%greater than
addi r2, r0, 2
addi r3, r0, 3
cgt r4, r2, r3
sw -160(r14), r4
lw r2, -160(r14)
bz r2, memAddress4
%if block

addi r2, r0, 0
sw -168(r14), r2

%write stat
lw r2, -168(r14)

addi r14, r14, -292
sw -8(r14), r2
addi r2, r0, buff
sw -12(r14), r2
jl r15, intstr
sw -8(r14), r13
jl r15, putstr

subi r14, r14, -292
%end if block
j memAddress5
%else block
memAddress4 
addi r2, r0, 1
sw -172(r14), r2

%write stat
lw r2, -172(r14)

addi r14, r14, -292
sw -8(r14), r2
addi r2, r0, buff
sw -12(r14), r2
jl r15, intstr
sw -8(r14), r13
jl r15, putstr

subi r14, r14, -292
%end else block
memAddress5 
addi r3, r0, 3
sw -176(r14), r3

%write stat
lw r3, -176(r14)

addi r14, r14, -292
sw -8(r14), r3
addi r3, r0, buff
sw -12(r14), r3
jl r15, intstr
sw -8(r14), r13
jl r15, putstr

subi r14, r14, -292

addi r3, r0, 0
sw -180(r14), r3

%assignment
addi r3, r0, 0
sw -4(r14), r3

%less than
lw r3, -4(r14)
addi r4, r0, 3
clt r5, r3, r4
sw -184(r14), r5

lw r3, -184(r14)
sw -188(r14), r3

%while statement
memAddress6 
%less than
lw r3, -4(r14)
addi r4, r0, 3
clt r5, r3, r4
sw -184(r14), r5
lw r3, -184(r14)
bz r3, memAddress7

addi r3, r0, 1
sw -192(r14), r3

%write stat
lw r3, -192(r14)

addi r14, r14, -292
sw -8(r14), r3
addi r3, r0, buff
sw -12(r14), r3
jl r15, intstr
sw -8(r14), r13
jl r15, putstr

subi r14, r14, -292

lw r3, -4(r14)
addi r4, r0, 1
add r5, r3, r4
sw -196(r14), r5

lw r3, -196(r14)
sw -200(r14), r3

%assignment

lw r3, -4(r14)
addi r4, r0, 1
add r5, r3, r4
sw -196(r14), r5
lw r3, -196(r14)
sw -4(r14), r3
j memAddress6
memAddress7 
addi r4, r0, 3
sw -204(r14), r4

%write stat
lw r4, -204(r14)

addi r14, r14, -292
sw -8(r14), r4
addi r4, r0, buff
sw -12(r14), r4
jl r15, intstr
sw -8(r14), r13
jl r15, putstr

subi r14, r14, -292

addi r4, r0, 2
sw -208(r14), r4

%write stat
lw r4, -208(r14)

addi r14, r14, -292
sw -8(r14), r4
addi r4, r0, buff
sw -12(r14), r4
jl r15, intstr
sw -8(r14), r13
jl r15, putstr

subi r14, r14, -292

addi r4, r0, 0
sw -212(r14), r4

%assignment
addi r4, r0, 0
sw -4(r14), r4

%greater than
lw r4, -4(r14)
addi r5, r0, 3
cgt r6, r4, r5
sw -216(r14), r6

lw r4, -216(r14)
sw -220(r14), r4

%while statement
memAddress8 
%greater than
lw r4, -4(r14)
addi r5, r0, 3
cgt r6, r4, r5
sw -216(r14), r6
lw r4, -216(r14)
bz r4, memAddress9

addi r4, r0, 1
sw -224(r14), r4

%write stat
lw r4, -224(r14)

addi r14, r14, -292
sw -8(r14), r4
addi r4, r0, buff
sw -12(r14), r4
jl r15, intstr
sw -8(r14), r13
jl r15, putstr

subi r14, r14, -292

lw r4, -4(r14)
addi r5, r0, 1
add r6, r4, r5
sw -228(r14), r6

lw r4, -228(r14)
sw -232(r14), r4

%assignment

lw r4, -4(r14)
addi r5, r0, 1
add r6, r4, r5
sw -228(r14), r6
lw r4, -228(r14)
sw -4(r14), r4
j memAddress8
memAddress9 
addi r5, r0, 2
sw -236(r14), r5

%write stat
lw r5, -236(r14)

addi r14, r14, -292
sw -8(r14), r5
addi r5, r0, buff
sw -12(r14), r5
jl r15, intstr
sw -8(r14), r13
jl r15, putstr

subi r14, r14, -292

addi r5, r0, 2
sw -240(r14), r5

%assignment
addi r5, r0, 2
sw -8(r14), r5

addi r5, r0, 3
sw -244(r14), r5

%assignment
addi r5, r0, 3
sw -12(r14), r5

addi r5, r0, 1
sw -248(r14), r5

%assignment
addi r5, r0, 1
sw -16(r14), r5

lw r5, -16(r14)
sw -252(r14), r5

%write stat
lw r5, -252(r14)

addi r14, r14, -292
sw -8(r14), r5
addi r5, r0, buff
sw -12(r14), r5
jl r15, intstr
sw -8(r14), r13
jl r15, putstr

subi r14, r14, -292

lw r5, -12(r14)
lw r6, -8(r14)
sub r7, r5, r6
sw -256(r14), r7

lw r5, -256(r14)
sw -260(r14), r5

%write stat
lw r5, -260(r14)

addi r14, r14, -292
sw -8(r14), r5
addi r5, r0, buff
sw -12(r14), r5
jl r15, intstr
sw -8(r14), r13
jl r15, putstr

subi r14, r14, -292

addi r5, r0, 0
sw -264(r14), r5

%assignment
addi r5, r0, 0
sw -48(r14), r5

addi r5, r0, 4
sw -268(r14), r5

%assignment
addi r5, r0, 4
sw -40(r14), r5

% minus expr
addi r5, r0, 3
muli r6, r5, -1
sw -272(r14), r6

lw r6, -272(r14)
sw -276(r14), r6

% minus expr
addi r6, r0, 3
muli r7, r6, -1
sw -272(r14), r7

%assignment
lw r7, -272(r14)
sw -56(r14), r7

lw r7, -48(r14)
lw r8, -40(r14)
add r9, r7, r8
sw -280(r14), r9

lw r7, -280(r14)
lw r8, -56(r14)
add r9, r7, r8
sw -284(r14), r9

lw r7, -284(r14)
sw -288(r14), r7

%write stat
lw r7, -288(r14)

addi r14, r14, -292
sw -8(r14), r7
addi r7, r0, buff
sw -12(r14), r7
jl r15, intstr
sw -8(r14), r13
jl r15, putstr

subi r14, r14, -292
hlt
buff res 20
