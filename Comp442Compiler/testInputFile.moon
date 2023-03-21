entry
addi r14,r0,100
addi r14,r14,-24
addi r1,r0,3
sw -8(r14),r1
addi r1,r0,buff
sw -12(r14),r1
jl r15, intstr
sw -8(r14),r13
jl r15, putstr
subi r14,r14,-24
hlt
buf res 20
