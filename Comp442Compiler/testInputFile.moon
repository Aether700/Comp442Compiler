
% start of func "bubbleSort(integer[7], integer)"
funcTag0 sw -120(r14), r15

% storing temporary value of expression

lw r1, -4(r14)
sw -24(r14), r1
% finished storing temporary value of expression

%assignment
lw r1, -4(r14)
sw -8(r14), r1
% assignment statement finished


% storing temporary value of expression

addi r1, r0, 0
sw -28(r14), r1
% finished storing temporary value of expression

%assignment
addi r1, r0, 0
sw -12(r14), r1
% assignment statement finished


% storing temporary value of expression

addi r1, r0, 0
sw -32(r14), r1
% finished storing temporary value of expression

%assignment
addi r1, r0, 0
sw -16(r14), r1
% assignment statement finished


% storing temporary value of expression

addi r1, r0, 0
sw -36(r14), r1
% finished storing temporary value of expression

%assignment
addi r1, r0, 0
sw -20(r14), r1
% assignment statement finished


% computing binary operator value

lw r1, -8(r14)
addi r2, r0, 1
sub r3, r1, r2
sw -40(r14), r3
% finished computing binary operator value

% computing binary operator value

%less than
lw r1, -12(r14)

lw r2, -8(r14)
addi r3, r0, 1
sub r4, r2, r3
sw -40(r14), r4
lw r2, -40(r14)
clt r3, r1, r2
sw -44(r14), r3
% finished computing binary operator value

% storing temporary value of expression


%less than
lw r1, -12(r14)

lw r2, -8(r14)
addi r3, r0, 1
sub r4, r2, r3
sw -40(r14), r4
lw r2, -40(r14)
clt r3, r1, r2
sw -44(r14), r3
lw r1, -44(r14)
sw -48(r14), r1
% finished storing temporary value of expression

%while statement
memAddress4 
%less than
lw r3, -12(r14)

lw r4, -8(r14)
addi r5, r0, 1
sub r6, r4, r5
sw -40(r14), r6
lw r4, -40(r14)
clt r5, r3, r4
sw -44(r14), r5
lw r3, -44(r14)
bz r3, memAddress5

% storing temporary value of expression

addi r1, r0, 0
sw -52(r14), r1
% finished storing temporary value of expression

%assignment
addi r1, r0, 0
sw -16(r14), r1
% assignment statement finished


% computing binary operator value

lw r1, -8(r14)
lw r2, -12(r14)
sub r3, r1, r2
sw -56(r14), r3
% finished computing binary operator value

% computing binary operator value

lw r1, -56(r14)
addi r2, r0, 1
sub r3, r1, r2
sw -60(r14), r3
% finished computing binary operator value

% computing binary operator value

%less than
lw r1, -16(r14)

lw r2, -56(r14)
addi r3, r0, 1
sub r4, r2, r3
sw -60(r14), r4
lw r2, -60(r14)
clt r3, r1, r2
sw -64(r14), r3
% finished computing binary operator value

% storing temporary value of expression


%less than
lw r1, -16(r14)

lw r2, -56(r14)
addi r3, r0, 1
sub r4, r2, r3
sw -60(r14), r4
lw r2, -60(r14)
clt r3, r1, r2
sw -64(r14), r3
lw r1, -64(r14)
sw -68(r14), r1
% finished storing temporary value of expression

%while statement
memAddress2 
%less than
lw r2, -16(r14)

lw r3, -56(r14)
addi r4, r0, 1
sub r5, r3, r4
sw -60(r14), r5
lw r3, -60(r14)
clt r4, r2, r3
sw -64(r14), r4
lw r2, -64(r14)
bz r2, memAddress3

% computing binary operator value

lw r1, -16(r14)
addi r2, r0, 1
add r3, r1, r2
sw -72(r14), r3
% finished computing binary operator value

% computing binary operator value

%greater than

% computing index at runtime
addi r2, r0, 0
lw r3, -16(r14)
muli r3, r3, -4
add r2, r2, r3
addi r2, r2, 0
% finished computing index at runtime

add r2, r2, r14
lw r1, 0(r2)

% computing index at runtime
addi r3, r0, 0

lw r4, -16(r14)
addi r5, r0, 1
add r6, r4, r5
sw -72(r14), r6
lw r4, -72(r14)
muli r4, r4, -4
add r3, r3, r4
addi r3, r3, 0
% finished computing index at runtime

add r3, r3, r14
lw r2, 0(r3)
cgt r3, r1, r2
sw -76(r14), r3
% finished computing binary operator value

% storing temporary value of expression


%greater than

