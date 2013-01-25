beq $0, $0, 2
.word endmoduleA5P1
.word endcodeA5P1

lis $3
rel1: .word abc
sw $31, 0xfffc($30)
lis $31
rel2: .word def
lw $31, 0($31)
lw $31, 0($31)
lw $31, 0($31)
sub $30, $30, $31
jalr $3
lis $31
rel3: .word def
lw $31, 0($31)
lw $31, 0($31)
lw $31, 0($31)
add $30, $30, $31
lw $31,0xfffc($30)
jr $31
ghi: .word 4

def: .word rel4
abc: jr $31
rel4: .word ghi

endcodeA5P1:

.word 1
.word def

.word 1
.word rel1

.word 1
.word rel2

.word 1
.word rel3

.word 1
.word rel4


.word 0x5
.word ghi
.word 3
.word 0x67 ;g
.word 0x68 ;h
.word 0x69 ;i

.word 0x5
.word def
.word 3
.word 0x64 ;d
.word 0x65 ;e
.word 0x66 ;f

.word 0x5
.word abc
.word 3
.word 0x61 ;a
.word 0x62 ;b
.word 0x63 ;c

endmoduleA5P1:
