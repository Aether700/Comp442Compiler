
entry
addi r14,r0,topaddr
addi r1, r0, 12253
sw -8(r14), r1
addi r1, r0, 3
sw -12(r14), r1

%assignment
lw r1, 0(r14)
sw -8(r14), r1
lw r1, -4(r14)
sw -12(r14), r1

addi r1, r0, 1
sw -16(r14), r1

%write stat
lw r1, -16(r14)

addi r1, r0, 46
sw -8(r14), r1
sw -12(r14), r0
putc r1

subi r14, r14, -20
hlt
buff res 20