% computing index at runtime
addi r2, r0, 0
lw r3, -16(r14)
muli r3, r3, -4
add r2, r2, r3
addi r2, r2, 0
% finished computing index at runtime

add r2, r2, r14
lw r1, 0(r2)

% computing index at runtime
addi r3, r0, 0

lw r4, -16(r14)
addi r5, r0, 1
add r6, r4, r5
sw -72(r14), r6
lw r4, -72(r14)
muli r4, r4, -4
add r3, r3, r4
addi r3, r3, 0
% finished computing index at runtime

add r3, r3, r14
lw r2, 0(r3)
cgt r3, r1, r2
sw -76(r14), r3
lw r1, -76(r14)
sw -80(r14), r1
% finished storing temporary value of expression

%if statement

%greater than

% computing index at runtime
addi r2, r0, 0
lw r3, -16(r14)
muli r3, r3, -4
add r2, r2, r3
addi r2, r2, 0
% finished computing index at runtime

add r2, r2, r14
lw r1, 0(r2)

% computing index at runtime
addi r3, r0, 0

lw r4, -16(r14)
addi r5, r0, 1
add r6, r4, r5
sw -72(r14), r6
lw r4, -72(r14)
muli r4, r4, -4
add r3, r3, r4
addi r3, r3, 0
% finished computing index at runtime

add r3, r3, r14
lw r2, 0(r3)
cgt r3, r1, r2
sw -76(r14), r3
lw r1, -76(r14)
bz r1, memAddress0
%if block

% storing temporary value of expression


% computing index at runtime
addi r2, r0, 0
lw r3, -16(r14)
muli r3, r3, -4
add r2, r2, r3
addi r2, r2, 0
% finished computing index at runtime

add r2, r2, r14
lw r1, 0(r2)
sw -84(r14), r1
% finished storing temporary value of expression

%assignment

% computing index at runtime
addi r2, r0, 0
lw r3, -16(r14)
muli r3, r3, -4
add r2, r2, r3
addi r2, r2, 0
% finished computing index at runtime

add r2, r2, r14
lw r1, 0(r2)
sw -20(r14), r1
% assignment statement finished


% computing binary operator value

lw r1, -16(r14)
addi r2, r0, 1
add r3, r1, r2
sw -88(r14), r3
% finished computing binary operator value

% storing temporary value of expression


% computing index at runtime
addi r2, r0, 0

lw r3, -16(r14)
addi r4, r0, 1
add r5, r3, r4
sw -88(r14), r5
lw r3, -88(r14)
muli r3, r3, -4
add r2, r2, r3
addi r2, r2, 0
% finished computing index at runtime

add r2, r2, r14
lw r1, 0(r2)
sw -92(r14), r1
% finished storing temporary value of expression

%assignment

% computing index at runtime
addi r2, r0, 0

lw r3, -16(r14)
addi r4, r0, 1
add r5, r3, r4
sw -88(r14), r5
lw r3, -88(r14)
muli r3, r3, -4
add r2, r2, r3
addi r2, r2, 0
% finished computing index at runtime

add r2, r2, r14
lw r1, 0(r2)

% computing index at runtime
addi r2, r0, 0
lw r3, -16(r14)
muli r3, r3, -4
add r2, r2, r3
addi r2, r2, 0
% finished computing index at runtime

add r2, r2, r14
sw 0(r2), r1
% assignment statement finished


% computing binary operator value

lw r1, -16(r14)
addi r2, r0, 1
add r3, r1, r2
sw -96(r14), r3
% finished computing binary operator value

% storing temporary value of expression

lw r1, -20(r14)
sw -100(r14), r1
% finished storing temporary value of expression

%assignment
lw r1, -20(r14)

% computing index at runtime
addi r2, r0, 0

lw r3, -16(r14)
addi r4, r0, 1
add r5, r3, r4
sw -96(r14), r5
lw r3, -96(r14)
muli r3, r3, -4
add r2, r2, r3
addi r2, r2, 0
% finished computing index at runtime

add r2, r2, r14
sw 0(r2), r1
% assignment statement finished

%end if block
j memAddress1
%else block
memAddress0 %end else block
memAddress1 % end of if statement

% computing binary operator value

lw r2, -16(r14)
addi r3, r0, 1
add r4, r2, r3
sw -104(r14), r4
% finished computing binary operator value

% storing temporary value of expression


lw r2, -16(r14)
addi r3, r0, 1
add r4, r2, r3
sw -104(r14), r4
lw r2, -104(r14)
sw -108(r14), r2
% finished storing temporary value of expression

%assignment

lw r2, -16(r14)
addi r3, r0, 1
add r4, r2, r3
sw -104(r14), r4
lw r2, -104(r14)
sw -16(r14), r2
% assignment statement finished

j memAddress2
memAddress3 % end of while statement

% computing binary operator value

lw r3, -12(r14)
addi r4, r0, 1
add r5, r3, r4
sw -112(r14), r5
% finished computing binary operator value

% storing temporary value of expression


lw r3, -12(r14)
addi r4, r0, 1
add r5, r3, r4
sw -112(r14), r5
lw r3, -112(r14)
sw -116(r14), r3
% finished storing temporary value of expression

%assignment

lw r3, -12(r14)
addi r4, r0, 1
add r5, r3, r4
sw -112(r14), r5
lw r3, -112(r14)
sw -12(r14), r3
% assignment statement finished

j memAddress4
memAddress5 % end of while statement
lw r15, -120(r14)
jr r15

% end of func "bubbleSort(integer[7], integer)"

% start of func "printArray(integer[7], integer)"
funcTag1 sw -44(r14), r15

% storing temporary value of expression

lw r4, -4(r14)
sw -16(r14), r4
% finished storing temporary value of expression

%assignment
lw r4, -4(r14)
sw -8(r14), r4
% assignment statement finished


% storing temporary value of expression

addi r4, r0, 0
sw -20(r14), r4
% finished storing temporary value of expression

%assignment
addi r4, r0, 0
sw -12(r14), r4
% assignment statement finished


% computing binary operator value

%less than
lw r4, -12(r14)
lw r5, -8(r14)
clt r6, r4, r5
sw -24(r14), r6
% finished computing binary operator value

% storing temporary value of expression


%less than
lw r4, -12(r14)
lw r5, -8(r14)
clt r6, r4, r5
sw -24(r14), r6
lw r4, -24(r14)
sw -28(r14), r4
% finished storing temporary value of expression

%while statement
memAddress6 
%less than
lw r4, -12(r14)
lw r5, -8(r14)
clt r6, r4, r5
sw -24(r14), r6
lw r4, -24(r14)
bz r4, memAddress7

% storing temporary value of expression


% computing index at runtime
addi r5, r0, 0
lw r6, -12(r14)
muli r6, r6, -4
add r5, r5, r6
addi r5, r5, 0
% finished computing index at runtime

add r5, r5, r14
lw r4, 0(r5)
sw -32(r14), r4
% finished storing temporary value of expression

%write stat

% computing index at runtime
addi r5, r0, 0
lw r6, -12(r14)
muli r6, r6, -4
add r5, r5, r6
addi r5, r5, 0
% finished computing index at runtime

add r5, r5, r14
lw r4, 0(r5)

addi r14, r14, -48
sw -8(r14), r4
addi r4, r0, buff
sw -12(r14), r4
jl r15, intstr
sw -8(r14), r13
jl r15, putstr

subi r14, r14, -48
% write statement finished


% computing binary operator value

lw r4, -12(r14)
addi r5, r0, 1
add r6, r4, r5
sw -36(r14), r6
% finished computing binary operator value

% storing temporary value of expression


lw r4, -12(r14)
addi r5, r0, 1
add r6, r4, r5
sw -36(r14), r6
lw r4, -36(r14)
sw -40(r14), r4
% finished storing temporary value of expression

%assignment

lw r4, -12(r14)
addi r5, r0, 1
add r6, r4, r5
sw -36(r14), r6
lw r4, -36(r14)
sw -12(r14), r4
% assignment statement finished

j memAddress6
memAddress7 % end of while statement
lw r15, -44(r14)
jr r15

% end of func "printArray(integer[7], integer)"

entry
addi r14,r0,topaddr

% storing temporary value of expression

addi r5, r0, 64
sw -28(r14), r5
% finished storing temporary value of expression

%assignment
addi r5, r0, 64

% computing index at runtime
addi r6, r0, 0
addi r7, r0, 0
muli r7, r7, -4
add r6, r6, r7
addi r6, r6, 0
% finished computing index at runtime

add r6, r6, r14
sw 0(r6), r5
% assignment statement finished


% storing temporary value of expression

addi r5, r0, 34
sw -32(r14), r5
% finished storing temporary value of expression

%assignment
addi r5, r0, 34

% computing index at runtime
addi r6, r0, 0
addi r7, r0, 1
muli r7, r7, -4
add r6, r6, r7
addi r6, r6, 0
% finished computing index at runtime

add r6, r6, r14
sw 0(r6), r5
% assignment statement finished


% storing temporary value of expression

addi r5, r0, 25
sw -36(r14), r5
% finished storing temporary value of expression

%assignment
addi r5, r0, 25

% computing index at runtime
addi r6, r0, 0
addi r7, r0, 2
muli r7, r7, -4
add r6, r6, r7
addi r6, r6, 0
% finished computing index at runtime

