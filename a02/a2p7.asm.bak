; push $31 on stack
sw $31, -4($30)
sw $1, -8($30)
sw $2, -12($30)
sw $3, -16($30)
lis $31
.word -16
add $30, $30, $31

add $3, $1, $0

PrintNumbersLoop:
beq $2, $0, PrintNumbersExit 

lw $1, 0($3) 
lis $31
.word print
jalr $31 ; call procedure
lis $31
        .word 4
add $3, $3, $31
lis $31
	.word 1
sub $2, $2, $31
beq $0, $0, PrintNumbersLoop

PrintNumbersExit:
; pop $31 from stack
lis $31
.word 16
add $30, $30, $31
lw $31, -4($30)
lw $1, -8($30)
lw $2, -12($30)
lw $3, -16($30)

jr $31
