; push $31 on stack
sw $31, -4($30)
sw $1, -8($30)
sw $2, -12($30)
sw $3, -16($30)
lis $31
.word -16
add $30, $30, $31


lis $31
	.word 0xff
add $4, $31, $0
add $5, $31, $0
add $6, $31, $0
add $7, $31, $0
add $8, $31, $0
add $9, $31, $0
add $10, $31, $0
add $11, $1, $0
add $12, $2, $0
add $13, $3, $0

add $15, $30, $0

lis $30
	.word 0x1200
add $14, $30, $0

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

add $16, $30, $0
add $30, $15, $0

lis $31
.word 16
add $30, $30, $31
lw $31, -4($30)
lw $1, -8($30)
lw $2, -12($30)
lw $3, -16($30)

jr $31
print:
sw $1, -4($30)
sw $2, -8($30)
sw $3, -12($30)
sw $4, -16($30)
sw $5, -20($30)

lis $2
	.word 20
sub $30, $30, $2

lis $2
        .word 0x80000000
lis $3
        .word 0xffff000c

bne $1, $2, START

lis $1
	.word 0xF3333334
lis $5
	.word 1

START:

add $4, $30, $0

slt $3, $1, $0
beq $3, $0, LOOP


lis $2
        .word 0xffff000c
lis $3
        .word 0x2d  ; minus sign
sw $3, 0($2)
lis $2
        .word -1
mult $1, $2
mflo $1

LOOP:
	lis $2
		.word 10
        div $1, $2
        mfhi $3

        sw $3, -4($30)

        div $1, $2
        mflo $1
        beq $1, $0, ENDLOOP
	
	lis $2
		.word 4
        sub $30, $30, $2
        beq $0, $0, LOOP
        
ENDLOOP:
	lis $1
		.word 0x30
        lw $3, -4($30)
        add $3, $3, $1
        
	lis $1
		.word 0xffff000c
        sw $3, 0($1)

	lis $2
		.word 4

        beq $30, $4, EXIT
        add $30, $30, $2
        beq $0, $0, ENDLOOP

EXIT:
	beq $5, $0, EXIT2
	        lis $1
                .word 0xffff000c
	lis $2
		.word 0x38
	sw $2, 0($1)
EXIT2:
	lis $1
		.word 0xffff000c
        lis $3
                .word 10
        sw $3, 0($1)
        
	lis $1
		.word 20
	add $30, $30, $1
	lw $1, -4($30)
	lw $2, -8($30)
	lw $3, -12($30)
	lw $4, -16($30)
	lw $5, -20($30)	

	jr $31
