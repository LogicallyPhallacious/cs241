lis $3
.word 0
sw $31, 0xfffc($30)
lis $31
.word 0
lw $31, 0($31)
lw $31, 0($31)
lw $31, 0($31)
sub $30, $30, $31
jalr $3
lis $31
.word 0
lw $31, 0($31)
lw $31, 0($31)
lw $31, 0($31)
add $30, $30, $31
lw $31,0xfffc($30)
jr $31
.word 4