add r6, r6, r14
sw 0(r6), r5
% assignment statement finished


% storing temporary value of expression

addi r5, r0, 12
sw -40(r14), r5
% finished storing temporary value of expression

%assignment
addi r5, r0, 12

% computing index at runtime
addi r6, r0, 0
addi r7, r0, 3
muli r7, r7, -4
add r6, r6, r7
addi r6, r6, 0
% finished computing index at runtime

add r6, r6, r14
sw 0(r6), r5
% assignment statement finished


% storing temporary value of expression

addi r5, r0, 22
sw -44(r14), r5
% finished storing temporary value of expression

%assignment
addi r5, r0, 22

% computing index at runtime
addi r6, r0, 0
addi r7, r0, 4
muli r7, r7, -4
add r6, r6, r7
addi r6, r6, 0
% finished computing index at runtime

add r6, r6, r14
sw 0(r6), r5
% assignment statement finished


% storing temporary value of expression

addi r5, r0, 11
sw -48(r14), r5
% finished storing temporary value of expression

%assignment
addi r5, r0, 11

% computing index at runtime
addi r6, r0, 0
addi r7, r0, 5
muli r7, r7, -4
add r6, r6, r7
addi r6, r6, 0
% finished computing index at runtime

add r6, r6, r14
sw 0(r6), r5
% assignment statement finished


% storing temporary value of expression

addi r5, r0, 90
sw -52(r14), r5
% finished storing temporary value of expression

%assignment
addi r5, r0, 90

% computing index at runtime
addi r6, r0, 0
addi r7, r0, 6
muli r7, r7, -4
add r6, r6, r7
addi r6, r6, 0
% finished computing index at runtime

add r6, r6, r14
sw 0(r6), r5
% assignment statement finished


% storing temporary value of expression

% copying data
lw r5, 0(r14)
sw -56(r14), r5
lw r5, -4(r14)
sw -60(r14), r5
lw r5, -8(r14)
sw -64(r14), r5
lw r5, -12(r14)
sw -68(r14), r5
lw r5, -16(r14)
sw -72(r14), r5
lw r5, -20(r14)
sw -76(r14), r5
lw r5, -24(r14)
sw -80(r14), r5
% finished copying data
% finished storing temporary value of expression

% storing temporary value of expression

addi r5, r0, 7
sw -56(r14), r5
% finished storing temporary value of expression

% calling function "printArray(integer[7], integer)"
add r5, r14, r0

addi r14, r14, -68

% sending parameters to function

add r6, r0, r5
sw 0(r14), r6
addi r6, r0, 7
sw -4(r14), r6

jl r15, funcTag1

subi r14, r14, -68
% finished calling function "printArray(integer[7], integer)"


% storing temporary value of expression

% copying data
lw r5, 0(r14)
sw -60(r14), r5
lw r5, -4(r14)
sw -64(r14), r5
lw r5, -8(r14)
sw -68(r14), r5
lw r5, -12(r14)
sw -72(r14), r5
lw r5, -16(r14)
sw -76(r14), r5
lw r5, -20(r14)
sw -80(r14), r5
lw r5, -24(r14)
sw -84(r14), r5
% finished copying data
% finished storing temporary value of expression

% storing temporary value of expression

addi r5, r0, 7
sw -60(r14), r5
% finished storing temporary value of expression

% calling function "bubbleSort(integer[7], integer)"
add r5, r14, r0

addi r14, r14, -68

% sending parameters to function

add r6, r0, r5
sw 0(r14), r6
addi r6, r0, 7
sw -4(r14), r6

jl r15, funcTag0

subi r14, r14, -68
% finished calling function "bubbleSort(integer[7], integer)"


% storing temporary value of expression

% copying data
lw r5, 0(r14)
sw -64(r14), r5
lw r5, -4(r14)
sw -68(r14), r5
lw r5, -8(r14)
sw -72(r14), r5
lw r5, -12(r14)
sw -76(r14), r5
lw r5, -16(r14)
sw -80(r14), r5
lw r5, -20(r14)
sw -84(r14), r5
lw r5, -24(r14)
sw -88(r14), r5
% finished copying data
% finished storing temporary value of expression

% storing temporary value of expression

addi r5, r0, 7
sw -64(r14), r5
% finished storing temporary value of expression

% calling function "printArray(integer[7], integer)"
add r5, r14, r0

addi r14, r14, -68

% sending parameters to function

add r6, r0, r5
sw 0(r14), r6
addi r6, r0, 7
sw -4(r14), r6

jl r15, funcTag1

subi r14, r14, -68
% finished calling function "printArray(integer[7], integer)"

hlt
buff res 20
