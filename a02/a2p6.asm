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

add $5, $0, $0

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
